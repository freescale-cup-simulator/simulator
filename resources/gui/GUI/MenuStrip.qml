import QtQuick 2.1
import QtQuick.Controls 1.0

MenuBar {
    property alias showCameraView: showCameraView

    Menu {
        title: qsTr("File")

        MenuItem {
            id: openMap
            text: qsTr("Open map")
        }
        MenuItem {
            id: openAlgorithm
            text: qsTr("Open Algorithm")
        }
        MenuItem {
            id: lastUsed
            text: qsTr("Last Used")
        }
        MenuItem {
            id: exit
            text: "Exit"
            onTriggered: Qt.quit();
        }
    }

    Menu {
        title: qsTr("Run")

        MenuItem {
            id: run
            text: qsTr("Run simulation")
        }
        MenuItem {
            id: pause
            text: qsTr("Pause")
        }
        MenuItem {
            id: stop
            text: qsTr("Stop")
        }
    }

    Menu {
        title: qsTr("Window")

        MenuItem {
            id: showCameraView
            text: qsTr("Show Camera View")
            checkable: true
            checked: true

//            onToggled: {
//                if (settingsColumn.state == ''){
//                    settingsColumn.state = 'closed';
//                    this.checked = false;
//                }
//                else {
//                    settingsColumn.state = '';
//                    this.checked = true;
//                }

//            }
        }
        MenuItem {
            id: showSettings
            text: qsTr("Show Settings")
            checkable: true
            checked: true
            onToggled: {
                if (settingsColumn.state == ''){
                    settingsColumn.state = 'closed';
                    this.checked = false;
                }
                else {
                    settingsColumn.state = '';
                    this.checked = true;
                }

            }
        }

    }

    Menu {
        title: qsTr("Help")

        MenuItem {
            id: reference
            text: qsTr("Reference")

        }
        MenuItem {
            id: about
            text: qsTr("About")
        }
        MenuItem {
            id: develpers
            text: qsTr("Developers")
        }
    }
}


