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
        Column {
            anchors.fill: parent

            PhysicsSetting { name: "slip1"; value: physicsSimulation.slip1 }
            PhysicsSetting { name: "slip2"; value: physicsSimulation.slip2 }
            PhysicsSetting { name: "mu"; value: physicsSimulation.mu }
            PhysicsSetting { name: "rho"; value: physicsSimulation.rho }
            PhysicsSetting {
                name: "suspension_k"
                value: physicsSimulation.suspension_k
            }
            PhysicsSetting {
                name: "suspension_damping"
                value: physicsSimulation.suspension_damping
            }
            PhysicsSetting {
                name: "tire_k"
                value: physicsSimulation.tire_k
            }
            PhysicsSetting {
                name: "tire_damping"
                value: physicsSimulation.tire_damping
            }
        }
    }
}
