import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0

import OgreTypes 1.0
import GUI 1.0

ApplicationWindow {
    objectName: "rootWindow"
    title: qsTr("Freescale cup simulator")
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    menuBar: MenuStrip { id: menuStrip}

    statusBar: StatusBar {
        Column {
            anchors.fill: parent
            RowLayout{
                id: footerRow

                Label {
                    id: stateLabel
                    text: "State: "
                }

                Label {
                    id: state
                    text: "car state"
                }
            }


            RowLayout {
                id: progessBarRow

                Label {
                    id: progressLabel
                    text: "Progress: "
                }

                ProgressBar {
                    id: run_progress_bar
                    Layout.fillWidth: true
                    anchors.right: parent.right
                    anchors.left: progressLabel.right
                }
            }
        }
    }


    Component.onCompleted: {
        globalRenderer.onContextObjectsSet.connect(loadCamera);
    }

    function loadCamera() {
        cameraLoader.active = true
    }

    SplitView{
        anchors.fill: parent
        orientation: Qt.Horizontal
        resizing: true

        GridLayout {
            id: mainWindowColumn
            Layout.fillWidth: true

            Component {
                id: cameraComponent
                CameraGrabber {
                    id: grabber
                    objectName: "freeCamView"
                    anchors.fill: parent
                    Component.onCompleted: {
                        camera = userCamera;
                        ogreEngine = ogreEngineInstance;
                        keyboardArea.forceActiveFocus();
                        guiController.onCameraSimulatorCreated.connect(loadCamSimulatorViewport);
                        guiController.onCameraSimulatorDestroyed.connect(hideCamSimulatorViewport);
                        //camView.ogreEngine = ogreEngineInstance
                    }
                    function loadCamSimulatorViewport(cam) {
                        camSimulatorViewportLoader.active=true;
                    }

                    function hideCamSimulatorViewport()
                    {
                        camSimulatorViewportLoader.active=false;
                    }

                    FocusScope {
                        x: keyboardArea.x; y: keyboardArea.y
                        width: keyboardArea.width; height: keyboardArea.height
                        Item {
                            id: keyboardArea
                            anchors.fill: parent
                            focus: true
                            Keys.onPressed: {
                                grabber.camera.onKeyPressed(event.key);
                                event.accepted=true;
                            }
                            Keys.onReleased: {
                                grabber.camera.onKeyReleased(event.key);
                                event.accepted=true;
                            }
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton

                        property int prevX: -1
                        property int prevY: -1

                        onPositionChanged: {
                            keyboardArea.forceActiveFocus()
                            if (pressedButtons & Qt.LeftButton) {
                                //mouse.x mouse.y grabber.camera
                                if (prevX > -1 && prevY > -1)
                                    grabber.camera.onMouseMove(mouse.x-prevX,mouse.y-prevY);
                                //ogreitem.camera.pitch -= (mouse.y - prevY) / 2
                                prevX = mouse.x
                                prevY = mouse.y
                            }
                        }
                        onReleased: { prevX = -1; prevY = -1 }
                    }

                    Rectangle {
                        id: camViewContainer
                        objectName: "camViewContainer"

                        x: parent.width - width
                        y: parent.height - height

                        width: 132
                        height: width

                        visible: menuStrip.showCameraView.checked

                        border.width: 4
                        border.color: "#cccccc"
                        clip: false

                        MouseArea {
                            anchors.fill: parent
                            drag.target: parent
                            drag.minimumX: 0
                            drag.maximumX: cameraLoader.width - width
                            drag.minimumY: 0
                            drag.maximumY: cameraLoader.height - height
                        }

                        Component
                        {
                            id: camSimulatorViewportComponent
                            CameraGrabber
                            {
                                id: camSimulatorViewport
                                objectName: "camSimulatorViewport"
                                anchors.left: parent.left
                                anchors.leftMargin: 2
                                anchors.top: parent.top
                                anchors.topMargin: 2
                                width: 128
                                height: 128
                                Component.onCompleted: {
                                    camera=carCamera;
                                    ogreEngine=ogreEngineInstance;
                                }
                            }

                        }
                        Loader {
                            id: camSimulatorViewportLoader
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            sourceComponent: camSimulatorViewportComponent
                            active: false
                        }
                    }
                }
            }

            Loader {
                id: cameraLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                sourceComponent: cameraComponent
                active: false

            }
        }


        Rectangle {
            id: settingsColumn

            Layout.minimumWidth: 255

            TableView {
                id: view

                anchors.margins: 10
                anchors.fill: parent
                model: settingsModel
                clip: true

                TableViewColumn {
                    width: 80
                    title: "Property"
                    role: "name"

                }
                TableViewColumn {
                    width: 155
                    title: "Value"
                    role: "value"
                    delegate: SpinBox {
                        value: styleData.value.x
                        decimals: 8
                        activeFocusOnPress: true
                        maximumValue: Infinity
                        minimumValue: -Infinity
                        stepSize: styleData.value.y
                        onValueChanged: settingsModel.valueChanged(value, styleData.row)
                    }
                }
                itemDelegate: Item {
                    Text {
                        anchors.centerIn: parent
                        renderType: Text.NativeRendering
                        text: styleData.value
                    }
                }


                states: [
                    State {
                        name: "closed"
                        PropertyChanges {
                            target: settingsColumn
                            width: 0
                            opacity: 0
                        }
                    }
                ]
            }

        }
    }
}
