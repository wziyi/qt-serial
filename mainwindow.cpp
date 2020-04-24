#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDataStream>
#include <QFile>
#include <QTextStream>
#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextBrowser>
#include <QTimer>
#include "QSettings"
#include "wzy.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //  默认配置
    connect(this->ui->push_debug,SIGNAL(clicked(bool)),this,SLOT(recover_default_slot()));
    //  exit 退出
    connect(this->ui->push_exit,SIGNAL(clicked(bool)),this,SLOT(close_window_slot()));
    //  send 发送
    connect(this->ui->push_send,SIGNAL(clicked(bool)),this,SLOT(send_char_slot()));
    //  clear 清空
    connect(this->ui->push_clear,SIGNAL(clicked(bool)),this,SLOT(clear_display_slot()));
    //  open 打开
    connect(this->ui->push_open_txet,SIGNAL(clicked(bool)),this,SLOT(open_txet_slot()));
    //  save 保存
    connect(this->ui->push_save_txet,SIGNAL(clicked(bool)),this,SLOT(save_txet_slot()));
    //  about 关于
    connect(this->ui->push_about,SIGNAL(clicked(bool)),this,SLOT(about_window_slot()));
    //  serial switch 串口开关
    connect(this->ui->push_serial,SIGNAL(clicked(bool)),this,SLOT(switch_serial_slot()));
    //  search serial 搜索串口
    connect(this->ui->push_search_serial,SIGNAL(clicked(bool)),this,SLOT(search_serial_slot()));
    //  serial receive 串口接收
    connect(&m_serial,SIGNAL(readyRead()),this,SLOT(read_serial_slot()));
    //  pause receive 暂停接收
    connect(this->ui->push_pause,SIGNAL(clicked(bool)),this,SLOT(pause_serial_slot()));
    //  recover receive 恢复接收
    connect(this->ui->push_recover,SIGNAL(clicked(bool)),this,SLOT(recover_serial_slot()));

    serial_set_init();  // 初始化串口
    setting_read();     // 读取上次配置信息
    this->ui->radio_serial->setChecked(0);  //初始化串口开关
}

MainWindow::~MainWindow()
{
    setting_write();        // 保存这次配置信息
    if (m_about != nullptr) // 释放指针
    {
        delete m_about;
        m_about = nullptr;
    }
    delete ui;
}

