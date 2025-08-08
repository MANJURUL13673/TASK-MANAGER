#ifndef TASK_H
#define TASK_H
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

class Task
{
public:
    QString title;
    QString description;
    QDateTime dueDate;
    QString priority;
    bool completed;
    QDateTime createdDate;

    Task(const QString& t = "", const QString& d = "", const QDateTime& due = QDateTime::currentDateTime(),
         const QString& p = "Medium", bool c = false);

    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject& obj);
};

#endif // TASK_H
