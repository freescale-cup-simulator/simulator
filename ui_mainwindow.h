/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.1.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGroupBox *groupBox;
    QWidget *layoutWidget_3;
    QFormLayout *formLayout_5;
    QLabel *label_11;
    QDoubleSpinBox *rotX;
    QLabel *label_12;
    QDoubleSpinBox *rotY;
    QLabel *label_13;
    QDoubleSpinBox *rotZ;
    QWidget *layoutWidget_4;
    QFormLayout *formLayout;
    QLabel *label;
    QDoubleSpinBox *poseX;
    QLabel *label_2;
    QDoubleSpinBox *poseY;
    QLabel *label_3;
    QDoubleSpinBox *poseZ;
    QGroupBox *groupBox_2;
    QLabel *picture;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(300, 228);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(0, 0, 291, 221));
        layoutWidget_3 = new QWidget(groupBox);
        layoutWidget_3->setObjectName(QStringLiteral("layoutWidget_3"));
        layoutWidget_3->setGeometry(QRect(10, 110, 117, 83));
        formLayout_5 = new QFormLayout(layoutWidget_3);
        formLayout_5->setSpacing(6);
        formLayout_5->setContentsMargins(11, 11, 11, 11);
        formLayout_5->setObjectName(QStringLiteral("formLayout_5"));
        formLayout_5->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget_3);
        label_11->setObjectName(QStringLiteral("label_11"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_11);

        rotX = new QDoubleSpinBox(layoutWidget_3);
        rotX->setObjectName(QStringLiteral("rotX"));
        rotX->setMinimum(-1000);
        rotX->setMaximum(1000.99);
        rotX->setSingleStep(0.01);

        formLayout_5->setWidget(0, QFormLayout::FieldRole, rotX);

        label_12 = new QLabel(layoutWidget_3);
        label_12->setObjectName(QStringLiteral("label_12"));

        formLayout_5->setWidget(1, QFormLayout::LabelRole, label_12);

        rotY = new QDoubleSpinBox(layoutWidget_3);
        rotY->setObjectName(QStringLiteral("rotY"));
        rotY->setMinimum(-1000);
        rotY->setMaximum(1000);
        rotY->setSingleStep(0.01);

        formLayout_5->setWidget(1, QFormLayout::FieldRole, rotY);

        label_13 = new QLabel(layoutWidget_3);
        label_13->setObjectName(QStringLiteral("label_13"));

        formLayout_5->setWidget(2, QFormLayout::LabelRole, label_13);

        rotZ = new QDoubleSpinBox(layoutWidget_3);
        rotZ->setObjectName(QStringLiteral("rotZ"));
        rotZ->setMinimum(-1000);
        rotZ->setMaximum(1000);
        rotZ->setSingleStep(0.01);

        formLayout_5->setWidget(2, QFormLayout::FieldRole, rotZ);

        layoutWidget_4 = new QWidget(groupBox);
        layoutWidget_4->setObjectName(QStringLiteral("layoutWidget_4"));
        layoutWidget_4->setGeometry(QRect(12, 21, 109, 83));
        formLayout = new QFormLayout(layoutWidget_4);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget_4);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        poseX = new QDoubleSpinBox(layoutWidget_4);
        poseX->setObjectName(QStringLiteral("poseX"));
        poseX->setMinimum(-1000);
        poseX->setMaximum(1000.99);
        poseX->setSingleStep(0.01);

        formLayout->setWidget(0, QFormLayout::FieldRole, poseX);

        label_2 = new QLabel(layoutWidget_4);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        poseY = new QDoubleSpinBox(layoutWidget_4);
        poseY->setObjectName(QStringLiteral("poseY"));
        poseY->setMinimum(-1000);
        poseY->setMaximum(1000);
        poseY->setSingleStep(0.01);

        formLayout->setWidget(1, QFormLayout::FieldRole, poseY);

        label_3 = new QLabel(layoutWidget_4);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        poseZ = new QDoubleSpinBox(layoutWidget_4);
        poseZ->setObjectName(QStringLiteral("poseZ"));
        poseZ->setMinimum(-1000);
        poseZ->setMaximum(1000);
        poseZ->setSingleStep(0.01);

        formLayout->setWidget(2, QFormLayout::FieldRole, poseZ);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(130, 130, 151, 51));
        picture = new QLabel(groupBox_2);
        picture->setObjectName(QStringLiteral("picture"));
        picture->setGeometry(QRect(10, 30, 141, 20));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        groupBox->setTitle(QApplication::translate("MainWindow", "Camera position", 0));
        label_11->setText(QApplication::translate("MainWindow", "RX:", 0));
        label_12->setText(QApplication::translate("MainWindow", "RY:", 0));
        label_13->setText(QApplication::translate("MainWindow", "RZ:", 0));
        label->setText(QApplication::translate("MainWindow", "X:", 0));
        label_2->setText(QApplication::translate("MainWindow", "Y:", 0));
        label_3->setText(QApplication::translate("MainWindow", "Z:", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Camera frame", 0));
        picture->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
