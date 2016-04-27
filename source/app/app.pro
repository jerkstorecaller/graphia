TEMPLATE = app

include(../common.pri)
include(../thirdparty/qtsingleapplication/qtsingleapplication.pri)

# Put the binary in the root of the build directory
DESTDIR = ../..

_PRODUCT_NAME=$$(PRODUCT_NAME)
isEmpty(_PRODUCT_NAME) {
    TARGET = "GraphTool"
}

_VERSION=$$(VERSION)
isEmpty(_VERSION) {
    _VERSION = "development"
}

_COPYRIGHT=$$(COPYRIGHT)
isEmpty(_COPYRIGHT) {
    _COPYRIGHT = "Copyright notice"
}

DEFINES += \
    "PRODUCT_NAME=\"\\\"$$TARGET\\\"\"" \
    "VERSION=\"\\\"$$_VERSION\\\"\"" \
    "COPYRIGHT=\"\\\"$$_COPYRIGHT\\\"\""

RC_ICONS = icon/Icon.ico # Windows
ICON = icon/Icon.icns # OSX

QT += qml quick opengl openglextensions

HEADERS += \
    application.h \
    commands/command.h \
    commands/commandmanager.h \
    commands/deleteselectednodescommand.h \
    graph/componentmanager.h \
    graph/elementiddistinctsetcollection.h \
    graph/elementid.h \
    graph/filter.h \
    graph/grapharray.h \
    graph/graphconsistencychecker.h \
    graph/graph.h \
    graph/graphmodel.h \
    graph/mutablegraph.h \
    graph/weightededgegraphmodel.h \
    layout/barneshuttree.h \
    layout/centreinglayout.h \
    layout/circlepackcomponentlayout.h \
    layout/collision.h \
    layout/componentlayout.h \
    layout/forcedirectedlayout.h \
    layout/layout.h \
    layout/layoutsettings.h \
    layout/nodepositions.h \
    layout/octree.h \
    layout/powerof2gridcomponentlayout.h \
    layout/randomlayout.h \
    layout/scalinglayout.h \
    layout/sequencelayout.h \
    loading/fileidentifier.h \
    loading/gmlfileparser.h \
    loading/gmlfiletype.h \
    loading/graphfileparser.h \
    loading/pairwisetxtfileparser.h \
    loading/pairwisetxtfiletype.h \
    maths/boundingbox.h \
    maths/boundingsphere.h \
    maths/circle.h \
    maths/conicalfrustum.h \
    maths/constants.h \
    maths/frustum.h \
    maths/interpolation.h \
    maths/line.h \
    maths/plane.h \
    maths/ray.h \
    rendering/camera.h \
    rendering/graphcomponentrenderer.h \
    rendering/graphcomponentscene.h \
    rendering/graphoverviewscene.h \
    rendering/graphrenderer.h \
    rendering/opengldebuglogger.h \
    rendering/openglfunctions.h \
    rendering/primitives/cylinder.h \
    rendering/primitives/sphere.h \
    rendering/scene.h \
    rendering/transition.h \
    transform/compoundtransform.h \
    transform/datafield.h \
    transform/edgecontractiontransform.h \
    transform/filtertransform.h \
    transform/graphtransform.h \
    transform/transformedgraph.h \
    ui/document.h \
    ui/graphcommoninteractor.h \
    ui/graphcomponentinteractor.h \
    ui/graphoverviewinteractor.h \
    ui/graphquickitem.h \
    ui/graphtransformconfiguration.h \
    ui/interactor.h \
    ui/selectionmanager.h \
    utils/circularbuffer.h \
    utils/debugpauser.h \
    utils/deferredexecutor.h \
    utils/enumreflection.h \
    utils/fixedsizestack.h \
    utils/movablepointer.h \
    utils/performancecounter.h \
    utils/preferences.h \
    utils/qmlcontainerwrapper.h \
    utils/qmlenum.h \
    utils/semaphore.h \
    utils/singleton.h \
    utils/threadpool.h \
    utils/utils.h

SOURCES += \
    application.cpp \
    commands/command.cpp \
    commands/commandmanager.cpp \
    commands/deleteselectednodescommand.cpp \
    graph/componentmanager.cpp \
    graph/graphconsistencychecker.cpp \
    graph/graph.cpp \
    graph/graphmodel.cpp \
    graph/mutablegraph.cpp \
    graph/weightededgegraphmodel.cpp \
    layout/barneshuttree.cpp \
    layout/centreinglayout.cpp \
    layout/circlepackcomponentlayout.cpp \
    layout/collision.cpp \
    layout/componentlayout.cpp \
    layout/forcedirectedlayout.cpp \
    layout/layout.cpp \
    layout/layoutsettings.cpp \
    layout/nodepositions.cpp \
    layout/powerof2gridcomponentlayout.cpp \
    layout/randomlayout.cpp \
    layout/scalinglayout.cpp \
    loading/fileidentifier.cpp \
    loading/gmlfileparser.cpp \
    loading/gmlfiletype.cpp \
    loading/graphfileparser.cpp \
    loading/pairwisetxtfileparser.cpp \
    loading/pairwisetxtfiletype.cpp \
    main.cpp \
    maths/boundingbox.cpp \
    maths/boundingsphere.cpp \
    maths/conicalfrustum.cpp \
    maths/frustum.cpp \
    maths/plane.cpp \
    maths/ray.cpp \
    rendering/camera.cpp \
    rendering/graphcomponentrenderer.cpp \
    rendering/graphcomponentscene.cpp \
    rendering/graphoverviewscene.cpp \
    rendering/graphrenderer.cpp \
    rendering/opengldebuglogger.cpp \
    rendering/openglfunctions.cpp \
    rendering/primitives/cylinder.cpp \
    rendering/primitives/sphere.cpp \
    rendering/transition.cpp \
    transform/compoundtransform.cpp \
    transform/datafield.cpp \
    transform/edgecontractiontransform.cpp \
    transform/filtertransform.cpp \
    transform/graphtransform.cpp \
    transform/transformedgraph.cpp \
    ui/document.cpp \
    ui/graphcommoninteractor.cpp \
    ui/graphcomponentinteractor.cpp \
    ui/graphoverviewinteractor.cpp \
    ui/graphquickitem.cpp \
    ui/graphtransformconfiguration.cpp \
    ui/selectionmanager.cpp \
    utils/debugpauser.cpp \
    utils/deferredexecutor.cpp \
    utils/performancecounter.cpp \
    utils/preferences.cpp \
    utils/semaphore.cpp \
    utils/threadpool.cpp \
    utils/utils.cpp

RESOURCES += \
    icon/mainicon.qrc \
    icon-themes/icons.qrc \
    rendering/shaders.qrc \
    ui/qml.qrc
