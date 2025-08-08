#include "tasktreewidget.h"
#include <QHeaderView>

TaskTreeWidget::TaskTreeWidget()
{
    setHeaderLabels({"Task", "Due Date", "Priority", "Status"});
    header()->setStretchLastSection(false);
    header()->setSectionResizeMode(0, QHeaderView::Stretch);
    header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    setRootIsDecorated(true);
    setIndentation(20);
}

void TaskTreeWidget::addTask(const Task& task)
{
    taskMap[task.id] = task;

    if (task.isMainTask()) {
        mainTaskIds.append(task.id);
    } else {
        // Add to parent's subtask list
        if (taskMap.contains(task.parentId)) {
            if (!taskMap[task.parentId].subtaskIds.contains(task.id)) {
                taskMap[task.parentId].subtaskIds.append(task.id);
            }
        }
    }
    applyCurrentFilter();
}

void TaskTreeWidget::removeTask(const QString& taskId)
{
    if (!taskMap.contains(taskId)) return;

    Task task = taskMap[taskId];

    // Remove all subtasks recursively
    QList<QString> toRemove = {taskId};
    for (int i = 0; i < toRemove.size(); ++i)
    {
        QString currentId = toRemove[i];
        if (taskMap.contains(currentId)) {
            toRemove.append(taskMap[currentId].subtaskIds);
        }
    }

    for (const QString& id : toRemove)
    {
        if (taskMap.contains(id)) {
            Task t = taskMap[id];
            // Remove from parent's subtask list
            if (!t.parentId.isEmpty() && taskMap.contains(t.parentId)) {
                taskMap[t.parentId].subtaskIds.removeOne(id);
            }
            // Remove from main task list if it's a main task
            mainTaskIds.removeOne(id);
            taskMap.remove(id);
        }
    }

    applyCurrentFilter();
}

void TaskTreeWidget::updateTask(const QString& taskId, const Task& newTask)
{
    if (taskMap.contains(taskId)) {
        Task updatedTask = newTask;
        updatedTask.id = taskId;
        updatedTask.subtaskIds = taskMap[taskId].subtaskIds; // Preserve subtasks
        updatedTask.parentId = taskMap[taskId].parentId; // Preserve parent
        taskMap[taskId] = updatedTask;
        applyCurrentFilter();
    }
}

Task TaskTreeWidget::getTask(QTreeWidgetItem* item) const
{
    if (!item) return Task();
    QString taskId = item->data(0, Qt::UserRole).toString();
    return taskMap.value(taskId, Task());
}

Task TaskTreeWidget::getSelectedTask() const
{
    QTreeWidgetItem* item = currentItem();
    return getTask(item);
}

QString TaskTreeWidget::getSelectedTaskId() const
{
    QTreeWidgetItem* item = currentItem();
    if (!item) return QString();
    return item->data(0, Qt::UserRole).toString();
}

void TaskTreeWidget::addSubtask(const QString& parentId, const Task& subtask)
{
    if (!taskMap.contains(parentId)) return;

    Task newSubtask = subtask;
    newSubtask.parentId = parentId;
    newSubtask.level = taskMap[parentId].level + 1;
    addTask(newSubtask);
}

bool TaskTreeWidget::canAddSubtask() const
{
    return currentItem() != nullptr;
}

QList<Task> TaskTreeWidget::getAllTasks() const
{
    return taskMap.values();
}

void TaskTreeWidget::setAllTasks(const QList<Task>& tasks)
{
    taskMap.clear();
    mainTaskIds.clear();

    // First pass: add all tasks to map
    for (const Task& task : tasks) {
        taskMap[task.id] = task;
        if (task.isMainTask()) {
            mainTaskIds.append(task.id);
        }
    }

    // Second pass: rebuild parent-child relationships
    for (const Task& task : tasks) {
        if (!task.parentId.isEmpty() && taskMap.contains(task.parentId)) {
            if (!taskMap[task.parentId].subtaskIds.contains(task.id)) {
                taskMap[task.parentId].subtaskIds.append(task.id);
            }
        }
    }

    applyCurrentFilter();
}

void TaskTreeWidget::applyFilter(const QString& filterType)
{
    currentFilter = filterType;
    applyCurrentFilter();
}

int TaskTreeWidget::getTaskProgress(const QString& taskId) const
{
    if (!taskMap.contains(taskId)) return 0;

    const Task& task = taskMap[taskId];
    if (!task.hasSubtasks()) {
        return task.completed ? 100 : 0;
    }

    int completed = 0;
    int total = task.subtaskIds.size();

    for (const QString& subtaskId : task.subtaskIds) {
        if (taskMap.contains(subtaskId) && taskMap[subtaskId].completed) {
            completed++;
        }
    }

    return total > 0 ? (completed * 100) / total : 0;
}

