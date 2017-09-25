#include "tools.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QObject>

static QStringList list;

//获取当前PC的串口名
QStringList getSerialName()
{
    QStringList temp;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            if (!list.contains(info.portName(), Qt::CaseSensitive)) {
                list.insert(0, info.portName());
            }
            serial.close();
            temp << info.portName();
        }
    }
    for (int i = 0 ; i < list.size() ; i ++)
    {
        if (!temp.contains(list.at(i))) {
            list.removeAt(i);
        }
    }
    return list;
}

///获取当前日期和时间
QString CurrentDateTime()
{
    QDateTime dt;
    QTime time;
    QDate date;
    dt.setTime(time.currentTime());
    dt.setDate(date.currentDate());
    return dt.toString("yyyy-MM-dd hh:mm:ss");
}

int CurrentMonthTime()
{
    QDateTime dt;
    QTime time;
    QDate date;
    dt.setTime(time.currentTime());
    dt.setDate(date.currentDate());
    //QString t = dt.toString("hh:mm:ss");
    //return QDateTime::fromString(t, "hh:mm:ss").toTime_t();
    return dt.toTime_t();
}
/**
 * @param src 用户输入的字符串
 * @return MD5字符串
 * MD5加密
 */
QString GetMD5String(QString src)
{
    QString result;
    QByteArray ba;
    ba = QCryptographicHash::hash(src.toLatin1(), QCryptographicHash::Md5);
    result.append(ba.toHex());
    return result;
}
///获取当前的时间
QString CurrentTime()
{
    QTime time;
    return time.currentTime().toString("hh:mm:ss");
}
///获取当前的时间
QString CurrentMTime()
{
    QTime time;
    return time.currentTime().toString("hh:mm:ss.zzz");
}
///普通字符串转为16进制字符串
QString  CharStringtoHexString(QString space, const char * src, int len)
{
    QString hex = "";
    if (space == NULL)
    {
        for (int i = 0 ; i < len ; i ++)
        {
            hex += QString("%1").arg(src[i] & 0xFF, 2, 16, QLatin1Char('0'));
        }
        return hex.toUpper();
    }
    else
    {
        for (int i = 0 ; i < len ; i ++)
        {
            hex += space + QString("%1").arg(src[i] & 0xFF, 2, 16, QLatin1Char('0'));
        }
        return hex.right(hex.length() - space.length()).toUpper();
    }
}
//QString 转 Hex char *
quint8 StringToHex(QString string, quint8 *hex)
{
    QString temp;
    quint8 len = string.length();

    for (quint8 i = 0; i < len; i += 2)
    {
        temp = string.mid(i, 2);
        hex[i / 2] = (quint8)temp.toInt(0, 16);
    }
    return len / 2;
}

/**
 * @brief QSTRING_TO_HEX
 * @param str QString指针
 * @param buf 字节数组指针
 * @param len 要转化的长度
 * QString 转十六进制数组
 */
void QSTRING_TO_HEX(const QString &str, quint8 *buf, quint8 len)
{
    quint8 strlen = ((quint8)str.count() + 1) / 2;
    quint8 buflen = (strlen > len) ? len : strlen;
    QString strHex;
    for (quint8 i = 0; i < buflen; i++) {
        strHex = str.mid((i << 1), 2);
        buf[i] = (quint8)(strHex.toInt(0, 16));
    }
}
///普通字符串转为16进制字符串
QString  CharStringtoHexString(QString space, const char * src, int start, int end)
{
    QString hex = "";
    if (space == NULL)
    {
        for (int i = start ; i < end ; i ++)
        {
            hex += QString("%1").arg(src[i] & 0xFF, 2, 16, QLatin1Char('0'));
        }
        return hex.toUpper();
    }
    else
    {
        for (int i = start ; i < end ; i ++)
        {
            hex += space + QString("%1").arg(src[i] & 0xFF, 2, 16, QLatin1Char('0'));
        }
        return hex.right(hex.length() - space.length()).toUpper();
    }
}

quint16 Str2time(QString time)
{
    // 时间格式为："yyyy-MM-dd hh:mm:ss"
    // 取出天数
    int day = time.mid(8, 2).toInt();       // 5
    // 取出小时信息
    int hour = time.mid(11, 2).toInt();     // 5
    // 取出分钟信息
    int minute = time.mid(14, 2).toInt();   // 6
    // 初始化变量
    quint16 ret_time = 0;
    // 天数信息保存在高5位
    ret_time |= day << 11;
    // 小时信息保存在中5位
    ret_time |= hour << 6;
    // 分钟信息保存在低6位
    ret_time |= minute;
    return ret_time;
}

QString Time2str(quint16 time)
{
    QString str = "2016-12-";
    QString day = QString::number((time & 0xF800) >> 11);
    QString hour = QString::number((time & 0x07C0) >> 6);
    QString minute = QString::number(time & 0x003F);
    return str + day + QObject::tr(" ") + hour + QObject::tr(":") + minute;
}
