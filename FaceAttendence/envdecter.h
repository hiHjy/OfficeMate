#ifndef ENVDECT_H
#define ENVDECT_H
#include <QFile>
#include <QObject>
#include <QLocalSocket>
class EnvDecter : public QObject
{
    Q_OBJECT
public:
    explicit EnvDecter(QObject *parent = nullptr);


signals:
    void envStatusUpdated(QString temp, QString  humidity);

public slots:
void readEnvStatus();

private:
    QLocalSocket *socket;
    QByteArray  rx;



};

#endif // ENVDECT_H
