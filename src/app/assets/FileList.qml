import QtQuick
import QtQuick.Layouts

ListView {
    id: tableView
    delegate: MouseArea {
        id: tableDelegate

        property int fileIndex: index

        width: tableView.width
        height: 20

        Rectangle {
            anchors.fill: parent
            color: tableView.currentIndex === index ? "grey" : "transparent"
        }

        RowLayout {
            spacing: 5
            anchors.fill: parent
            Text {
                Layout.maximumWidth: 0.6 * tableView.width
                Layout.fillWidth: true
                text: Name + "." + Suffix
            }

            Text {
                Layout.maximumWidth: 0.2 * tableView.width
                Layout.fillWidth: true
                text: Size  + " bytes"
            }

            Text {
                Layout.maximumWidth: 0.2 *tableView.width
                Layout.fillWidth: true
                text: Processing

            }
        }

        onClicked: tableView.currentIndex = index

        onDoubleClicked: {
            mainModel.processFile(fileIndex)
        }
    }

    headerPositioning: ListView.OverlayHeader

    header: RowLayout {
            spacing: 0
            z: 2
            implicitHeight: 32

            TextLabel {
                height: parent.height
                implicitWidth: 0.6 * tableView.width
                color: "lightgray"
                Layout.fillWidth: true

                label.text: "File"
            }

            TextLabel {
                height: 32
                implicitWidth: 0.2 * tableView.width
                color: "lightgray"
                Layout.fillWidth: true

                label.text: "Size"
            }

            TextLabel {
                height: 32
                implicitWidth: 0.2 * tableView.width
                color: "lightgray"
                Layout.fillWidth: true

                label.text: "Status"
            }
        }
}

