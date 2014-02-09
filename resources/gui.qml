import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0

import OgreTypes 1.0
import GUI 1.0

ApplicationWindow {
    objectName: "rootWindow"
    title: qsTr("Freescale cup simulator")
    minimumWidth: 1024
    minimumHeight: 768
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
        globalRenderer.onContextObjectsSet.connect(loadCamera)
    }

    function loadCamera() {
        cameraLoader.active = true
    }

    GridLayout {
        id: mainWindowColumn

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.right: settingsColumn.left
        anchors.rightMargin: 10

        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10

        Component {
            id: cameraComponent
            CameraGrabber {
                id: grabber
                objectName: "freeCamView"
                anchors.fill: parent

                Component.onCompleted: {
                    camera = userCamera
                    ogreEngine = ogreEngineInstance
                    keyboardArea.forceActiveFocus()
                    //camView.ogreEngine = ogreEngineInstance
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

                    width: parent.width/4
                    height: width

                    visible: menuStrip.showCameraView.checked

                    border.width: 4
                    border.color: "#1a1a1a"
                    clip: false

                    MouseArea {
                        anchors.fill: parent
                        drag.target: parent
                        drag.minimumX: 0
                        drag.maximumX: cameraLoader.width - width
                        drag.minimumY: 0
                        drag.maximumY: cameraLoader.height - height
                    }

                    CameraGrabber
                    {
                        id: camView
                        objectName: "camView"
                        anchors.left: camViewContainer.left
                        anchors.leftMargin: 4
                        anchors.top: camViewContainer.top
                        anchors.topMargin: 4
                        width: 128
                        height: 128
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

    GroupBox {
        id: settingsColumn
        width: 160

        anchors.topMargin: 10
        anchors.bottomMargin: 8
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.top: parent.top

        Label {
            id: label1

            text: "Settings"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        GroupBox {
            id: row1

            anchors.top: label1.bottom
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.left: parent.left

            Label {
                id: speedLabel
                text: "Speed"
            }

            SpinBox {
                id: spinbox1
                x: 40
                width: 90
                minimumValue: 0
                decimals: 1
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