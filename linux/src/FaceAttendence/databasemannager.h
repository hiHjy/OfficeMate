#ifndef DATABASEMANNAGER_H
#define DATABASEMANNAGER_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

struct UserInfo {
    QString name;
    QString workId;
    QString identity;
    QString imgPath;
    bool valid = false;   // 用来表示是否查到
};
class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseManager(QObject *parent = nullptr);
    static DatabaseManager *getInstance();
    bool initDataBase();
    bool insertPersonInfo(QString faceId,
                          QString identity,
                          QString workId,
                          QString name,
                          QString faceFile);

    UserInfo getInfoByUID(const QString& faceToken);


private:
    static DatabaseManager *self;
    QSqlDatabase db;

};

#endif // DATABASEMANNAGER_H
