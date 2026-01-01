#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
class SystemMonitor : public QObject
{
    Q_OBJECT
public:
    explicit SystemMonitor(QObject *parent = nullptr);


signals:
    void updated(QString date, QString cpuTemp);

private:
    QTimer *timer;
    QString readCpuTemp();
    QString readDate();

    void updateSysStatus();
    static bool readFirstLine(const QString path, QByteArray &out);
};

#endif // SYSTEMMONITOR_H
