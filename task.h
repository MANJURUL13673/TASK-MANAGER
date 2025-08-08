#ifndef TASK_H
#define TASK_H
#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>

class Task
{
public:
    QString id;
    QString title;
    QString description;
    QDateTime dueDate;
    QString priority;
    bool completed;
    QDateTime createdDate;
    QString parentId;
    QList<QString> subtaskIds;
    int level; // Depth level for display

    Task(const QString& t = "", const QString& d = "", const QDateTime& due = QDateTime::currentDateTime(),
         const QString& p = "Medium", bool c = false, const QString& parent = "");

    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject& obj);
    bool isMainTask() const;
    bool hasSubtasks() const;
};

#endif // TASK_H
