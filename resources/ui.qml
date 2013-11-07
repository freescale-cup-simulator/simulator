/*!
 * \copyright (c) Nokia Corporation and/or its subsidiary(-ies) (qt-info@nokia.com) and/or contributors
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 *
 * \license{This source file is part of QmlOgre abd subject to the BSD license that is bundled
 * with this source code in the file LICENSE.}
 */

import QtQuick 2.0
import CameraGrabber 1.0

Rectangle {
    id: ogre
    objectName: "root"
    width: 1024
    height: 768
    color: "black"

    CameraGrabber {
        id: grabber
        width: ogre.width
        height: ogre.height
        anchors.leftMargin: -5
        anchors.topMargin: 6
        camera: Camera
        ogreEngine: OgreEngine
        Behavior on opacity { NumberAnimation { } }
        Behavior on width { NumberAnimation { } }
        Behavior on height { NumberAnimation { } }

        Item {
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
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            property int prevX: -1
            property int prevY: -1

            onPositionChanged: {
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

    }



    Rectangle {
        id: rectangle1
        objectName: "camViewContainer"
        width: 136
        height: 136

        anchors.left: rectangle2.left
        anchors.leftMargin: -5
        anchors.top: rectangle2.bottom
        anchors.topMargin: 6
        border.width: 4
        border.color: "#1a1a1a"
        clip: false
        CameraGrabber
        {
            objectName: "camView"
            anchors.left: rectangle1.left
            anchors.leftMargin: 4
            anchors.top: rectangle1.top
            anchors.topMargin: 4
            width: 128
            height: 128
            ogreEngine: OgreEngine
        }

        Behavior on opacity { PropertyAnimation { } }
    }

    Rectangle {
        id: rectangle2
        x: 31
        y: 269
        width: 25
        height: 25
        radius: 5
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#c83e3e3e"
            }

            GradientStop {
                position: 1
                color: "#c8919191"
            }
        }

        border.width: 2
        border.color: "#1a1a1a"


        MouseArea {
            anchors.fill: parent
            drag.target: rectangle2
            drag.axis: "XandYAxis"
            drag.minimumX: 0
            drag.minimumY: 0
            drag.maximumX: ogre.width - rectangle2.width
            drag.maximumY: ogre.height - rectangle2.height
        }
    }

    Rectangle {
        id: rectangle10
        width: 25
        radius: 5
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#c83e3e3e"
            }

            GradientStop {
                position: 1
                color: "#c8919191"
            }
        }
        anchors.left: rectangle2.right
        anchors.leftMargin: 6
        anchors.top: rectangle2.top
        anchors.bottom: rectangle2.bottom
        border.color: "#1a1a1a"

        MouseArea {
            anchors.fill: parent
            drag.minimumY: 0
            drag.axis: "XandYAxis"
            drag.minimumX: 0
            drag.target: rectangle10
            drag.maximumY: ogre.height - rectangle10.height
            drag.maximumX: ogre.width - rectangle10.width

            onClicked: ogre.state = ogre.state == '' ? 'State1' : ''
        }

        Rectangle {
            id: rectangle11
            x: 0
            y: -2
            radius: 12
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#5a5a5a"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            rotation: -35
            anchors.rightMargin: 6
            anchors.bottomMargin: 6
            anchors.leftMargin: 6
            anchors.topMargin: 6
            anchors.fill: parent
        }
        border.width: 2
    }

    Rectangle {
        id: rectangle12
        width: 25
        height: 25
        radius: 5
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#c83e3e3e"
            }

            GradientStop {
                position: 1
                color: "#c8919191"
            }
        }
        anchors.top: rectangle2.top
        anchors.right: rectangle1.right
        anchors.rightMargin: 5
        border.color: "#1a1a1a"

        MouseArea {
            property bool fullscreen: false
            anchors.fill: parent
            onClicked: {
                if (fullscreen)
                    Window.showNormal();
                else
                    Window.showFullScreen();
                fullscreen = !fullscreen;
            }
        }

        Rectangle {
            id: rectangle13
            color: "#28ffffff"
            radius: 2
            border.width: 2
            border.color: "#000000"
            anchors.rightMargin: 7
            anchors.leftMargin: 7
            anchors.topMargin: 7
            anchors.bottomMargin: 7
            anchors.fill: parent

            Rectangle {
                id: rectangle14
                height: 3
                color: "#000000"
                anchors.right: parent.right
                anchors.left: parent.left
                anchors.top: parent.top
            }
        }
        border.width: 2
    }
    states: [
        State {
            name: "State1"

            PropertyChanges {
                target: rectangle1
                opacity: 0
            }
        }
    ]
}
