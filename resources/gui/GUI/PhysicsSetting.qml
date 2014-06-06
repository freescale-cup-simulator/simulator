import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

ColumnLayout {
    property alias name: settingName.text
    property alias value: settingSpinbox.value
    property alias decimals: settingSpinbox.decimals

    signal changed(real v)

    Layout.fillWidth: true

    Label { id: settingName }
    SpinBox {
        Layout.fillWidth: true
        id: settingSpinbox
        decimals: 4
        maximumValue: Infinity
        minimumValue: -Infinity
        onValueChanged: changed(value)
    }
}
