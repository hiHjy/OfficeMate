#include "netonlinechecker.h"
#include <QSsl>
#include <QNetworkReply>
#define TIMER_INTERVAL 5000
#define TIMEOUT 3000

#define URL "https://www.apple.com/library/test/success.html"
NetOnlineChecker::NetOnlineChecker(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    url = QUrl(URL);
    checkOnce(); //开局直接先检测一次网络
    timer = new QTimer(this);
    timer->setInterval(TIMER_INTERVAL); //TIMER_INTERVAL = 5000; 5s
    timer->start();
    connect(timer, &QTimer::timeout, this, &NetOnlineChecker::checkOnce); //5s检查一次
}

void NetOnlineChecker::checkOnce()
{


    QNetworkRequest request(url); //https请求

    /*这段代码是禁用 SSL 证书验证的配置，主要用于开发板或测试环境中绕过 HTTPS 的证书验证问题*/
        // 1. 获取默认的SSL配置
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();

        // 2. 设置使用任何SSL协议版本（TLS 1.0/1.1/1.2/1.3等）
    config.setProtocol(QSsl::AnyProtocol);

        // 3. 禁用对端（服务器）证书验证 ⚠️重要
    config.setPeerVerifyMode(QSslSocket::VerifyNone);

        // 4. 将配置应用到请求
    request.setSslConfiguration(config);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtNetProbe/1.0");

    if (reply) return; //如果reply还没有被释放那么说明已经发送请求了，不过正在等待，此时不要发送新的请求了
    reply = manager->head(request); //立即发送了 HTTP HEAD 请求

    QTimer *timeoutTimer = new QTimer(reply);//让这个定时器作为reply的子对象，当reply被析构时，定时器被自动删除，安全！
    timeoutTimer->setSingleShot(true); //这个定时器只被触发一次
    timeoutTimer->setInterval(TIMEOUT);
    timeoutTimer->start();

    /* 注意:connect 的第三个参数是上下文对象，当这个对象被删除后，这个信号和槽就失效了 */
    QNetworkReply *r = reply;
    connect(timeoutTimer, &QTimer::timeout, reply, [r](){
        if (r && r->isRunning()) //如果超时就干掉这个请求，
             r->abort();    //立即终止请求, 会触发它自己的errorOccurred和finished()信号，虽然finished但错误 ☺

    });


//    QTimer::singleShot(TIMEOUT, [reply]() {
//        if (reply && reply->isRunning()) //如果超时就干掉这个请求，
//             reply->abort();    //立即终止请求, 会触发它自己的errorOccurred和finished()信号，虽然finished但错误 ☺
//    });

    connect(reply, &QNetworkReply::finished, this, [this]() {
            bool ok = false;
            if (reply->error() == QNetworkReply::NoError) {
                int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                ok = (code >= 200 && code < 300); //2xx /3xx/4xx 都算网络可达 /2xx 表示能上网。这里是这个类判断的最根本的地方，如果到时候有问题，可能是这里有问题

            }

            reply->deleteLater();
            reply = nullptr;
            if (ok != netStatus) {
                netStatus = ok;
                emit netStatusChanged(ok); //如果状态改变了通知ui更新状态
            }



    });





}
