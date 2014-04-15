import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

import Simulator 1.0
import GUI 1.0

TabView {
    id: settingsPanel
    Layout.minimumWidth: 255

    Tab {
        title: "Algorithm Settings"

        TableView {
            id: view
            anchors.margins: 10
            anchors.fill: parent
            model: propertyModel
            clip: true

            TableViewColumn { width: 80; title: "Property"; role: "name" }
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
                    onValueChanged: propertyModel.valueChanged(value, styleData.row)
                }
            }

            itemDelegate:  Text {
                anchors.centerIn: parent
                renderType: Text.NativeRendering
                text: styleData.value
            }
        }
    }

    Tab {
        title: "Physics Settings"
        id: physicsTab

        ScrollView {
            anchors.fill: parent
            ColumnLayout {
                width: physicsTab.width - 15

                GroupBox {
                    title: "FDS"
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting { name: "Direction 1"; value: physicsSimulation.slip1 }
                        PhysicsSetting { name: "Direction 2"; value: physicsSimulation.slip2 }
                    }
                }

                GroupBox {
                    title: "Friction"
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting { name: "Contact (μ)"; value: physicsSimulation.mu }
                        PhysicsSetting { name: "Rolling (ρ)"; value: physicsSimulation.rho }
                    }
                }

                GroupBox {
                    title: "Suspension"
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting {
                            name: "Coefficient "
                            value: physicsSimulation.suspension_k
                        }
                        PhysicsSetting {
                            name: "Damping "
                            value: physicsSimulation.suspension_damping
                        }
                    }
                }

                GroupBox {
                    title: "Tires"
                    Layout.fillWidth: true
                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting {
                            name: "Coefficient "
                            value: physicsSimulation.tire_k
                        }
                        PhysicsSetting {
                            name: "Damping "
                            value: physicsSimulation.tire_damping
                        }
                    }
                }
            }
        }
    }
}
