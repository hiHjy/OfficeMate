#include "record.h"
#include "ui_record.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
Record::Record(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Record)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());

    model = new QSqlQueryModel(this);

    /*
        作用： 将数据模型（Model）与视图（View）绑定.
              建立 MVC 架构中的 Model-View 连接.
                  相当于：
                      View (tableView) ← Model (数据)
                      视图从模型获取数据并显示
    */
    ui->tableView->setModel(model); //注意：当使用这个查询的时候调用model->setQuery成功后，内部会自动更新数据到 ui->tableView
    getRecordByDate(QDate::currentDate());

}

Record::~Record()
{
    delete ui;
}

void Record::on_pushButton_2_clicked()
{
    ui->dateEdit->setDate(QDate::currentDate());
    ui->lineEdit->clear();
    getRecordByDate(QDate::currentDate(), QString());
}
/*
    按日期去查询考勤记录
    Note:数据库中的时间格式为：2025-11-18 23:04:30



*/
bool Record::getRecordByDate(const QDate &date, const QString &userId)
{
    /*
        QDateTime start(...)：创建一个名为 start 的 QDateTime 对象
        date：一个 QDate 对象，表示日期部分（年/月/日）
        QTime(0, 0, 0)：创建一个时间对象，表示 00:00:00（午夜）
    */
    qDebug() << "getRecordByDate() 调用";
    QDateTime start(date, QTime(0, 0, 0)); //创建一个date（某年某月某日） 00：00：00的QDateTime对象

    QDateTime end = start.addDays(1);      //第二天的00：00：00

    QString startStr = start.toString("yyyy-MM-dd HH:mm:ss");//转换成字符串，格式是2025-11-18 23:04:30这样的,一会查询的时候用
    QString endStr = end.toString("yyyy-MM-dd HH:mm:ss");

    /*---------------------------------------------------------------------------------------------------------------------------------
         R"()" 使用说明

    R"()"是 C++11 原始字符串字面量（Raw String Literal）

        用法：R"(原始字符串内容)"

        为什么这里要用？
            举例：
                不使用
                    QString json = "{\"name\": \"John\", \"age\": 30, \"city\": \"New York\"}";

                使用
                    QString json = R"({"name": "John","age": 30,"city": "New York"})";

      总结：R"()" 可以不需要难看的"\"转义字符
    */


    QString sql = R"(
                  SELECT
                  p.user_id    AS 工号,
                  p.name       AS 姓名,
                  p.dept       AS 部门,
                  v.visit_time AS 考勤时间
                  FROM VisitRecordTable v
                  LEFT JOIN PersonInfoTable p
                  ON v.user_id = p.user_id
                  WHERE v.visit_time >= :start
                  AND v.visit_time <  :end
                  )";

    if (!userId.isEmpty()) {
        //如果用户在界面选择要查询某个user_id，那么sql加上下面这条语句就可以了
        sql += " AND v.user_id = :uid";
    }
    sql += " ORDER BY v.visit_time DESC";

    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(":start", startStr);
    query.bindValue(":end", endStr);
    if (!userId.isEmpty()) {

        query.bindValue(":uid", userId);
    }

    query.exec();

    model->setQuery(query);

    if (model->lastError().isValid()) {

        qDebug() << model->lastError().text();
        return false;
    }
    if (model->rowCount() == 0) {
        //sql执行成功，但是没查询到数据，那么就不要 《列宽按内容自适应》
        return true;
    }
    ui->tableView->resizeColumnsToContents();// 列宽按内容自适应
    return true;

}

void Record::on_pushButton_clicked()
{
    const QDate date = ui->dateEdit->date();
    const QString uid = ui->lineEdit->text().trimmed();
    if (!getRecordByDate(date, uid) ) {
        qDebug() << "查询失败，数据库异常，请联系管理员";

    }

    qDebug() << "查询按钮被点击";
}
