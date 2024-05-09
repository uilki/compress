import QtQuick
import QtQuick.Window
import QtQuick.Dialogs
import QtQuick.Layouts

import assets as AS

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Compress")

    AS.FileList {
        id: fileList
        anchors.fill: parent
        model: mainModel
        snapMode: ListView.NoSnap
        boundsBehavior: Flickable.StopAtBounds
    }

    MessageDialog {
        id: errDlg
    }

    Connections {
        target: mainModel
        function onErrorMessage(mes, idx) {
            errDlg.text = mes;
            errDlg.informativeText = mainModel.getFilename(idx);
            errDlg.open();
        }
    }
}
