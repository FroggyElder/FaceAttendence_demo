#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "ElaWindow.h"

#include "ElaToolBar.h"


class MainWindow : public ElaWindow
{
    Q_OBJECT
    \
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ElaToolBar* m_mainToolbar{new ElaToolBar};
};
#endif // MAINWINDOW_H
