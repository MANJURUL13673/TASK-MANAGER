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

    taskList->addTask(task);
    clearInputs();
    saveTasks();
}

void TaskManager::editTask()
{
    int index = taskList->currentRow();
    if (index < 0) {
        QMessageBox::warning(this, "Warning", "Please select a task to edit.");
        return;
    }

    Task task = taskList->getTask(index);
    titleEdit->setText(task.title);
    descEdit->setPlainText(task.description);
    dueDateEdit->setDateTime(task.dueDate);
    priorityCombo->setCurrentText(task.priority);

    editButton->setEnabled(false);
    updateButton->setEnabled(true);
    currentEditIndex = index;
}

void TaskManager::updateTask()
{
    QString title = titleEdit->text().trimmed();
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a task title.");
        return;
    }

    Task task(title, descEdit->toPlainText(), dueDateEdit->dateTime(),
              priorityCombo->currentText(), taskList->getTask(currentEditIndex).completed);
    task.createdDate = taskList->getTask(currentEditIndex).createdDate;

    taskList->updateTask(currentEditIndex, task);
    clearInputs();
    editButton->setEnabled(true);
    updateButton->setEnabled(false);
    currentEditIndex = -1;
    saveTasks();
}

void TaskManager::deleteTask()
{
    int index = taskList->currentRow();
    if (index < 0) {
        QMessageBox::warning(this, "Warning", "Please select a task to delete.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                              "Are you sure you want to delete this task?", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        taskList->removeTask(index);
        saveTasks();
    }
}

void TaskManager::onTaskSelectionChanged()
{
    int index = taskList->currentRow();
    editButton->setEnabled(index >= 0);
    deleteButton->setEnabled(index >= 0);

    if (index >= 0) {
        Task task = taskList->getTask(index);
        taskDetailsLabel->setText(QString(
                                      "<b>Created:</b> %1<br>"
                                      "<b>Due Date:</b> %2<br>"
                                      "<b>Priority:</b> %3<br>"
                                      "<b>Status:</b> %4<br><br>"
                                      "<b>Description:</b><br>%5")
                                      .arg(task.createdDate.toString("MMM dd, yyyy hh:mm"))
                                      .arg(task.dueDate.toString("MMM dd, yyyy hh:mm"))
                                      .arg(task.priority)
                                      .arg(task.completed ? "Completed" : "Pending")
                                      .arg(task.description));
    } else {
        taskDetailsLabel->clear();
    }
}

void TaskManager::onTaskToggled(int index)
{
    saveTasks();
}

void TaskManager::filterTasks()
{
    QString filter = filterCombo->currentText();
    // Implementation for filtering would go here
    // For now, this is a placeholder
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

    setWindowTitle("Task Manager");
    setMinimumSize(800, 600);
    resize(1000, 700);
}

void TaskManager::setupLeftPanel()
{
    leftPanel = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);

    // Filter
    QLabel* filterLabel = new QLabel("Filter:");
    filterCombo = new QComboBox();
    filterCombo->addItems({"All Tasks", "Pending", "Completed", "High Priority", "Due Today"});

    // Task list
    taskList = new TaskListWidget();

    leftLayout->addWidget(filterLabel);
    leftLayout->addWidget(filterCombo);
    leftLayout->addWidget(new QLabel("Tasks:"));
    leftLayout->addWidget(taskList);

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
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    addButton = new QPushButton("Add Task");
    editButton = new QPushButton("Edit Task");
    updateButton = new QPushButton("Update Task");
    deleteButton = new QPushButton("Delete Task");

    editButton->setEnabled(false);
    updateButton->setEnabled(false);
    deleteButton->setEnabled(false);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);

    // Task details
    QGroupBox* detailsGroup = new QGroupBox("Task Details");
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);

    taskDetailsLabel = new QLabel();
    taskDetailsLabel->setWordWrap(true);
    taskDetailsLabel->setAlignment(Qt::AlignTop);
    taskDetailsLabel->setStyleSheet("QLabel { padding: 10px; background-color: #f0f0f0; border-radius: 5px; }");

    detailsLayout->addWidget(taskDetailsLabel);

    rightLayout->addWidget(inputGroup);
    rightLayout->addLayout(buttonLayout);
    rightLayout->addWidget(detailsGroup);
    rightLayout->addStretch();
}

void TaskManager::connectSignals() {
    connect(addButton, &QPushButton::clicked, this, &TaskManager::addTask);
    connect(editButton, &QPushButton::clicked, this, &TaskManager::editTask);
    connect(updateButton, &QPushButton::clicked, this, &TaskManager::updateTask);
    connect(deleteButton, &QPushButton::clicked, this, &TaskManager::deleteTask);
    connect(taskList, &QListWidget::currentRowChanged, this, &TaskManager::onTaskSelectionChanged);
    connect(taskList, &TaskListWidget::taskToggled, this, &TaskManager::onTaskToggled);
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
    QString filePath = dataDir + "/tasks.json";

    QJsonArray jsonArray;
    for (const Task& task : taskList->getTasks()) {
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
    qDebug() << dataDir;
    QString filePath = dataDir + "/tasks.json";

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

    taskList->setTasks(tasks);
}

