#include "taskmanager.h"

TaskManager::TaskManager(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

TaskManager::~TaskManager() {}

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
