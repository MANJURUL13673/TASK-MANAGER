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
    Task getTask(int index) const;
    const QList<Task>& getTasks() const;
    void setTasks(const QList<Task>& taskList);
    int getActualIndex(int displayIndex) const;
    void applyFilter(const QString& filterType);

private slots:
    void onItemChanged(QListWidgetItem* item);

signals:
    void taskToggled(int index);

private:
    QList<Task>tasks;
    QList<int>filteredIndices;
    QString currentFilter = "All Tasks";

    void updateDisplay();
    void updateTaskAppearance(QListWidgetItem* item, const Task& task);
    bool matchesFilter(const Task& task, const QString& filter);
    void applyCurrentFilter();
};

#endif // TASKLISTWIDGET_H
