#include "taskmanager.h"

TaskManager::TaskManager(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    connectSignals();
    loadTasks();
}

TaskManager::~TaskManager()
{
    saveTasks();
}

void TaskManager::addTask()
{
    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a task title.");
        return;
    }

    Task task(title, descEdit->toPlainText(), dueDateEdit->dateTime(),
              priorityCombo->currentText(), false);

    taskTree->addTask(task);
    clearInputs();
    saveTasks();
}

void TaskManager::addSubtask() {
    if (!taskTree->canAddSubtask()) {
        QMessageBox::warning(this, "Warning", "Please select a parent task first.");
        return;
    }

    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a subtask title.");
        return;
    }

    QString parentId = taskTree->getSelectedTaskId();
    Task subtask(title, descEdit->toPlainText(), dueDateEdit->dateTime(),
                 priorityCombo->currentText(), false);

    taskTree->addSubtask(parentId, subtask);
    clearInputs();
    saveTasks();
}


void TaskManager::editTask()
{
    Task task = taskTree->getSelectedTask();
    if (task.id.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a task to edit.");
        return;
    }

    titleEdit->setText(task.title);
    descEdit->setPlainText(task.description);
    dueDateEdit->setDateTime(task.dueDate);
    priorityCombo->setCurrentText(task.priority);

    editButton->setEnabled(false);
    updateButton->setEnabled(true);
    currentEditId = task.id;
}

void TaskManager::updateTask()
{
    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a task title.");
        return;
    }

    if (currentEditId.isEmpty()) return;

    Task task(title, descEdit->toPlainText(), dueDateEdit->dateTime(),
              priorityCombo->currentText(), false);

    taskTree->updateTask(currentEditId, task);
    clearInputs();
    editButton->setEnabled(true);
    updateButton->setEnabled(false);
    currentEditId.clear();
    saveTasks();
}

void TaskManager::deleteTask()
{
    QString taskId = taskTree->getSelectedTaskId();
    if (taskId.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a task to delete.");
        return;
    }

    Task task = taskTree->getSelectedTask();
    QString message = "Are you sure you want to delete this task?";
    if (task.hasSubtasks()) {
        message += "\n\nThis will also delete all subtasks!";
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                              message, QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        taskTree->removeTask(taskId);
        saveTasks();
    }
}

void TaskManager::onTaskSelectionChanged()
{
    Task task = taskTree->getSelectedTask();
    bool hasSelection = !task.id.isEmpty();

    editButton->setEnabled(hasSelection);
    deleteButton->setEnabled(hasSelection);
    addSubtaskButton->setEnabled(hasSelection);

    if (hasSelection) {
        QString statusText = task.completed ? "Completed" : "Pending";
        if (task.hasSubtasks()) {
            int progress = taskTree->getTaskProgress(task.id);
            statusText += QString(" (%1% subtasks complete)").arg(progress);
        }

        taskDetailsLabel->setText(QString(
                                      "<b>Created:</b> %1<br>"
                                      "<b>Due Date:</b> %2<br>"
                                      "<b>Priority:</b> %3<br>"
                                      "<b>Status:</b> %4<br>"
                                      "<b>Type:</b> %5<br>"
                                      "<b>Subtasks:</b> %6<br><br>"
                                      "<b>Description:</b><br>%7")
                                      .arg(task.createdDate.toString("MMM dd, yyyy hh:mm"))
                                      .arg(task.dueDate.toString("MMM dd, yyyy hh:mm"))
                                      .arg(task.priority)
                                      .arg(statusText)
                                      .arg(task.isMainTask() ? "Main Task" : "Subtask")
                                      .arg(task.subtaskIds.size())
                                      .arg(task.description));
    } else {
        taskDetailsLabel->clear();
    }
}

void TaskManager::onTaskToggled(const QString& taskId)
{
    saveTasks();
}

void TaskManager::filterTasks()
{
    QString filter = filterCombo->currentText();
    taskTree->applyFilter(filter);
}

void TaskManager::setupUI()
{
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    mainSplitter = new QSplitter(Qt::Horizontal);

    setupLeftPanel();
    setupRightPanel();

    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 1);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);

    setWindowTitle("Task Manager - with Subtasks");
    setMinimumSize(900, 600);
    resize(1200, 700);
}

void TaskManager::setupLeftPanel()
{
    leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Filter
    QLabel* filterLabel = new QLabel("Filter:");
    filterCombo = new QComboBox();
    filterCombo->addItems({"All Tasks", "Pending", "Completed", "High Priority",
                           "Due Today", "Main Tasks Only"});

    // Task tree
    taskTree = new TaskTreeWidget();

    leftLayout->addWidget(filterLabel);
    leftLayout->addWidget(filterCombo);
    leftLayout->addWidget(new QLabel("Tasks:"));
    leftLayout->addWidget(taskTree);

}

