#include "MainWindow.h"

#include "showheadimgpanel.h"
#include "registerpanel.h"
#include "dbquerypanel.h"

MainWindow::MainWindow(QWidget *parent)
    : ElaWindow(parent)
{
    //hide info card
    setUserInfoCardVisible(false);

    addPageNode("Avatar", new showHeadImgPanel, ElaIconType::FaceAwesome);
    addPageNode("Log", new dbQueryPanel, ElaIconType::IdCard);
    addPageNode("Register", new registerPanel, ElaIconType::IdCard);
}

MainWindow::~MainWindow()
{

}
