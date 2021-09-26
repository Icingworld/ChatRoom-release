#include "copyright.h"
#include "ui_copyright.h"
#include <QDesktopServices>
#include <QUrl>

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

void copyright::on_pushButton_clicked()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://github.com/Icingworld/ChatRoom-release")));
}
