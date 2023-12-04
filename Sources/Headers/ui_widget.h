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
#include <QtWidgets/QWidget>
#include "clickableqslider.hpp"

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    ClickableQSlider *horizontalSlider;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *comboBox;
    QLabel *label_5;
    QCheckBox *checkBox;
    QPushButton *pushButton_5;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(845, 582);
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(150, 320, 141, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(12);
        pushButton->setFont(font);
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(350, 320, 131, 51));
        pushButton_2->setFont(font);
        pushButton_3 = new QPushButton(Widget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(530, 320, 131, 51));
        pushButton_3->setFont(font);
        pushButton_4 = new QPushButton(Widget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(640, 70, 81, 31));
        pushButton_4->setFont(font);
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
        pushButton_5 = new QPushButton(Widget);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(560, 450, 75, 24));

        retranslateUi(Widget);
        QObject::connect(pushButton, SIGNAL(clicked()), Widget, SLOT(Play()));
        QObject::connect(pushButton_3, SIGNAL(clicked()), Widget, SLOT(Stop()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), Widget, SLOT(Pause()));
        QObject::connect(pushButton_4, SIGNAL(clicked()), Widget, SLOT(Open()));
        QObject::connect(pushButton_5, SIGNAL(clicked()), Widget, SLOT(UpdateDeviceList()));

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "Play", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "Pause", nullptr));
        pushButton_3->setText(QCoreApplication::translate("Widget", "Stop", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Widget", "Open", nullptr));
        label->setText(QCoreApplication::translate("Widget", "Playing: ", nullptr));
        lineEdit->setText(QString());
        label_2->setText(QCoreApplication::translate("Widget", "URL:", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "Duration:", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "Current:", nullptr));
        label_5->setText(QCoreApplication::translate("Widget", "Output Device:", nullptr));
        checkBox->setText(QCoreApplication::translate("Widget", "Auto Loop", nullptr));
        pushButton_5->setText(QCoreApplication::translate("Widget", "Refresh", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