//  slot function 槽函数
//  debug 调试
void MainWindow::recover_default_slot()
{
    setting_default();
}
//  close progress 关闭窗口
void MainWindow::close_window_slot()
{
    m_about->close();
    this->close();
}
//  send  string 发送字符串
void MainWindow::send_char_slot()
{  
    QString a = this->ui->line_input->text();   //读取文本框内的字符串

    if(tx_mode)
    {
        this->ui->text_display->append("TX - hex\n"+a);
    }
    else
    {
        this->ui->text_display->append("TX - char\n"+a);
    }

    display_clear_auto();       //  auto clear 自动清空屏幕
    serial_write(a, tx_mode);   //  write serial 写串口
}
//  clear diplay 清空屏幕
void MainWindow::clear_display_slot()
{
    // clear textbrowser 清空屏幕
    this->ui->text_display->setText("");
}
//  open txet 打开文本文件
void MainWindow::open_txet_slot()
{
    QString file_path;
    //  default txet's path 默认文本的路径
    if(file_path.isEmpty())
    {
        QString file_path = ".";
    }
    file_path = QFileDialog::getOpenFileName(this,"打开文本",file_path,"*.txt");
    //  judge path Whether or not right 判断路径是否正确
    if(!file_path.isEmpty())
    {
        //  take text to a text box 将文本读入发送框
        QFile file_txet(file_path);
        file_txet.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray file_data = file_txet.readAll();
        QString a = QString(file_data);
        //  remove line break 除掉换行符
        a.remove(QRegExp("\\n"));
        this->ui->line_input->setText(a);
        file_txet.close();
    }
    else
    {
        //  open error window 弹出错误信息窗口
        QMessageBox::information(this,"错误","文件打开失败","确定");
    }
}
//  save txet 保存文本文件
void MainWindow::save_txet_slot()
{
    QString file_path;
    //  default txet's path 默认文本的路径
    if(file_path.isEmpty())
    {
        QString file_path = ".";
    }
    //  accept txet's path 获得文本的路径
    file_path = QFileDialog::getSaveFileName(this,"打开文本",file_path,"*.txt");
    //  judge path Whether or not right 判断路径是否正确
    if(!file_path.isEmpty())
    {
        //  Setting file path and name 设置文件路径和名称
        QFile file_txet(file_path);
        file_txet.open(QIODevice::WriteOnly | QIODevice::Text);
        //  Docking output stream to file 将输出流与文件对接
        QTextStream file_output(&file_txet);
        //  Read the data on the screen 读取屏幕上的数据
        QString file_data = this->ui->text_display->toPlainText();
        file_output<<file_data;
        file_txet.close();
    }
    else
    {
        //  open error window 弹出错误信息窗口
        QMessageBox::information(this,"错误","文件保存失败","确定");
    }
}
// about window 关于
void MainWindow::about_window_slot()
{
    m_about=new about;
    //QEventLoop w;
    //m_about->setAttribute(Qt::WA_DeleteOnClose, true);
    m_about->show();
    //w.exec();
    //qDebug()<<"z";
}
//  serial switch 串口开关
void MainWindow::switch_serial_slot()
{
    serial_set();
    if(!this->ui->radio_serial->isChecked())
    {
        this->ui->radio_serial->setChecked(1);
        serial_open();
    }
    else
    {
        this->ui->radio_serial->setChecked(0);
        serial_close();
    }
}
//  search serial 搜索串口
void MainWindow::search_serial_slot()
{
    const auto infos = QSerialPortInfo::availablePorts();
    //  statistics serial numbers 统计串口数量
    int num = infos.count();
    //  int to QSrring 整型转换为字符串型
    QString num_qs = QString::number(num,10,0);
    QString out = "Total number of ports available: " + num_qs;
    //  send data to text_display 将数据显示在屏幕上
    QString a = out;
    for (const QSerialPortInfo &info : infos)
    {
        QString b = QObject::tr("Port: ") + info.portName() + "\n"
                + QObject::tr("Location: ") + info.systemLocation() + "\n"
                + QObject::tr("Description: ") + info.description() + "\n"
                + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("Busy: ") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";
        a = a+"\n"+b;
    }
    this->ui->text_display->setText(a);
}
//  serial receive 串口接收
void MainWindow::read_serial_slot()
{
    QString a= serial_read(rx_mode);
    display_clear_auto();   //  auto clear 自动清空屏幕
    if(!a.isEmpty())
    {
        if(rx_mode)
        {
            this->ui->text_display->append("RX - hex\n"+a);
        }
        else
        {
            this->ui->text_display->append("RX - char\n"+a);
        }
    }
}
//  pause receive 暂停接收
void MainWindow::pause_serial_slot()
{
    disconnect(&m_serial,SIGNAL(readyRead()),this,SLOT(read_serial_slot()));
}
//  recover receive 恢复接收
void MainWindow::recover_serial_slot()
{
    connect(&m_serial,SIGNAL(readyRead()),this,SLOT(read_serial_slot()));
}

//  public function 公共函数
//  auto clear diplay 自动清空屏幕
void MainWindow::display_clear_auto()
{
    //  judge clear_diplay_auto status 判断是否开启自动清空
    int status_auto = this->ui->check_clear->isChecked();
    if(status_auto)
    {
        // statistics char number 统计屏幕上的字符数量
        int char_num = this->ui->text_display->toPlainText().count();
        //  over char number will auto clear textbrowser 屏幕上的字符数量超过上限则自动清除
        if(char_num > 1000)
        {
            this->ui->text_display->setText("");
        }
    }
}
//  serial inital 串口初始化
void MainWindow::serial_set_init()
{
    this->ui->combox_baudrate->addItem(QStringLiteral("1200"), QSerialPort::Baud1200);
    this->ui->combox_baudrate->addItem(QStringLiteral("2400"), QSerialPort::Baud2400);
    this->ui->combox_baudrate->addItem(QStringLiteral("4800"), QSerialPort::Baud4800);
    this->ui->combox_baudrate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    this->ui->combox_baudrate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    this->ui->combox_baudrate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    this->ui->combox_baudrate->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    this->ui->combox_baudrate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    this->ui->combox_databit->addItem(QStringLiteral("5"), QSerialPort::Data5);
    this->ui->combox_databit->addItem(QStringLiteral("6"), QSerialPort::Data6);
    this->ui->combox_databit->addItem(QStringLiteral("7"), QSerialPort::Data7);
    this->ui->combox_databit->addItem(QStringLiteral("8"), QSerialPort::Data8);

    this->ui->combox_parity->addItem(tr("None"), QSerialPort::NoParity);
    this->ui->combox_parity->addItem(tr("Even"), QSerialPort::EvenParity);
    this->ui->combox_parity->addItem(tr("Odd"), QSerialPort::OddParity);
    this->ui->combox_parity->addItem(tr("Mark"), QSerialPort::MarkParity);
    this->ui->combox_parity->addItem(tr("Space"), QSerialPort::SpaceParity);

    this->ui->combox_stopbit->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    this->ui->combox_stopbit->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    this->ui->combox_stopbit->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    this->ui->combox_flow->addItem(tr("None"), QSerialPort::NoFlowControl);
    this->ui->combox_flow->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    this->ui->combox_flow->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}
