import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Dialogs.qml 1.0

Window {
    id: window
    visible: true
    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480
    opacity: 1
    title: qsTr("CPU-l")

    property int currentIndex: 0


    Rectangle {
        id: rectangle1
        width: 100
        color: "#323232"
        border.width: 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        ColumnLayout {
            id: column1
            x: 0
            y: 0

            anchors.fill: parent
            spacing: 2


            function setIndex( index ){
                window.currentIndex = index
            }

            TabButton {
                id: cpuButton
                text: qsTr("CPU Button")

                icon.width: implicitBackgroundWidth
                icon.height: implicitBackgroundHeight

                icon.name: "CPU-icon"
                icon.color: checked ? "#FFFFFF" : "#000000"
                icon.source: "./cpuIconxcf.png"

                hoverEnabled: true



                Layout.rightMargin: checked ? 0 : 2

                Layout.fillWidth: true //checked ? true : false
                Layout.fillHeight: true
                checked: true
                display: AbstractButton.IconOnly

                states: [
                    State{
                        name: "Hovering"
                        PropertyChanges{
                            target: cpuBackground
                            color: parent.checked ? "#707070" : "#666666"
                        }
                    },
                    State{
                        name: ""
                        PropertyChanges{
                            target: cpuBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: ""
                        ColorAnimation{ duration: 200 }
                    }

                ]

                state: hovered ? "Hovering" : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        column1.setIndex( 0 )
                        parent.checked = true
                        parent.clicked()
                    }
                }


                background: Rectangle{
                    id: cpuBackground
                    anchors.fill: parent
                    opacity: 1.0//cpuButton.checked ? 0.5 : 0.1
                    color: parent.checked ? "#707070" : "#444444"
                }

            }

            TabButton {
                id: ramButton
                text: qsTr("RAM Button")

                icon.name: "CPU-icon"
                icon.color: checked ? "#FFFFFF" : "#000000"
                icon.source: "./cpuIconxcf.png"
                icon.width: implicitBackgroundWidth
                icon.height: implicitBackgroundHeight
                hoverEnabled: true

                Layout.rightMargin: checked ? 0 : 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                display: AbstractButton.IconOnly

                states: [
                    State{
                        name: "Hovering"
                        PropertyChanges{
                            target: ramBackground
                            color: parent.checked ? "#707070" : "#666666"
                        }
                    },
                    State{
                        name: ""
                        PropertyChanges{
                            target: ramBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: ""
                        ColorAnimation{ duration: 200 }
                    }

                ]

                state: hovered ? "Hovering" : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        column1.setIndex( 1 )
                        parent.checked = true
                    }
                }


                background: Rectangle{
                    id: ramBackground
                    anchors.fill: parent
                    opacity: 1.0//ramButton.checked ? 0.5 : 0.10
                    color: parent.checked ? "#707070" : "#444444"
                }
            }

            TabButton {
                id: moboButton

                text: qsTr("Mobo Button")
                icon.name: "CPU-icon"
                icon.color: checked ? "#FFFFFF" : "#000000"
                icon.source: "./cpuIconxcf.png"
                icon.width: implicitBackgroundWidth
                icon.height: implicitBackgroundHeight
                hoverEnabled: true



                Layout.rightMargin: checked ? 0 : 2
                Layout.fillWidth: true
                Layout.fillHeight: true

                display: AbstractButton.IconOnly

                states: [
                    State{
                        name: "Hovering"
                        PropertyChanges{
                            target: moboBackground
                            color: parent.checked ? "#707070" : "#666666"
                        }
                    },
                    State{
                        name: ""
                        PropertyChanges{
                            target: moboBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: ""
                        ColorAnimation{ duration: 200 }
                    }

                ]

                state: hovered ? "Hovering" : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        column1.setIndex( 2 )
                        parent.checked = true
                    }
                }

                background: Rectangle{
                    id: moboBackground
                    anchors.fill: parent
                    opacity: 1.0//moboButton.checked ? 0.5 : 0.10
                    color: parent.checked ? "#707070" : "#444444"
                }
            }

            TabButton {
                id: gpuButton

                text: qsTr("GPU Button")


                icon.name: "CPU-icon"
                icon.color: checked ? "#FFFFFF" : "#000000"
                icon.source: "./cpuIconxcf.png"
                icon.width: implicitBackgroundWidth
                icon.height: implicitBackgroundHeight
                hoverEnabled: true

                states: [
                    State{
                        name: "Hovering"
                        PropertyChanges{
                            target: gpuBackground
                            color: parent.checked ? "#707070" : "#666666"
                        }
                    },
                    State{
                        name: ""
                        PropertyChanges{
                            target: gpuBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: ""
                        ColorAnimation{ duration: 200 }
                    }

                ]

                state: hovered ? "Hovering" : ""

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        //            _petersCoolClass.cppSlot( "yay" )
                        column1.setIndex( 3 )
                        parent.checked = true
                    }
                }

                Layout.rightMargin: checked ? 0 : 2
                Layout.fillWidth: true
                Layout.fillHeight: true
                display: AbstractButton.IconOnly
                background: Rectangle{
                    id: gpuBackground
                    anchors.fill: parent
                    opacity: 1.0//gpuButton.checked ? 0.5 : 0.10
                    color: gpuButton.checked ? "#707070" : "#444444"
                }
            }
        }
    }



    StackLayout {
        id: stackView
        objectName: "mainStackView"
        x: 100
        y: 0
        width: 540
        height: 480

        currentIndex: window.currentIndex

        Rectangle{
            color: "#707070"
            visible: true

            ColumnLayout {
                id: columnLayout
                anchors.fill: parent

                MenuBar: MenuBar{
                    id: test

                }

                Row {
                    id: row
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Row {
                    id: row1
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                Row {
                    id: row2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
            }
        }
        Rectangle{
            color: "#707070"
            visible: false
        }
        Rectangle{
            color: "#707070"
        }
        Rectangle{
            color: "#707070"
        }
    }
}
