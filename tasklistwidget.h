#ifndef TASKLISTWIDGET_H
#define TASKLISTWIDGET_H
#include <QListWidget>
#include <QListWidgetItem>
#include "task.h"

class TaskListWidget : public QListWidget
{
    Q_OBJECT

public:
    TaskListWidget(QWidget* parent = nullptr);
    void addTask(const Task& task);
    void removeTask(int index);
    void updateTask(int index, const Task& task);
    Task getTask(int index);
    const QList<Task>& getTasks() const;
    void setTasks(const QList<Task>& taskList);

private slots:
    //void onItemChanged(QListWidgetItem* item);

};

#endif // TASKLISTWIDGET_H
