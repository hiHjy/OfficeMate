#ifndef NETONLINECHECKER_H
#define NETONLINECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
/*
    用于检测网络是否畅通。
    原理：使用定时器对指定url发送一次https请求，然后根据结果判断网络是否畅通，为啥用https呢？
    应为我的考勤人脸识别也是https的所以得一致要不然可能造成误判。
*/
class NetOnlineChecker : public QObject
{
    Q_OBJECT
public:
    explicit NetOnlineChecker(QObject *parent = nullptr);

signals:
    void netStatusChanged(int ok);


private:

    //网络访问管理器
    QNetworkAccessManager * manager;
    QTimer *timer;
    QUrl url;
    void checkOnce();
    bool netStatus = false; //网络正常 status = true 不通则为false
    QNetworkReply *reply = nullptr;


};

#endif // NETONLINECHECKER_H
