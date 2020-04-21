#include "about.h"
#include "ui_about.h"

about::about(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::about)
{
    ui->setupUi(this);

    //  about 关于
    connect(this->ui->push_qt,SIGNAL(clicked(bool)),this,SLOT(about_qt_slot()));
}

about::~about()
{
    delete ui;
}

//  about qt 致谢qt
void about::about_qt_slot()
{
    QApplication::aboutQt();    //致谢
}
