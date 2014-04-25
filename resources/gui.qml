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
        Label { text: "Vehicle velocity: " + vehicle.vehicleVelocity.toFixed(3) + " m/s"}
    }

    SplitView{
        anchors.fill: parent
        orientation: Qt.Horizontal
        resizing: true

        GridLayout {
            id: mainWindowColumn
            Layout.fillWidth: true

            CameraGrabber {
                id: freeCamera
                objectName: "freeCamera"
                Layout.fillWidth: true
                Layout.fillHeight: true

                FocusScope {
                    x: keyboardArea.x; y: keyboardArea.y
                    width: keyboardArea.width; height: keyboardArea.height
                    Item {
                            id: keyboardArea
                            anchors.fill: parent
                            focus: true
                            Keys.onPressed: {
                                freeCamera.camera.onKeyPressed(event.key);
                                event.accepted=true;
                            }
                            Keys.onReleased: {
                                freeCamera.camera.onKeyReleased(event.key);
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
                            if (prevX > -1 && prevY > -1)
                                freeCamera.camera.onMouseMove(mouse.x - prevX,
                                                              mouse.y - prevY);
                            prevX = mouse.x
                            prevY = mouse.y
                        }
                    }
                    onReleased: { prevX = -1; prevY = -1 }
                }

                Rectangle {
                    id: camViewContainer

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
                        drag.maximumX: freeCamera.width - width
                        drag.minimumY: 0
                        drag.maximumY: freeCamera.height - height
                    }

                    CameraGrabber
                    {
                        id: linescanCamera
                        objectName: "linescanCamera"
                        anchors.left: parent.left
                        anchors.leftMargin: 2
                        anchors.top: parent.top
                        anchors.topMargin: 2
                        width: 128
                        height: 128
                    }
                }
            }

            SettingsPanel {}
        }
    }
}
