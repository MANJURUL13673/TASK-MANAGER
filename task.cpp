#include "task.h"
#include <QUuid>

Task::Task(const QString& t, const QString& d, const QDateTime& due, const QString& p, bool c, const QString& parent)
    : title(t), description(d), dueDate(due), priority(p), completed(c),
    createdDate(QDateTime::currentDateTime()), parentId(parent), level(0) {
    id = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

QJsonObject Task::toJson() const {
    QJsonObject obj;
    obj["id"] = id;
    obj["title"] = title;
    obj["description"] = description;
    obj["dueDate"] = dueDate.toString(Qt::ISODate);
    obj["priority"] = priority;
    obj["completed"] = completed;
    obj["createdDate"] = createdDate.toString(Qt::ISODate);
    obj["parentId"] = parentId;
    obj["level"] = level;

    QJsonArray subtaskArray;
    for (const QString& subtaskId : subtaskIds) {
        subtaskArray.append(subtaskId);
    }
    obj["subtaskIds"] = subtaskArray;

    return obj;
}

Task Task::fromJson(const QJsonObject& obj) {
    Task task;
    task.id = obj["id"].toString();
    task.title = obj["title"].toString();
    task.description = obj["description"].toString();
    task.dueDate = QDateTime::fromString(obj["dueDate"].toString(), Qt::ISODate);
    task.priority = obj["priority"].toString();
    task.completed = obj["completed"].toBool();
    task.createdDate = QDateTime::fromString(obj["createdDate"].toString(), Qt::ISODate);
    task.parentId = obj["parentId"].toString();
    task.level = obj["level"].toInt();

    QJsonArray subtaskArray = obj["subtaskIds"].toArray();
    for (const QJsonValue& value : subtaskArray) {
        task.subtaskIds.append(value.toString());
    }

    return task;
}

bool Task::isMainTask() const { return parentId.isEmpty(); }
bool Task::hasSubtasks() const { return !subtaskIds.isEmpty(); }
