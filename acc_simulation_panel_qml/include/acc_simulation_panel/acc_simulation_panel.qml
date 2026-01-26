import QtQuick 2.15
import QtQuick.Controls 2.15

Pane{  
    id: root

    Column {
        anchors.left: parent
        spacing: 10        

        Text {
            text: "Vehicles: " + panel.vehicleCount
        }

        Row {
            spacing: 10
            Button {
                text: "Add Vehicle"
                onClicked: panel.onAddVehicleClicked()
            }
            Button {
                text: "Remove Vehicle"
                onClicked: panel.onRemoveVehicleClicked()
            }
        }
        

        Label {
            text: "Distance Setting: " + distanceSlider.value.toFixed(1) + " meters"
        }

        Slider {
            id: distanceSlider
            from: 10.0
            value: 40.0
            to: 50.0
            onValueChanged: panel.onDistanceChanged(value)
        }
    }
}