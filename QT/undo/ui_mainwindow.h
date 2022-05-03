/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QUndoView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionClose;
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionExit;
    QAction *actionRed;
    QAction *actionGreen;
    QAction *actionBlue;
    QAction *actionAddRectangle;
    QAction *actionAddCircle;
    QAction *actionRemoveShape;
    QAction *actionAddRobot;
    QAction *actionAddSnowman;
    QAction *actionAddTriangle;
    QAction *actionAbout;
    QAction *actionAboutQt;
    QAction *actionExportPgms;

    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QTabWidget *documentTabs;
    QWidget *tab;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuShape;
    QMenu *menuMacros;
    QMenu *menuHelp;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QToolBar *shapeToolBar;
    QDockWidget *dockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QLabel *label;
    QSpinBox *undoLimit;
    QUndoView *undoView;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(567, 600);
        MainWindow->setIconSize(QSize(32, 32));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        const QIcon icon = QIcon(QString::fromUtf8(":/icons/fileopen.png"));
        actionOpen->setIcon(icon);
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        const QIcon icon1 = QIcon(QString::fromUtf8(":/icons/fileclose.png"));
        actionClose->setIcon(icon1);
        actionNew = new QAction(MainWindow);
        actionNew->setObjectName(QString::fromUtf8("actionNew"));
        const QIcon icon2 = QIcon(QString::fromUtf8(":/icons/filenew.png"));
        actionNew->setIcon(icon2);
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        const QIcon icon3 = QIcon(QString::fromUtf8(":/icons/filesave.png"));
        actionSave->setIcon(icon3);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        const QIcon icon4 = QIcon(QString::fromUtf8(":/icons/exit.png"));
        actionExit->setIcon(icon4);
        actionRed = new QAction(MainWindow);
        actionRed->setObjectName(QString::fromUtf8("actionRed"));
        const QIcon icon5 = QIcon(QString::fromUtf8(":/icons/red.png"));
        actionRed->setIcon(icon5);
        actionGreen = new QAction(MainWindow);
        actionGreen->setObjectName(QString::fromUtf8("actionGreen"));
        const QIcon icon6 = QIcon(QString::fromUtf8(":/icons/green.png"));
        actionGreen->setIcon(icon6);
        actionBlue = new QAction(MainWindow);
        actionBlue->setObjectName(QString::fromUtf8("actionBlue"));
        const QIcon icon7 = QIcon(QString::fromUtf8(":/icons/blue.png"));
        actionBlue->setIcon(icon7);
        actionAddRectangle = new QAction(MainWindow);
        actionAddRectangle->setObjectName(QString::fromUtf8("actionAddRectangle"));
        const QIcon icon8 = QIcon(QString::fromUtf8(":/icons/rectangle.png"));
        actionAddRectangle->setIcon(icon8);
        actionAddCircle = new QAction(MainWindow);
        actionAddCircle->setObjectName(QString::fromUtf8("actionAddCircle"));
        const QIcon icon9 = QIcon(QString::fromUtf8(":/icons/circle.png"));
        actionAddCircle->setIcon(icon9);
        actionRemoveShape = new QAction(MainWindow);
        actionRemoveShape->setObjectName(QString::fromUtf8("actionRemoveShape"));
        const QIcon icon10 = QIcon(QString::fromUtf8(":/icons/remove.png"));
        actionRemoveShape->setIcon(icon10);
        actionAddRobot = new QAction(MainWindow);
        actionAddRobot->setObjectName(QString::fromUtf8("actionAddRobot"));
        actionAddSnowman = new QAction(MainWindow);
        actionAddSnowman->setObjectName(QString::fromUtf8("actionAddSnowman"));
        actionAddTriangle = new QAction(MainWindow);
        actionAddTriangle->setObjectName(QString::fromUtf8("actionAddTriangle"));
        const QIcon icon11 = QIcon(QString::fromUtf8(":/icons/triangle.png"));
        actionAddTriangle->setIcon(icon11);

        actionExportPgms = new QAction(MainWindow);
        actionExportPgms->setObjectName(QString::fromUtf8("actionExportPgms"));
        const QIcon icon12 = QIcon(QString::fromUtf8(":/icons/export_to_pgms.png"));
        actionExportPgms->setIcon(icon12);




        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAboutQt = new QAction(MainWindow);
        actionAboutQt->setObjectName(QString::fromUtf8("actionAboutQt"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        vboxLayout = new QVBoxLayout(centralwidget);
        vboxLayout->setSpacing(0);
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        documentTabs = new QTabWidget(centralwidget);
        documentTabs->setObjectName(QString::fromUtf8("documentTabs"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        documentTabs->addTab(tab, QString());

        vboxLayout->addWidget(documentTabs);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 567, 27));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuShape = new QMenu(menubar);
        menuShape->setObjectName(QString::fromUtf8("menuShape"));
        menuMacros = new QMenu(menuShape);
        menuMacros->setObjectName(QString::fromUtf8("menuMacros"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setOrientation(Qt::Horizontal);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        shapeToolBar = new QToolBar(MainWindow);
        shapeToolBar->setObjectName(QString::fromUtf8("shapeToolBar"));
        shapeToolBar->setOrientation(Qt::Vertical);
        MainWindow->addToolBar(Qt::LeftToolBarArea, shapeToolBar);
        dockWidget = new QDockWidget(MainWindow);
        dockWidget->setObjectName(QString::fromUtf8("dockWidget"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        vboxLayout1 = new QVBoxLayout(dockWidgetContents);
        vboxLayout1->setSpacing(4);
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setContentsMargins(0, 0, 0, 0);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        label = new QLabel(dockWidgetContents);
        label->setObjectName(QString::fromUtf8("label"));

        hboxLayout->addWidget(label);

        undoLimit = new QSpinBox(dockWidgetContents);
        undoLimit->setObjectName(QString::fromUtf8("undoLimit"));

        hboxLayout->addWidget(undoLimit);


        vboxLayout1->addLayout(hboxLayout);

        undoView = new QUndoView(dockWidgetContents);
        undoView->setObjectName(QString::fromUtf8("undoView"));
        undoView->setAlternatingRowColors(false);

        vboxLayout1->addWidget(undoView);

        dockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), dockWidget);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuShape->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionNew);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionSave);
        menuFile->addAction(actionClose);
        menuFile->addAction(actionExportPgms);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuShape->addSeparator();
        menuShape->addAction(actionAddCircle);
        menuShape->addAction(actionAddRectangle);
        menuShape->addAction(actionAddTriangle);
        menuShape->addAction(actionRemoveShape);
        menuShape->addAction(actionExportPgms);

        menuShape->addSeparator();
        menuShape->addAction(actionRed);
        menuShape->addAction(actionGreen);
        menuShape->addAction(actionBlue);
        menuShape->addSeparator();
        menuShape->addAction(menuMacros->menuAction());
        menuMacros->addAction(actionAddRobot);
        menuMacros->addAction(actionAddSnowman);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAboutQt);
        toolBar->addAction(actionNew);
        toolBar->addAction(actionOpen);
        toolBar->addAction(actionSave);
        toolBar->addAction(actionClose);
        toolBar->addAction(actionExportPgms);
        toolBar->addSeparator();
        shapeToolBar->addAction(actionAddRectangle);
        shapeToolBar->addAction(actionAddCircle);
        shapeToolBar->addAction(actionAddTriangle);
        shapeToolBar->addAction(actionRemoveShape);

        shapeToolBar->addSeparator();
        shapeToolBar->addAction(actionRed);
        shapeToolBar->addAction(actionGreen);
        shapeToolBar->addAction(actionBlue);

        retranslateUi(MainWindow);

        documentTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        actionOpen->setText(QApplication::translate("MainWindow", "&Open", nullptr));
        actionClose->setText(QApplication::translate("MainWindow", "&Close", nullptr));
        actionNew->setText(QApplication::translate("MainWindow", "&New", nullptr));
        actionExportPgms->setText(QApplication::translate("MainWindow", "Export Pgms", nullptr));
        actionSave->setText(QApplication::translate("MainWindow", "&Save", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "E&xit", nullptr));
        actionRed->setText(QApplication::translate("MainWindow", "Red", nullptr));
        actionGreen->setText(QApplication::translate("MainWindow", "Green", nullptr));
        actionBlue->setText(QApplication::translate("MainWindow", "Blue", nullptr));
        actionAddRectangle->setText(QApplication::translate("MainWindow", "Add Rectangle", nullptr));
        actionAddCircle->setText(QApplication::translate("MainWindow", "Add Circle", nullptr));
        actionRemoveShape->setText(QApplication::translate("MainWindow", "Remove Shape", nullptr));
        actionAddRobot->setText(QApplication::translate("MainWindow", "Add robot", nullptr));
        actionAddSnowman->setText(QApplication::translate("MainWindow", "Add snowan", nullptr));
        actionAddTriangle->setText(QApplication::translate("MainWindow", "addTriangle", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        actionAboutQt->setText(QApplication::translate("MainWindow", "About Qt", nullptr));
        documentTabs->setTabText(documentTabs->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", nullptr));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", nullptr));
        menuShape->setTitle(QApplication::translate("MainWindow", "Edit", nullptr));
        menuMacros->setTitle(QApplication::translate("MainWindow", "Macros", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "File actions", nullptr));
        shapeToolBar->setWindowTitle(QApplication::translate("MainWindow", "Shape actions", nullptr));
        dockWidget->setWindowTitle(QApplication::translate("MainWindow", "Undo Stack", nullptr));
        label->setText(QApplication::translate("MainWindow", "Undo limit", nullptr));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
