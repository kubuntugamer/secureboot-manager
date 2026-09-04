/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *mainHorizontalLayout;
    QVBoxLayout *sidebarLayout;
    QListWidget *listWidget;
    QTextBrowser *text_raw_certificate_;
    QStackedWidget *stackedWidget;
    QWidget *page_enrolled_keys;
    QVBoxLayout *page1Layout;
    QTableWidget *table_enrolled_keys_;
    QWidget *page_generate_keys;
    QWidget *page_sign_binaries;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainHorizontalLayout = new QHBoxLayout(centralwidget);
        mainHorizontalLayout->setSpacing(10);
        mainHorizontalLayout->setObjectName("mainHorizontalLayout");
        mainHorizontalLayout->setContentsMargins(10, 10, 10, 10);
        sidebarLayout = new QVBoxLayout();
        sidebarLayout->setSpacing(10);
        sidebarLayout->setObjectName("sidebarLayout");
        listWidget = new QListWidget(centralwidget);
        QListWidgetItem *__qlistwidgetitem = new QListWidgetItem(listWidget);
        __qlistwidgetitem->setTextAlignment(Qt::AlignCenter);
        QListWidgetItem *__qlistwidgetitem1 = new QListWidgetItem(listWidget);
        __qlistwidgetitem1->setTextAlignment(Qt::AlignCenter);
        QListWidgetItem *__qlistwidgetitem2 = new QListWidgetItem(listWidget);
        __qlistwidgetitem2->setTextAlignment(Qt::AlignCenter);
        listWidget->setObjectName("listWidget");
        listWidget->setMaximumSize(QSize(220, 116));
        listWidget->setFocusPolicy(Qt::FocusPolicy::NoFocus);

        sidebarLayout->addWidget(listWidget);

        text_raw_certificate_ = new QTextBrowser(centralwidget);
        text_raw_certificate_->setObjectName("text_raw_certificate_");
        text_raw_certificate_->setMaximumSize(QSize(220, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8("Monospace")});
        font.setPointSize(8);
        text_raw_certificate_->setFont(font);

        sidebarLayout->addWidget(text_raw_certificate_);


        mainHorizontalLayout->addLayout(sidebarLayout);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        page_enrolled_keys = new QWidget();
        page_enrolled_keys->setObjectName("page_enrolled_keys");
        page1Layout = new QVBoxLayout(page_enrolled_keys);
        page1Layout->setObjectName("page1Layout");
        page1Layout->setContentsMargins(0, 0, 0, 0);
        table_enrolled_keys_ = new QTableWidget(page_enrolled_keys);
        if (table_enrolled_keys_->columnCount() < 3)
            table_enrolled_keys_->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table_enrolled_keys_->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        table_enrolled_keys_->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        table_enrolled_keys_->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        table_enrolled_keys_->setObjectName("table_enrolled_keys_");
        table_enrolled_keys_->horizontalHeader()->setStretchLastSection(true);

        page1Layout->addWidget(table_enrolled_keys_);

        stackedWidget->addWidget(page_enrolled_keys);
        page_generate_keys = new QWidget();
        page_generate_keys->setObjectName("page_generate_keys");
        stackedWidget->addWidget(page_generate_keys);
        page_sign_binaries = new QWidget();
        page_sign_binaries->setObjectName("page_sign_binaries");
        stackedWidget->addWidget(page_sign_binaries);

        mainHorizontalLayout->addWidget(stackedWidget);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1100, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "KDE Secure Boot Manager", nullptr));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QCoreApplication::translate("MainWindow", "Enrolled MOK Keys", nullptr));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QCoreApplication::translate("MainWindow", "Key Generation", nullptr));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QCoreApplication::translate("MainWindow", "Sign Binaries", nullptr));
        listWidget->setSortingEnabled(__sortingEnabled);

        QTableWidgetItem *___qtablewidgetitem = table_enrolled_keys_->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Common Name (CN)", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table_enrolled_keys_->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Expiration Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table_enrolled_keys_->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Serial Number", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
