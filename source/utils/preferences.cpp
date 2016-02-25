#include "preferences.h"

#include <QSettings>
#include <QMetaProperty>
#include <QTimerEvent>
#include <QRegularExpression>

#include "../utils/utils.h"

void Preferences::define(const QString& key, const QVariant& defaultValue,
                         const QVariant& minimumValue, const QVariant& maximumValue)
{
    if(minimumValue.isValid() && _minimumValue[key] != minimumValue)
    {
        _minimumValue[key] = minimumValue;
        emit minimumChanged(key, minimumValue);
    }

    if(maximumValue.isValid() && _maximumValue[key] != maximumValue)
    {
        _maximumValue[key] = maximumValue;
        emit maximumChanged(key, maximumValue);
    }

    if(!exists(key))
        set(key, defaultValue);
}

QVariant Preferences::get(const QString& key)
{
    define(key);

    return _settings.value(key);
}

QVariant Preferences::minimum(const QString& key) const
{
    if(u::contains(_minimumValue, key))
        return _minimumValue.at(key);

    return {};
}

QVariant Preferences::maximum(const QString& key) const
{
    if(u::contains(_maximumValue, key))
        return _maximumValue.at(key);

    return {};
}

void Preferences::set(const QString& key, QVariant value)
{
    bool changed = (value != _settings.value(key));

    auto minimumValue = minimum(key);
    if(minimumValue.canConvert(value.type()))
    {
        minimumValue.convert(value.type());
        value = std::max(value, minimumValue);
    }

    auto maximumValue = maximum(key);
    if(maximumValue.canConvert(value.type()))
    {
        maximumValue.convert(value.type());
        value = std::min(value, maximumValue);
    }

    _settings.setValue(key, value);

    if(changed)
        emit preferenceChanged(key, value);
}

bool Preferences::exists(const QString& key)
{
    return _settings.contains(key);
}

QmlPreferences::QmlPreferences(QObject* parent) :
    QObject(parent)
{
    connect($(Preferences), &Preferences::preferenceChanged, this, &QmlPreferences::onPreferenceChanged);
    connect($(Preferences), &Preferences::minimumChanged, this, &QmlPreferences::onMinimumChanged);
    connect($(Preferences), &Preferences::maximumChanged, this, &QmlPreferences::onMaximumChanged);
}

QmlPreferences::~QmlPreferences()
{
    flush();
}

QString QmlPreferences::section() const
{
    return _section;
}

void QmlPreferences::setSection(const QString& section)
{
    if(_section != section)
    {
        flush();
        _section = section;

        if(_initialised)
            load();
    }
}

void QmlPreferences::timerEvent(QTimerEvent* event)
{
    if(event->timerId() == _timerId)
    {
        killTimer(_timerId);
        _timerId = 0;
        save();
    }

    QObject::timerEvent(event);
}

enum class PropertyType
{
    Value,
    Minimum,
    Maximum
};

template<typename Fn> static void forEachProperty(const QObject* o, Fn fn)
{
    const QMetaObject *mo = o->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();
    for(int i = offset; i < count; i++)
    {
        QString propertyName(mo->property(i).name());
        PropertyType type;

        if(propertyName.endsWith("MinimumValue"))
            type = PropertyType::Minimum;
        else if(propertyName.endsWith("MaximumValue"))
            type = PropertyType::Maximum;
        else
            type = PropertyType::Value;

        fn(mo->property(i), type);
    }
}

void QmlPreferences::componentComplete()
{
    if(!_initialised)
    {
        load();

        // Connect all the property notify signals so we know when they change
        forEachProperty(this,
        [this](const QMetaProperty& property, PropertyType propertyType)
        {
            if(property.hasNotifySignal() && propertyType == PropertyType::Value)
            {
                auto index = metaObject()->indexOfSlot("onPropertyChanged()");
                auto slot = metaObject()->method(index);
                QObject::connect(this, property.notifySignal(), this, slot);
            }
        });

        _initialised = true;
    }
}

QString QmlPreferences::preferenceNameByPropertyName(const QString& propertyName)
{
    QString canoncicalPropertyName(propertyName);
    canoncicalPropertyName.replace(QRegularExpression("M(ax|in)imumValue"), "");

    return QString("%1/%2").arg(_section).arg(canoncicalPropertyName);
}

QMetaProperty QmlPreferences::propertyByName(const QString& propertyName)
{
    if(!propertyName.isEmpty())
    {
        auto index = metaObject()->indexOfProperty(propertyName.toLatin1());
        if(index >= 0)
            return metaObject()->property(index);
    }

    return {};
}

static QString propertyNameFrom(const QString& preferenceName)
{
    auto sepRegex = QRegularExpression("\\/");
    if(preferenceName.contains(sepRegex))
    {
        auto stringList = preferenceName.split(sepRegex);
        if(stringList.size() == 2)
            return stringList.at(1);
    }

    return preferenceName;
}

QMetaProperty QmlPreferences::valuePropertyFrom(const QString& preferenceName)
{
    return propertyByName(propertyNameFrom(preferenceName));
}

QMetaProperty QmlPreferences::minimumPropertyFrom(const QString& preferenceName)
{
    return propertyByName(QString("%1MinimumValue").arg(propertyNameFrom(preferenceName)));
}

QMetaProperty QmlPreferences::maximumPropertyFrom(const QString& preferenceName)
{
    return propertyByName(QString("%1MaximumValue").arg(propertyNameFrom(preferenceName)));
}

void QmlPreferences::setProperty(QMetaProperty property, const QVariant& value)
{
    if(!property.isValid())
        return;

    const QVariant previousValue = property.read(this);
    const QVariant currentValue = value;

    if((previousValue != currentValue && currentValue.canConvert(previousValue.type())) ||
       !previousValue.isValid())
    {
        property.write(this, currentValue);
    }
}

void QmlPreferences::load()
{
    forEachProperty(this,
    [this](const QMetaProperty& property, PropertyType propertyType)
    {
        auto preferenceName = preferenceNameByPropertyName(property.name());
        switch(propertyType)
        {
        case PropertyType::Value:
            setProperty(property, $(Preferences)->get(preferenceName));
            break;
        case PropertyType::Minimum:
            if($(Preferences)->exists(preferenceName))
                setProperty(property, $(Preferences)->minimum(preferenceName));
            break;
        case PropertyType::Maximum:
            if($(Preferences)->exists(preferenceName))
                setProperty(property, $(Preferences)->maximum(preferenceName));
            break;
        }
    });
}

void QmlPreferences::save()
{
    for(auto& pendingPreferenceChange : _pendingPreferenceChanges)
    {
        $(Preferences)->set(preferenceNameByPropertyName(pendingPreferenceChange.first),
                   pendingPreferenceChange.second);
    }

    _pendingPreferenceChanges.clear();
}

void QmlPreferences::flush()
{
    if(_initialised && !_pendingPreferenceChanges.empty())
        save();
}

void QmlPreferences::onPreferenceChanged(const QString& key, const QVariant& value)
{
    setProperty(valuePropertyFrom(key), value);
}

void QmlPreferences::onMinimumChanged(const QString& key, const QVariant& value)
{
    setProperty(minimumPropertyFrom(key), value);
}

void QmlPreferences::onMaximumChanged(const QString& key, const QVariant& value)
{
    setProperty(maximumPropertyFrom(key), value);
}

void QmlPreferences::onPropertyChanged()
{
    auto metaMethodName = metaObject()->method(senderSignalIndex()).name();
    QMetaProperty changedProperty;

    forEachProperty(this,
    [this, &metaMethodName, &changedProperty](const QMetaProperty& property, PropertyType)
    {
        if(property.notifySignal().name() == metaMethodName)
            changedProperty = property;
    });

    if(changedProperty.isValid())
    {
        _pendingPreferenceChanges[changedProperty.name()] = changedProperty.read(this);

        if(_timerId != 0)
            killTimer(_timerId);

        // Delay actually setting the preference until a property hasn't changed for some time
        // This prevents many rapid property changes causing redundant preference writes
        _timerId = startTimer(250);
    }
}
