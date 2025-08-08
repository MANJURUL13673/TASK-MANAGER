#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QMainWindow>
#include <QtWidgets>
#include <QSplitter>

#include "tasktreewidget.h"

class TaskManager : public QMainWindow
{
    Q_OBJECT

public:
    TaskManager(QWidget *parent = nullptr);
    ~TaskManager();

private slots:
    void addTask();
    void addSubtask();
    void editTask();
    void updateTask();
    void deleteTask();
    void onTaskSelectionChanged();
    void onTaskToggled(const QString& taskId);
    void filterTasks();


private:
    QWidget* centralWidget;
    QSplitter* mainSplitter;

    // Left panel - Task tree
    QWidget* leftPanel;
    TaskTreeWidget* taskTree;
    QComboBox* filterCombo;

    // Right panel - Task input and details
    QWidget* rightPanel;
    QLineEdit* titleEdit;
    QTextEdit* descEdit;
    QDateTimeEdit* dueDateEdit;
    QComboBox* priorityCombo;
    QPushButton* addButton;
    QPushButton* addSubtaskButton;
    QPushButton* editButton;
    QPushButton* updateButton;
    QPushButton* deleteButton;
    QLabel* taskDetailsLabel;

    QString currentEditId;

    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
    void connectSignals();
    void clearInputs();
    void saveTasks();
    void loadTasks();
};
#endif // TASKMANAGER_H
