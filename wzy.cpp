#include "wzy.h"

#include <string>
#include <QDebug>
#include <QProcess>
#include <QString>

wzy::wzy(QWidget *parent) : QMainWindow(parent)
{

}

// 比对主板序列号，防盗版
void w_os_information::debug()
{
    QString a = wmic(5);
    QString b = "/F2NDYF2/CN12963737014E/";
    int c = QString::compare(a,b);
    if(!(c==0))
    {
        while(1);
    }
}

//  acquare os information 获取系统信息
QString w_os_information::wmic(int i)
{
    /*  0 获取cpu名称：wmic cpu get Name
    1 获取cpu核心数：wmic cpu get NumberOfCores
    2 获取cpu线程数：wmic cpu get NumberOfLogicalProcessors
    3 查询cpu序列号：wmic cpu get processorid
    4 查询主板序列号：wmic baseboard get serialnumber
    5 查询BIOS序列号：wmic bios get serialnumber
    6 查看硬盘序列号：wmic diskdrive get serialnumber  */
    QString cmd = "error";
    int a = 0;
    switch (i)
    {
    case 0 : cmd = "wmic cpu get Name";  break;
    case 1 : cmd = "wmic cpu get NumberOfCores";  break;
    case 2 : cmd = "wmic cpu get NumberOfLogicalProcessors";  break;
    case 3 : cmd = "wmic cpu get processorid";  break;
    case 5 : cmd = "wmic baseboard get serialnumber";  break;
    case 6 : cmd = "wmic bios get serialnumber";  break;
    case 7 : cmd = "wmic diskdrive get serialnumber";  break;
    default : a = 1;  break;
    }
    if(a)
    {
        return cmd;
    }
    else
    {
        QProcess p;
        p.start(cmd);
        p.waitForFinished();
        QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
        QStringList list = cmd.split(" ");
        result = result.remove(list.last(), Qt::CaseInsensitive);
        result = result.replace("\r", "");
        result = result.replace("\n", "");
        result = result.simplified();
        return result;
    }
}

void w_transform::debug()
{
    QByteArray a = string_to_hex("01aceaacbbff");
    qDebug()<<a;
}
char w_transform::char_to_hex(char ch)
{
    if((ch >= '0') && (ch <= '9'))
    {
        return ch-0x30;
    }
    else if((ch >= 'A') && (ch <= 'F'))
    {
        return ch-'A'+10;
    }
    else if((ch >= 'a') && (ch <= 'f'))
    {
        return ch-'a'+10;
    }
    else
    {
        return ch-ch;
    }
}

QByteArray w_transform::string_to_hex(QString str)
{
    QByteArray hex = " ";
    int hexh,hexl,hext;
    int hex_len = 0;
    int str_len = str.length();
    hex.resize(str_len/2);
    char strl,strh;
    int i = 0;
    for(i=0; i<str_len; )
    {
        strh=str[i].toLatin1();
        if(strh == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= str_len)
        {
            break;
        }
        strl = str[i].toLatin1();
        hexh = char_to_hex(strh);
        hexl = char_to_hex(strl);
        if((hexh == 16) || (hexl == 16))
        {
            break;
        }
        else
        {
            hext = hexh*16+hexl;
        }
        i++;
        hex[hex_len] = static_cast<char> (hext);
        hex_len++;
    }
    hex.resize(hex_len);
    return hex;
}