void TaskManager::setupRightPanel()
{
    rightPanel = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);

    // Input group
    QGroupBox* inputGroup = new QGroupBox("Add/Edit Task");
    QVBoxLayout* inputLayout = new QVBoxLayout(inputGroup);

    titleEdit = new QLineEdit();
    titleEdit->setPlaceholderText("Task title...");

    descEdit = new QTextEdit();
    descEdit->setPlaceholderText("Task description...");
    descEdit->setMaximumHeight(100);

    dueDateEdit = new QDateTimeEdit(QDateTime::currentDateTime().addDays(1));
    dueDateEdit->setCalendarPopup(true);

    priorityCombo = new QComboBox();
    priorityCombo->addItems({"Low", "Medium", "High"});
    priorityCombo->setCurrentText("Medium");

    inputLayout->addWidget(new QLabel("Title:"));
    inputLayout->addWidget(titleEdit);
    inputLayout->addWidget(new QLabel("Description:"));
    inputLayout->addWidget(descEdit);
    inputLayout->addWidget(new QLabel("Due Date:"));
    inputLayout->addWidget(dueDateEdit);
    inputLayout->addWidget(new QLabel("Priority:"));
    inputLayout->addWidget(priorityCombo);

    // Buttons
    QHBoxLayout* buttonLayout1 = new QHBoxLayout();
    QHBoxLayout* buttonLayout2 = new QHBoxLayout();

    addButton = new QPushButton("Add Main Task");
    addSubtaskButton = new QPushButton("Add Subtask");
    editButton = new QPushButton("Edit Task");
    updateButton = new QPushButton("Update Task");
    deleteButton = new QPushButton("Delete Task");

    editButton->setEnabled(false);
    updateButton->setEnabled(false);
    deleteButton->setEnabled(false);
    addSubtaskButton->setEnabled(false);

    buttonLayout1->addWidget(addButton);
    buttonLayout1->addWidget(addSubtaskButton);
    buttonLayout2->addWidget(editButton);
    buttonLayout2->addWidget(updateButton);
    buttonLayout2->addWidget(deleteButton);

    // Task details
    QGroupBox* detailsGroup = new QGroupBox("Task Details");
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);

    taskDetailsLabel = new QLabel();
    taskDetailsLabel->setWordWrap(true);
    taskDetailsLabel->setAlignment(Qt::AlignTop);
    taskDetailsLabel->setStyleSheet("QLabel { padding: 10px; background-color: #f0f0f0; border-radius: 5px; }");

    detailsLayout->addWidget(taskDetailsLabel);

    rightLayout->addWidget(inputGroup);
    rightLayout->addLayout(buttonLayout1);
    rightLayout->addLayout(buttonLayout2);
    rightLayout->addWidget(detailsGroup);
    rightLayout->addStretch();
}

void TaskManager::connectSignals() {
    connect(addButton, &QPushButton::clicked, this, &TaskManager::addTask);
    connect(addSubtaskButton, &QPushButton::clicked, this, &TaskManager::addSubtask);
    connect(editButton, &QPushButton::clicked, this, &TaskManager::editTask);
    connect(updateButton, &QPushButton::clicked, this, &TaskManager::updateTask);
    connect(deleteButton, &QPushButton::clicked, this, &TaskManager::deleteTask);
    connect(taskTree, &QTreeWidget::currentItemChanged, this, &TaskManager::onTaskSelectionChanged);
    connect(taskTree, &TaskTreeWidget::taskToggled, this, &TaskManager::onTaskToggled);
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TaskManager::filterTasks);
}

void TaskManager::clearInputs() {
    titleEdit->clear();
    descEdit->clear();
    dueDateEdit->setDateTime(QDateTime::currentDateTime().addDays(1));
    priorityCombo->setCurrentText("Medium");
}

void TaskManager::saveTasks() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    QString filePath = dataDir + "/tasks_with_subtasks.json";

    QJsonArray jsonArray;
    for (const Task& task : taskTree->getAllTasks()) {
        jsonArray.append(task.toJson());
    }

    QJsonDocument doc(jsonArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}

void TaskManager::loadTasks() {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = dataDir + "/tasks_with_subtasks.json";

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray jsonArray = doc.array();

    QList<Task> tasks;
    for (const QJsonValue& value : jsonArray) {
        tasks.append(Task::fromJson(value.toObject()));
    }

    taskTree->setAllTasks(tasks);
}

