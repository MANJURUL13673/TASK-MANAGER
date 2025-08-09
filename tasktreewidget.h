#ifndef TASKTREEWIDGET_H
#define TASKTREEWIDGET_H

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "task.h"

class TaskTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    TaskTreeWidget();
    void addTask(const Task& task);
    void removeTask(const QString& taskId);
    void updateTask(const QString& taskId, const Task& newTask);
    Task getTask(QTreeWidgetItem* item) const;
    Task getSelectedTask() const;
    QString getSelectedTaskId() const;
    void addSubtask(const QString& parentId, const Task& subtask);
    bool canAddSubtask() const;
    QList<Task> getAllTasks() const;
    void setAllTasks(const QList<Task>& tasks);
    void applyFilter(const QString& filterType);
    int getTaskProgress(const QString& taskId) const;

public slots:
    void onItemChanged(QTreeWidgetItem* item, int column);

signals:
    void taskToggled(const QString& taskId);

private:
    QMap<QString, Task> taskMap;
    QList<QString> mainTaskIds;
    QString currentFilter = "All Tasks";
    static bool isUpdating;

    bool matchesFilter(const Task& task, const QString& filter);
    void applyCurrentFilter();
    void addSubtaskItems(QTreeWidgetItem* parentItem, const QString& parentTaskId);
    QTreeWidgetItem* createTaskItem(const Task& task);
    void updateTaskAppearance(QTreeWidgetItem* item, const Task& task);
    void updateParentCompletion(const QString& taskId);
    void updateParentCompletionSafe(const QString& taskId);

};

#endif // TASKTREEWIDGET_H
