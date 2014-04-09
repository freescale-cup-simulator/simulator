import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.0
import Simulator 1.0

MenuBar {

    property alias showCameraView: showCameraView
    Menu {
        title: qsTr("File")
        FileDialog {
            id: openTrackDialog
            title: "Please choose track file"
            nameFilters: [ "Track files (*.xml)" ]
            selectExisting : true
            selectMultiple : false
            onAccepted: sceneInstance.loadTrack(openTrackDialog.fileUrl)
        }

        FileDialog {
            id: openAlgoDialog
            title: "Please choose algorithm file"
            nameFilters: [ "Lua algorithm files (*.lua)" ]
            selectExisting : true
            selectMultiple : false
            onAccepted: simulationRunner.loadAlgorithmFile(openAlgoDialog.fileUrl)
        }
        MenuItem {
            id: openTrack
            text: qsTr("Open track...")
            enabled: simulationRunner.simulationState === SimulationRuner.Stopped
            onTriggered: openTrackDialog.open()
        }
        MenuItem {

            id: openAlgorithm
            text: qsTr("Open algorithm...")
            enabled: simulationRunner.simulationState === SimulationRuner.Stopped
            onTriggered: openAlgoDialog.open();
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
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: qsTr("Simulation")

        MenuItem {
            id: run
            text: qsTr("Run simulation")
            shortcut: "Ctrl+R"
            onTriggered: simulationRunner.run()
        }
        MenuItem {
            id: pause
            text: qsTr("Pause/Resume")
            shortcut: "Pause"
            onTriggered: simulationRunner.pause()
        }
        MenuItem {
            id: stop
            text: qsTr("Stop")
            shortcut: "Ctrl+Break"
            onTriggered: simulationRunner.stop()
        }
    }
    Menu {
        title: qsTr("View")
        ExclusiveGroup { id: viewGroup }
        MenuItem {
            id: freeCam
            objectName:"free"
            text: qsTr("Free camera view")
            checkable: true
            checked: true
            exclusiveGroup: viewGroup
            onToggled:{
                /*if(checked)
                    guiController.viewSwitched(this)*/
            }

        }
        MenuItem {
            id: chasingCam
            objectName:"chasing"
            text: qsTr("Chasing camera view")
            checkable: true
            exclusiveGroup: viewGroup
            onToggled:{
                /*if(checked)
                    guiController.viewSwitched(this)*/
            }
        }
        MenuItem {
            id: fixedCam
            objectName:"fixed"
            text: qsTr("Car camera view")
            checkable: true
            exclusiveGroup: viewGroup
            onToggled:{
                /*if(checked)
                    guiController.viewSwitched(this)*/
            }
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


