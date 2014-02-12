import QtQuick 2.1
import QtQuick.Controls 1.0

MenuBar {
    property alias showCameraView: showCameraView

    Menu {
        title: qsTr("File")

        MenuItem {
            id: openTrack
            text: qsTr("Open track...")
        }
        MenuItem {
            id: openAlgorithm
            text: qsTr("Open algorithm...")
        }
        MenuSeparator { }
        MenuItem {
            id: recentTracks
            text: qsTr("Recent tracks")
        }
        MenuItem {
            id: recentAlgorithm
            text: qsTr("Recent algorithms")
        }
        MenuSeparator { }
        MenuItem {
            id: quit
            text: "Quit"
            shortcut: "Ctrl+Q"
        }
    }

    Menu {
        title: qsTr("Simulation")

        MenuItem {
            id: run
            text: qsTr("Run simulation")
            shortcut: "Ctrl+R"
        }
        MenuItem {
            id: pause
            text: qsTr("Pause")
            shortcut: "Pause"
        }
        MenuItem {
            id: stop
            text: qsTr("Stop")
        }
    }
    Menu {
        title: qsTr("View")
        ExclusiveGroup { id: viewGroup }
        MenuItem {
            id: freeCam
            text: qsTr("Free camera view")
            checkable: true
            checked:true
            exclusiveGroup: viewGroup
        }
        MenuItem {
            id: chasingCam
            text: qsTr("Chasing camera view")
            checkable: true
            exclusiveGroup: viewGroup
        }
        MenuItem {
            id: fixedCam
            text: qsTr("Car camera view")
            checkable: true
            exclusiveGroup: viewGroup
        }
    }

    Menu {
        title: qsTr("Tools")
        MenuItem {
            id: trackBuilder
            text: qsTr("Track builder")
        }
        MenuItem {
            id: graphViewer
            text: qsTr("Graphics viewer")
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


