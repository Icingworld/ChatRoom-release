#ifndef COPYRIGHT_H
#define COPYRIGHT_H

#include <QWidget>

namespace Ui {
class copyright;
}

class copyright : public QWidget
{
    Q_OBJECT

public:
    explicit copyright(QWidget *parent = nullptr);
    ~copyright();

private:
    Ui::copyright *ui;
};

#endif // COPYRIGHT_H
