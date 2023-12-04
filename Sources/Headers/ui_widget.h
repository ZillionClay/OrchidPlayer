/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>
#include "clickableqslider.hpp"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *PlayButton;
    QPushButton *PauseButton;
    QPushButton *StopButton;
    QPushButton *OpenButton;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    ClickableQSlider *horizontalSlider;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *comboBox;
    QLabel *label_5;
    QCheckBox *checkBox;
    QPushButton *RefreshButton;
    QSlider *VolumeSlider;
    QLabel *label_6;
    QPushButton *BrowseButton;
    QComboBox *SampleRateBox;
    QComboBox *BitFormatBox;
    QLabel *label_7;
    QLabel *label_8;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(845, 582);
        PlayButton = new QPushButton(Widget);
        PlayButton->setObjectName("PlayButton");
        PlayButton->setGeometry(QRect(150, 320, 141, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(12);
        PlayButton->setFont(font);
        PauseButton = new QPushButton(Widget);
        PauseButton->setObjectName("PauseButton");
        PauseButton->setGeometry(QRect(350, 320, 131, 51));
        PauseButton->setFont(font);
        StopButton = new QPushButton(Widget);
        StopButton->setObjectName("StopButton");
        StopButton->setGeometry(QRect(530, 320, 131, 51));
        StopButton->setFont(font);
        OpenButton = new QPushButton(Widget);
        OpenButton->setObjectName("OpenButton");
        OpenButton->setGeometry(QRect(640, 70, 81, 31));
        OpenButton->setFont(font);
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(90, 130, 641, 31));
        label->setFont(font);
        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(130, 70, 491, 31));
        lineEdit->setFont(font);
        label_2 = new QLabel(Widget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(90, 80, 41, 16));
        label_2->setFont(font);
        horizontalSlider = new ClickableQSlider(Widget);
        horizontalSlider->setObjectName("horizontalSlider");
        horizontalSlider->setGeometry(QRect(160, 240, 501, 18));
        horizontalSlider->setFont(font);
        horizontalSlider->setMaximum(100);
        horizontalSlider->setOrientation(Qt::Horizontal);
        label_3 = new QLabel(Widget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(90, 170, 301, 16));
        label_3->setFont(font);
        label_4 = new QLabel(Widget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(420, 170, 301, 16));
        label_4->setFont(font);
        comboBox = new QComboBox(Widget);
        comboBox->setObjectName("comboBox");
        comboBox->setGeometry(QRect(210, 450, 331, 21));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Consolas")});
        font1.setPointSize(9);
        comboBox->setFont(font1);
        label_5 = new QLabel(Widget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(100, 450, 101, 16));
        label_5->setFont(font1);
        checkBox = new QCheckBox(Widget);
        checkBox->setObjectName("checkBox");
        checkBox->setGeometry(QRect(100, 400, 91, 31));
        checkBox->setFont(font1);
        RefreshButton = new QPushButton(Widget);
        RefreshButton->setObjectName("RefreshButton");
        RefreshButton->setGeometry(QRect(560, 450, 75, 24));
        VolumeSlider = new QSlider(Widget);
        VolumeSlider->setObjectName("VolumeSlider");
        VolumeSlider->setGeometry(QRect(760, 370, 20, 121));
        VolumeSlider->setMaximum(100);
        VolumeSlider->setValue(100);
        VolumeSlider->setOrientation(Qt::Vertical);
        label_6 = new QLabel(Widget);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(750, 500, 54, 16));
        BrowseButton = new QPushButton(Widget);
        BrowseButton->setObjectName("BrowseButton");
        BrowseButton->setGeometry(QRect(730, 70, 81, 31));
        BrowseButton->setFont(font);
        SampleRateBox = new QComboBox(Widget);
        SampleRateBox->setObjectName("SampleRateBox");
        SampleRateBox->setGeometry(QRect(240, 500, 101, 22));
        QFont font2;
        font2.setFamilies({QString::fromUtf8("Consolas")});
        SampleRateBox->setFont(font2);
        BitFormatBox = new QComboBox(Widget);
        BitFormatBox->setObjectName("BitFormatBox");
        BitFormatBox->setGeometry(QRect(490, 500, 161, 22));
        BitFormatBox->setFont(font2);
        label_7 = new QLabel(Widget);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(100, 500, 141, 16));
        label_7->setFont(font2);
        label_8 = new QLabel(Widget);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(400, 500, 81, 16));
        label_8->setFont(font2);

        retranslateUi(Widget);
        QObject::connect(PlayButton, SIGNAL(clicked()), Widget, SLOT(Play()));
        QObject::connect(StopButton, SIGNAL(clicked()), Widget, SLOT(Stop()));
        QObject::connect(PauseButton, SIGNAL(clicked()), Widget, SLOT(Pause()));
        QObject::connect(OpenButton, SIGNAL(clicked()), Widget, SLOT(Open()));
        QObject::connect(RefreshButton, SIGNAL(clicked()), Widget, SLOT(UpdateDeviceList()));

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        PlayButton->setText(QCoreApplication::translate("Widget", "Play", nullptr));
        PauseButton->setText(QCoreApplication::translate("Widget", "Pause", nullptr));
        StopButton->setText(QCoreApplication::translate("Widget", "Stop", nullptr));
        OpenButton->setText(QCoreApplication::translate("Widget", "Open", nullptr));
        label->setText(QCoreApplication::translate("Widget", "Playing: ", nullptr));
        lineEdit->setText(QString());
        label_2->setText(QCoreApplication::translate("Widget", "URL:", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "Duration:", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "Current:", nullptr));
        label_5->setText(QCoreApplication::translate("Widget", "Output Device:", nullptr));
        checkBox->setText(QCoreApplication::translate("Widget", "Auto Loop", nullptr));
        RefreshButton->setText(QCoreApplication::translate("Widget", "Refresh", nullptr));
        label_6->setText(QCoreApplication::translate("Widget", "Volume", nullptr));
        BrowseButton->setText(QCoreApplication::translate("Widget", "Browse", nullptr));
        label_7->setText(QCoreApplication::translate("Widget", "Output Sample Rate:", nullptr));
        label_8->setText(QCoreApplication::translate("Widget", "Bit Format:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
