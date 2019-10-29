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

    RowLayout{
        id: mainWindowRowLayout
        anchors.fill: parent
        spacing: 0



    Rectangle {
        id: rectangle1
        width: 100
        color: "#323232"
        border.width: 0
        Layout.fillHeight: true
       // anchors.top: parent.top
       // anchors.bottom: parent.bottom
       // Layout.alignment: left

        ColumnLayout {
            id: column1
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

                Layout.fillWidth: true
                Layout.fillHeight: true
                checked: true
                display: AbstractButton.IconOnly

                states: [
                    State{
                        name: "Hovering"
                        PropertyChanges{
                            target: cpuBackground
                            color: parent.checked ? "#303030" : "#666666"
                        }
                    },

                    State{
                        name: "Notvering"
                        PropertyChanges{
                            target: cpuBackground
                            color: parent.checked ? "#303030" : "#444444"
                        }
                    }
                ]

                transitions: [
                    Transition{
                        from: "Notvering";
                        to: "Hovering"
                        ColorAnimation{  duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: "Notvering"
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
                    color: parent.checked ? "#303030" : "#444444"
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
                        name: "Notvering"
                        PropertyChanges{
                            target: ramBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "Notvering";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: "Notvering"
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
                        name: "Notvering"
                        PropertyChanges{
                            target: moboBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "Notvering";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: "Notvering"
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
                        name: "Notvering"
                        PropertyChanges{
                            target: gpuBackground
                            color: parent.checked ? "#707070" : "#444444"
                        }
                    }

                ]
                transitions: [
                    Transition{
                        from: "Notvering";
                        to: "Hovering"
                        ColorAnimation{ duration: 200 }
                    },
                    Transition{
                        from: "Hovering";
                        to: "Notvering"
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
        //x: 100
        //y: 0

        Layout.fillHeight: true

        currentIndex: window.currentIndex

        TitleBox{
            titleString: "CPU Information"

            GridLayout {
                id: grid
                columns: 3
                rows: 4
                height: parent.height
                width: parent.width
                columnSpacing: 4
                rowSpacing: 4


                TitleBox{
                    titleString: "Vendor"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }
                TitleBox{
                    titleString: "uArch"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }
                TitleBox{
                    titleString: "ID Codes"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }

                TitleBox{
                    titleString: "Cores"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }
                TitleBox{
                    titleString: "Threads"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }
                TitleBox{
                    titleString: "Clock"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }

                TitleBox{
                    titleString: "L1 Cache"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"

                }
                TitleBox{
                    titleString: "L2 Cache"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"
                }
                TitleBox{
                    titleString: "L3 Cache"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    color: "#505050"
                }

                TitleBox{
                    titleString: "CPU Extensions"
                    Layout.fillWidth: true
                    Layout.columnSpan: 3
                    Layout.fillHeight: true
                    color: "#505050"

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
}
