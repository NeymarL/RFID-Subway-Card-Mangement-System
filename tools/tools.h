#ifndef TOOLS_H
#define TOOLS_H
#include <QString>
#include <QDateTime>
#include <QTime>
#include <QDate>
#include <QByteArray>
#include <QSqlTableModel>
#include <QCryptographicHash>

//获取当前PC的串口名
extern QStringList getSerialName();
//获取当前日期和时间字符串
extern QString CurrentDateTime();
//将原字符串进行MD5编码
extern QString GetMD5String(QString src);
//字符串转十六进制字节数组
extern quint8 StringToHex(QString string, quint8 *hex);
//QString 转十六进制数组
extern void QSTRING_TO_HEX(const QString &str, quint8 *buf, quint8 len);
//普通字符串转为16进制字符串
extern QString  CharStringtoHexString(QString space, const char * src, int start, int end);
//普通字符串转为16进制字符串
extern QString  CharStringtoHexString(QString space, const char * src, int len);
//获取当前的时间
extern QString CurrentTime();
//获取当前的时间到毫秒
extern QString CurrentMTime();
extern int CurrentMonthTime();

extern quint16 Str2time(QString time);
extern QString Time2str(quint16 time);

#endif // TOOLS_H