//  serial set 串口设置
void MainWindow::serial_set()
{
    m_currentSettings.name =this->ui->combox_name->currentText();

    m_currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(
                this->ui->combox_baudrate->itemData(this->ui->combox_baudrate->currentIndex()).toInt());
    m_currentSettings.stringBaudRate = QString::number(m_currentSettings.baudRate);

    m_currentSettings.dataBits = static_cast<QSerialPort::DataBits>(
                this->ui->combox_databit->itemData(this->ui->combox_databit->currentIndex()).toInt());
    m_currentSettings.stringDataBits =this->ui->combox_databit->currentText();

    m_currentSettings.parity = static_cast<QSerialPort::Parity>(
                this->ui->combox_parity->itemData(this->ui->combox_parity->currentIndex()).toInt());
    m_currentSettings.stringParity = this->ui->combox_parity->currentText();

    m_currentSettings.stopBits = static_cast<QSerialPort::StopBits>(
                this->ui->combox_stopbit->itemData(this->ui->combox_stopbit->currentIndex()).toInt());
    m_currentSettings.stringStopBits = this->ui->combox_stopbit->currentText();

    m_currentSettings.flowControl = static_cast<QSerialPort::FlowControl>(
                this->ui->combox_flow->itemData(this->ui->combox_flow->currentIndex()).toInt());
    m_currentSettings.stringFlowControl = this->ui->combox_flow->currentText();

    tx_mode = this->ui->combox_name_2->currentIndex();
    rx_mode = this->ui->combox_name_3->currentIndex();
}
//  open serial 打开串口
void MainWindow::serial_open()
{
    //  Setting Serial Port Parameters 设置串口参数
    const settings p = m_currentSettings;
    m_serial.setPortName(p.name);
    m_serial.setBaudRate(p.baudRate);
    m_serial.setDataBits(p.dataBits);
    m_serial.setParity(p.parity);
    m_serial.setStopBits(p.stopBits);
    m_serial.setFlowControl(p.flowControl);

    tx_mode = this->ui->combox_name_2->currentIndex();
    rx_mode = this->ui->combox_name_3->currentIndex();
    if (m_serial.open(QIODevice::ReadWrite))
    {
        this->ui->text_display->setText("串口打开");
    }
    else
    {
        QMessageBox::information(this,"错误","串口打开失败","确定");
        this->ui->radio_serial->setChecked(0);
    }
}
//  close serial 关闭串口
void MainWindow::serial_close()
{
    if (m_serial.isOpen())
    {
        m_serial.close();
        this->ui->text_display->setText("串口关闭");
    }
}
//  write serial 写串口
void MainWindow::serial_write(QString data, bool mode)
{
    QByteArray tx_send_byte;

    data = data.remove(QRegExp("\\s")); //  remove all space 除掉所有空格
    if(mode)    //hex 模式
    {
        tx_send_byte = m_t.string_to_hex(data);
    }
    else        //char 模式
    {
        tx_send_byte = data.toUtf8();
    }
    m_serial.write(tx_send_byte);
}
//  read serial 读串口
QString MainWindow::serial_read(bool mode)
{   
    QEventLoop loop;
    QString rx_display_string;  //经过处理后的显示在屏幕上的数据
    QByteArray rx_raw_byte;     //接收的原始数据

    QTimer::singleShot(100,&loop,SLOT(quit())); // 延时,以防数据接受不完整
    loop.exec();
    rx_raw_byte = m_serial.readAll();

    if(mode)    // hex 模式
    {
        QString a = rx_raw_byte.toHex();  //把原始数据转为字符串 a
        if (a.length() % 2) // 奇数个字符自动补零
        {
            a.insert(a.length(), '0');
        }
        //  Add a space between each two characters 每两个字符间加一个空格
        int b = a.length(); //确定字符串长度 b
        int i=0 , c=2;      //每隔 c 位插空格
        for (i=2;i<b;i=i+2)
        {
            a.insert(c," ");
            c=c+3;
        }
        rx_display_string = a;
    }
    else        //char 模式
    {
        rx_display_string = QString(rx_raw_byte);   //在 char 模式下直接转换成字符串
    }
    return rx_display_string;
}
//  soft default set 软件默认设置
void MainWindow::setting_default()
{    
    this->ui->radio_serial->setChecked(0);
    this->ui->combox_name->setCurrentIndex(0);
    this->ui->combox_baudrate->setCurrentIndex(3);
    this->ui->combox_databit->setCurrentIndex(3);
    this->ui->combox_stopbit->setCurrentIndex(0);
    this->ui->combox_parity->setCurrentIndex(0);
    this->ui->combox_flow->setCurrentIndex(0);

    this->ui->combox_name_2->setCurrentIndex(0);
    this->ui->combox_name_3->setCurrentIndex(0);
}
void MainWindow::setting_write()
{
    int a0,a1,a2,a3,a4,a5,a6,a7,a8;
    a0=this->ui->radio_serial->isChecked();
    a1=this->ui->combox_name->currentIndex();
    a2=this->ui->combox_baudrate->currentIndex();
    a3=this->ui->combox_databit->currentIndex();
    a4=this->ui->combox_stopbit->currentIndex();
    a5=this->ui->combox_parity->currentIndex();
    a6=this->ui->combox_flow->currentIndex();

    a7=this->ui->combox_name_2->currentIndex();
    a8=this->ui->combox_name_3->currentIndex();
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *IniWrite = new QSettings("wzy_serial.ini", QSettings::IniFormat);
    IniWrite->setValue("radio_serial", a0);
    IniWrite->setValue("combox_name", a1);
    IniWrite->setValue("combox_baudrate", a2);
    IniWrite->setValue("combox_databit", a3);
    IniWrite->setValue("combox_stopbit", a4);
    IniWrite->setValue("combox_parity", a5);
    IniWrite->setValue("combox_flow", a6);

    IniWrite->setValue("tx_mode", a7);
    IniWrite->setValue("rx_mode", a8);
    //写入完成后删除指针
    delete IniWrite;
}
void MainWindow::setting_read()
{
    int a0,a1,a2,a3,a4,a5,a6,a7,a8;
    //QSettings构造函数的第一个参数是ini文件的路径,第二个参数表示针对ini文件,第三个参数可以缺省
    QSettings *iniRead = new QSettings("wzy_serial.ini", QSettings::IniFormat);
    a0= iniRead->value("radio_serial").toInt();
    a1= iniRead->value("combox_name").toInt();
    a2= iniRead->value("combox_baudrate").toInt();
    a3= iniRead->value("combox_databit").toInt();
    a4= iniRead->value("combox_stopbit").toInt();
    a5= iniRead->value("combox_parity").toInt();
    a6= iniRead->value("combox_flow").toInt();

    a7= iniRead->value("tx_mode").toInt();
    a8= iniRead->value("rx_mode").toInt();
    //读入完成后删除指针
    delete iniRead;
    this->ui->radio_serial->setChecked(a0);
    this->ui->combox_name->setCurrentIndex(a1);
    this->ui->combox_baudrate->setCurrentIndex(a2);
    this->ui->combox_databit->setCurrentIndex(a3);
    this->ui->combox_stopbit->setCurrentIndex(a4);
    this->ui->combox_parity->setCurrentIndex(a5);
    this->ui->combox_flow->setCurrentIndex(a6);

    this->ui->combox_name_2->setCurrentIndex(a7);
    this->ui->combox_name_3->setCurrentIndex(a8);
}
