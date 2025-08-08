#include "task.h"

Task::Task(const QString& t, const QString& d, const QDateTime& due, const QString& p, bool c)
    : title(t), description(d), dueDate(due), priority(p), completed(c), createdDate(QDateTime::currentDateTime()) {}

QJsonObject Task::toJson() const {
    QJsonObject obj;
    obj["title"] = title;
    obj["description"] = description;
    obj["dueDate"] = dueDate.toString(Qt::ISODate);
    obj["priority"] = priority;
    obj["completed"] = completed;
    obj["createdDate"] = createdDate.toString(Qt::ISODate);
    return obj;
}

Task Task::fromJson(const QJsonObject& obj) {
    Task task;
    task.title = obj["title"].toString();
    task.description = obj["description"].toString();
    task.dueDate = QDateTime::fromString(obj["dueDate"].toString(), Qt::ISODate);
    task.priority = obj["priority"].toString();
    task.completed = obj["completed"].toBool();
    task.createdDate = QDateTime::fromString(obj["createdDate"].toString(), Qt::ISODate);
    return task;
}
