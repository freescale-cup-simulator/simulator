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
            onAccepted: {
                sceneInstance.loadTrack(openTrackDialog.fileUrl)
                userSettings.setValue("LastOpenTrack", openTrackDialog.fileUrl)
            }
        }

        FileDialog {
            id: openAlgoDialog
            title: "Please choose algorithm file"
            nameFilters: [ "Lua algorithm files (*.lua)" ]
            selectExisting : true
            selectMultiple : false
            onAccepted: {
                simulationRunner.loadAlgorithmFile(openAlgoDialog.fileUrl)
                userSettings.setValue("LastOpenAlgorithm", openAlgoDialog.fileUrl)
            }
        }
        MenuItem {
            id: openTrack
            text: qsTr("Open track...")
            enabled: simulationRunner.simulationState === SimulationRuner.Stopped
            onTriggered: openTrackDialog.open()
            shortcut: "Ctrl+T"
        }
        MenuItem {
            id: openAlgorithm
            text: qsTr("Open algorithm...")
            enabled: simulationRunner.simulationState === SimulationRuner.Stopped
            onTriggered: openAlgoDialog.open();
            shortcut: "Ctrl+A"
        }

        MenuSeparator { }

        MenuItem {
            id: quickStart
            text: qsTr("Quick start")
            shortcut: "Ctrl+S"
            onTriggered: {

                var us = userSettings

                if (us.contains("LastOpenAlgorithm"))
                    simulationRunner.loadAlgorithmFile(us.value("LastOpenAlgorithm"))
                else
                    openAlgoDialog.open()

                if (us.contains("LastOpenTrack"))
                    sceneInstance.loadTrack(us.value("LastOpenTrack"))
                else
                    openTrackDialog.open()
            }
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
            text: qsTr("Begin simulation")
            onTriggered: simulationRunner.startThread()
            shortcut: "Alt+R"
            enabled: simulationRunner.simulationState == SimulationRuner.Stopped
        }
        MenuItem {
            id: pause
            text: simulationRunner.simulationState == SimulationRuner.Started
                  ? qsTr("Pause") : qsTr("Resume")
            onTriggered: {
                if (simulationRunner.simulationState == SimulationRuner.Paused)
                    simulationRunner.setState(SimulationRuner.Started)
                else
                    simulationRunner.setState(SimulationRuner.Paused)
            }
            shortcut: "Alt+P"
            enabled: simulationRunner.simulationState != SimulationRuner.Stopped
        }
        MenuItem {
            id: stop
            text: qsTr("Stop")
            onTriggered: simulationRunner.setState(SimulationRuner.Stopped)
            shortcut: "Alt+S"
            enabled: simulationRunner.simulationState != SimulationRuner.Stopped
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


