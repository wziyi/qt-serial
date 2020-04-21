#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "about.h"
#include "wzy.h"

namespace Ui {
class MainWindow;
}

struct settings //  serial argv 串口参数
{
    QString name;       // 串口号

    qint32 baudRate;    //波特率
    QString stringBaudRate;

    QSerialPort::DataBits dataBits;//数据位
    QString stringDataBits;

    QSerialPort::Parity parity;    //校验位
    QString stringParity;

    QSerialPort::StopBits stopBits;//停止位
    QString stringStopBits;

    QSerialPort::FlowControl flowControl;   //控制流
    QString stringFlowControl;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

public:
    void display_clear_auto();  //  auto clear diplay 自动清空屏幕
    void serial_set_init(); //  serial inital 串口初始化
    void serial_set();      //  serial set 串口设置
    void serial_open();     //  open serial 打开串口
    void serial_close();    //  close serial 关闭串口
    void serial_write(const QString data);  //  write serial 写串口
    void serial_read();     //  read serial 读串口
    void setting_default(); //  serial default set 软件默认设置
    void setting_write();
    void setting_read();

private slots:
    void close_window_slot();   //  close progress 关闭窗口
    void send_char_slot();      //  send line_input's string 发送文本框里的字符串
    void clear_display_slot();  //  clear diplay 清空屏幕
    void open_txet_slot();      //  open txet 打开文本文件
    void save_txet_slot();      //  save txet 保存文本文件
    void about_window_slot();   //  about window 关于
    void switch_serial_slot();  //  serial switch 串口开关
    void search_serial_slot();  //  search serial 搜索串口
    void read_serial_slot();    //  serial receive 串口接收
    void pause_serial_slot();   //  pause receive 暂停接收
    void recover_serial_slot(); //  recover receive 恢复接收
    void recover_default_slot();//  恢复默认设置

private:
    about *m_about = nullptr;   // about window class 关于窗口类
    QSerialPort m_serial;       // serial class 串口类
    settings m_currentSettings; // setting struct 设置结构体
    w_transform m_t;            // char 与 hex 互相转换

};

#endif // MAINWINDOW_H
