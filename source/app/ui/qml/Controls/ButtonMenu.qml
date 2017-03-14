import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQml 2.8

Item
{
    id: root
    width: button.width
    height: button.height
    implicitWidth: button.implicitWidth
    implicitHeight: button.implicitHeight

    property string defaultText: ""
    property string selectedValue: ""
    property color hoverColor
    property color textColor
    property bool propogatePresses: false

    property alias model: instantiator.model

    property bool menuDropped: false
    Menu
    {
        id: menu

        onAboutToShow: root.menuDropped = true;
        onAboutToHide: root.menuDropped = false;

        Instantiator
        {
            id: instantiator
            delegate: MenuItem
            {
                text: index >= 0 ? instantiator.model[index] : ""

                onTriggered: { root.selectedValue = text; }
            }

            onObjectAdded: menu.insertItem(index, object)
            onObjectRemoved: menu.removeItem(object)
        }
    }

    Rectangle
    {
        id: button

        width: label.width + 2 * 4/*padding*/
        height: label.height + 2 * 4/*padding*/
        implicitWidth: width
        implicitHeight: height
        radius: 2
        color: (mouseArea.containsMouse || root.menuDropped) ? root.hoverColor : "transparent"

        Label
        {
            id: label
            anchors
            {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }

            text: root.selectedValue != "" ? root.selectedValue : root.defaultText
            color: root.textColor
            font.bold: true
        }
    }

    MouseArea
    {
        id: mouseArea

        hoverEnabled: true
        anchors.fill: parent
        onClicked:
        {
            if(menu)
                menu.__popup(parent.mapToItem(null, 0, parent.height + 4/*padding*/, 0, 0), 0);
        }

        onPressed: { mouse.accepted = !propogatePresses; }
    }
}