void TaskTreeWidget::onItemChanged(QTreeWidgetItem* item, int column)
{
    if (column == 3) { // Status column
        QString taskId = item->data(0, Qt::UserRole).toString();
        if (taskMap.contains(taskId)) {
            bool completed = item->checkState(3) == Qt::Checked;

            taskMap[taskId].completed = completed;
            updateTaskAppearance(item, taskMap[taskId]);

            // Update parent completion status
            updateParentCompletion(taskId);

            emit taskToggled(taskId);
        }
    }
}

bool TaskTreeWidget::matchesFilter(const Task& task, const QString& filter)
{
    if (filter == "All Tasks") {
        return true;
    } else if (filter == "Pending") {
        return !task.completed;
    } else if (filter == "Completed") {
        return task.completed;
    } else if (filter == "High Priority") {
        return task.priority == "High";
    } else if (filter == "Due Today") {
        QDate today = QDate::currentDate();
        return task.dueDate.date() == today;
    } else if (filter == "Main Tasks Only") {
        return task.isMainTask();
    }
    return true;
}

void TaskTreeWidget::applyCurrentFilter()
{
    clear();
    disconnect(this, &QTreeWidget::itemChanged, this, &TaskTreeWidget::onItemChanged);

    for (const QString& mainTaskId : mainTaskIds) {
        if (taskMap.contains(mainTaskId)) {
            const Task& mainTask = taskMap[mainTaskId];
            if (matchesFilter(mainTask, currentFilter)) {
                QTreeWidgetItem* mainItem = createTaskItem(mainTask);
                addTopLevelItem(mainItem);
                addSubtaskItems(mainItem, mainTaskId);

                // Expand items with subtasks
                if (mainTask.hasSubtasks()) {
                    mainItem->setExpanded(true);
                }
            }
        }
    }

    connect(this, &QTreeWidget::itemChanged, this, &TaskTreeWidget::onItemChanged);
}

void TaskTreeWidget::addSubtaskItems(QTreeWidgetItem* parentItem, const QString& parentTaskId)
{
    if (!taskMap.contains(parentTaskId)) return;

    const Task& parentTask = taskMap[parentTaskId];
    for (const QString& subtaskId : parentTask.subtaskIds) {
        if (taskMap.contains(subtaskId)) {
            const Task& subtask = taskMap[subtaskId];
            if (matchesFilter(subtask, currentFilter)) {
                QTreeWidgetItem* subtaskItem = createTaskItem(subtask);
                parentItem->addChild(subtaskItem);

                // Recursively add sub-subtasks
                if (subtask.hasSubtasks()) {
                    addSubtaskItems(subtaskItem, subtaskId);
                    subtaskItem->setExpanded(true);
                }
            }
        }
    }
}

QTreeWidgetItem* TaskTreeWidget::createTaskItem(const Task& task)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setData(0, Qt::UserRole, task.id);
    updateTaskAppearance(item, task);
    return item;
}

void TaskTreeWidget::updateTaskAppearance(QTreeWidgetItem* item, const Task& task)
{
    // Task name with progress indicator
    QString taskText = task.title;
    if (task.hasSubtasks()) {
        int progress = getTaskProgress(task.id);
        taskText += QString(" (%1%)").arg(progress);
    }
    item->setText(0, taskText);

    // Due date
    item->setText(1, task.dueDate.toString("MMM dd, yyyy"));

    // Priority
    item->setText(2, task.priority);

    // Status checkbox
    item->setCheckState(3, task.completed ? Qt::Checked : Qt::Unchecked);

    // Visual styling
    QFont font = item->font(0);
    if (task.completed) {
        font.setStrikeOut(true);
        item->setForeground(0, QColor(128, 128, 128));
    } else {
        font.setStrikeOut(false);
        if (task.priority == "High") {
            item->setForeground(0, QColor(255, 0, 0));
        } else if (task.priority == "Medium") {
            item->setForeground(0, QColor(255, 165, 0));
        } else {
            item->setForeground(0, QColor(0, 128, 0));
        }
    }
    item->setFont(0, font);

    // Add icon for tasks with subtasks
    if (task.hasSubtasks()) {
        item->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
    } else {
        item->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
    }
}

void TaskTreeWidget::updateParentCompletion(const QString& taskId)
{
    if (!taskMap.contains(taskId)) return;

    const Task& task = taskMap[taskId];
    if (task.parentId.isEmpty() || !taskMap.contains(task.parentId)) return;

    // Check if all siblings are completed
    const Task& parent = taskMap[task.parentId];
    bool allCompleted = true;
    for (const QString& siblingId : parent.subtaskIds) {
        if (taskMap.contains(siblingId) && !taskMap[siblingId].completed) {
            allCompleted = false;
            break;
        }
    }

    // Update parent completion status
    if (taskMap[task.parentId].completed != allCompleted) {
        taskMap[task.parentId].completed = allCompleted;
        applyCurrentFilter(); // Refresh display
        updateParentCompletion(task.parentId); // Recursive update
    }
}
