/********************************************************************************
** Form generated from reading UI file 'MonoStudioWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONOSTUDIOWINDOW_H
#define UI_MONOSTUDIOWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MonoStudioWindow
{
public:

    void setupUi(QWidget *MonoStudioWindow)
    {
        if (MonoStudioWindow->objectName().isEmpty())
            MonoStudioWindow->setObjectName(QString::fromUtf8("MonoStudioWindow"));
        MonoStudioWindow->resize(650, 421);

        retranslateUi(MonoStudioWindow);

        QMetaObject::connectSlotsByName(MonoStudioWindow);
    } // setupUi

    void retranslateUi(QWidget *MonoStudioWindow)
    {
        MonoStudioWindow->setWindowTitle(QCoreApplication::translate("MonoStudioWindow", "MonoStudioWindow", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MonoStudioWindow: public Ui_MonoStudioWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONOSTUDIOWINDOW_H
