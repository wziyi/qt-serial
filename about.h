#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class about;
}

class about : public QMainWindow
{
    Q_OBJECT

public:
    explicit about(QWidget *parent = nullptr);
    ~about();

private:
    Ui::about *ui;

public:


public slots:
    void about_qt_slot();
};

#endif // ABOUT_H
