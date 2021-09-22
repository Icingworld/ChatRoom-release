#include "copyright.h"
#include "ui_copyright.h"

copyright::copyright(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::copyright)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(),this->height());
}

copyright::~copyright()
{
    delete ui;
}
