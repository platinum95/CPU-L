import QtQuick 2.0
import QtQuick.Layouts 1.3

Rectangle {
    id: infoBox
    property string titleString: "Placeholder Title"

    default property alias content: boxCanvas.children
    property int canvasBorderWidth: 0;

    color: "#303030"

    ColumnLayout {
        id: column
        width: parent.width
        height: parent.height
        spacing: 0

        Rectangle {
                Layout.alignment: Qt.AlignCenter
                Layout.leftMargin: 1
                Layout.topMargin: 3
                Layout.rightMargin: 1
                Layout.bottomMargin: 3

                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.1
                Layout.maximumHeight:  480 * 0.1
                id: titleRow
                color: infoBox.color

                Text{
                    id: titleText
                    color: "#ffffff"
                    text: infoBox.titleString
                    anchors.fill: parent

                    font.pointSize: 16
                    fontSizeMode: Text.VerticalFit
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter


                }
        }

        Rectangle {
            id: dividerRow
            height: 13
            Layout.leftMargin: 1
            Layout.topMargin: 1
            Layout.rightMargin: 1
            Layout.bottomMargin: 1

            Layout.alignment: Qt.AlignRight
            color: infoBox.color
            Layout.fillWidth: true
            Divider{
                width: infoBox.width * 0.8
                anchors.horizontalCenter: parent.horizontalCenter
                height: 11
            }
        }


        Rectangle {
            id: contentRow
            Layout.alignment: Qt.AlignBottom
            Layout.fillHeight: true
            color: infoBox.color
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            Layout.topMargin: parent.height * 0.02
            Layout.bottomMargin: parent.height * 0.05
            Layout.rightMargin: parent.width * 0.05
            Layout.leftMargin: parent.width * 0.05
            Rectangle{
                id: boxCanvas
                anchors.fill: parent
                color: infoBox.color
                border.width: canvasBorderWidth


            }
        }
    }

}
