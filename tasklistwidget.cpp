#include "tasklistwidget.h"

TaskListWidget::TaskListWidget(QWidget* parent)
    : QListWidget(parent) {}

void TaskListWidget::addTask(const Task& task)
{
    tasks.append(task);
    updateDisplay();
}

void TaskListWidget::removeTask(int index)
{
    if (index >= 0 && index < tasks.size()) {
        tasks.removeAt(index);
        updateDisplay();
    }
}

void TaskListWidget::updateTask(int index, const Task& task) {
    if (index >= 0 && index < tasks.size()) {
        tasks[index] = task;
        updateDisplay();
    }
}

Task TaskListWidget::getTask(int index) const
{
    if (index >= 0 && index < tasks.size()) {
        return tasks[index];
    }
    return Task();
}

const QList<Task>& TaskListWidget::getTasks() const { return tasks; }
void TaskListWidget::setTasks(const QList<Task>& taskList)
{
    tasks = taskList;
    updateDisplay();
}

void TaskListWidget::onItemChanged(QListWidgetItem* item) {
    int index = row(item);
    if (index >= 0 && index < tasks.size()) {
        tasks[index].completed = (item->checkState() == Qt::Checked);
        updateTaskAppearance(item, tasks[index]);
        emit taskToggled(index);
    }
}

void TaskListWidget::updateDisplay() {
    clear();
    for (const Task& task : tasks) {
        QListWidgetItem* item = new QListWidgetItem();
        updateTaskAppearance(item, task);
        item->setCheckState(task.completed ? Qt::Checked : Qt::Unchecked);
        addItem(item);
    }
    connect(this, &QListWidget::itemChanged, this, &TaskListWidget::onItemChanged);
}

void TaskListWidget::updateTaskAppearance(QListWidgetItem* item, const Task& task) {
    QString displayText = task.title;
    if (!task.dueDate.isNull()) {
        displayText += QString(" (Due: %1)").arg(task.dueDate.toString("MMM dd, yyyy"));
    }
    displayText += QString(" [%1]").arg(task.priority);

    item->setText(displayText);

    QFont font = item->font();
    if (task.completed) {
        font.setStrikeOut(true);
        item->setForeground(QColor(128, 128, 128));
    } else {
        font.setStrikeOut(false);
        if (task.priority == "High") {
            item->setForeground(QColor(255, 0, 0));
        } else if (task.priority == "Medium") {
            item->setForeground(QColor(255, 165, 0));
        } else {
            item->setForeground(QColor(0, 128, 0));
        }
    }
    item->setFont(font);
}


