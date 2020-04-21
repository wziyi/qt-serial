#ifndef WZY_H
#define WZY_H

#include <QMainWindow>
#include <QWidget>

class wzy : public QMainWindow
{
    Q_OBJECT
public:
    explicit wzy(QWidget *parent = nullptr);
};

class w_os_information
{
public:
    void debug();
    QString wmic(int i);    //  acquare os information 获取系统信息

};

class w_transform
{
public:
    void debug();
    char char_to_hex(char ch);
    QByteArray string_to_hex(QString str);
};

#endif // WZY_H
