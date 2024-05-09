import QtQuick

Rectangle {
    id: myRect
    property alias label: textLabel

    border.width: 1
    border.color: "black"

    Text {
        id: textLabel
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: "File"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
