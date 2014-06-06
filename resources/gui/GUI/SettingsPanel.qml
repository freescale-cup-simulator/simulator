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
                        PhysicsSetting {
                            name: "Direction 1";
                            value: vehicle.slip1
                            onChanged: vehicle.slip1 = value
                            decimals: 8
                        }
                        PhysicsSetting {
                            name: "Direction 2";
                            value: vehicle.slip2
                            onChanged: vehicle.slip2 = value
                            decimals: 8
                        }
                    }
                }

                GroupBox {
                    title: "Friction"
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting {
                            name: "Contact (μ)";
                            value: vehicle.mu
                            onChanged: vehicle.mu = value
                        }
                        PhysicsSetting { name: "Rolling (ρ)"; value: vehicle.rho }
                    }
                }

                GroupBox {
                    title: "Suspension"
                    Layout.fillWidth: true

                    ColumnLayout {
                        anchors.fill: parent
                        PhysicsSetting {
                            name: "Coefficient "
                            value: vehicle.suspension_k
                            onChanged: vehicle.suspension_k = value
                        }
                        PhysicsSetting {
                            name: "Damping "
                            value: vehicle.suspension_damping
                            onChanged: vehicle.suspension_damping = value
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
                            value: vehicle.tire_k
                            onChanged: vehicle.tire_k = value
                        }
                        PhysicsSetting {
                            name: "Damping "
                            value: vehicle.tire_damping
                            onChanged: vehicle.tire_damping = value
                        }
                    }
                }
            }
        }
    }
}
