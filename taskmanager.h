#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QMainWindow>
#include <QtWidgets>
#include <QSplitter>

#include "tasklistwidget.h"

class TaskManager : public QMainWindow
{
    Q_OBJECT

public:
    TaskManager(QWidget *parent = nullptr);
    ~TaskManager();

private:
    QWidget* centralWidget;
    QSplitter* mainSplitter;

    // Left panel - Task list
    QWidget* leftPanel;
    TaskListWidget* taskList;
    QComboBox* filterCombo;

    // Right panel - Task input and details
    QWidget* rightPanel;
    QLineEdit* titleEdit;
    QTextEdit* descEdit;
    QDateTimeEdit* dueDateEdit;
    QComboBox* priorityCombo;
    QPushButton* addButton;
    QPushButton* editButton;
    QPushButton* updateButton;
    QPushButton* deleteButton;
    QLabel* taskDetailsLabel;

    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
};
#endif // TASKMANAGER_H
