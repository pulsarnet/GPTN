//
// Created by darkp on 23.10.2022.
//

#ifndef FFI_RUST_NEWPROJECTWINDOW_H
#define FFI_RUST_NEWPROJECTWINDOW_H

#include <QDialog>
#include <QDir>

class QLineEdit;
class QPushButton;

class NewProjectWindow : public QDialog {

    Q_OBJECT

public:

    explicit NewProjectWindow(QWidget* parent = nullptr);

signals:
    void createProject(const QDir& dir, const QString& name);

protected slots:

    void onBrowserButtonClicked();
    void onCreateButtonClicked();
    void onCancelButtonClicked();
    void updateState();

private:

    QLineEdit *m_dirPath; // Select file field
    QLineEdit *m_projectName; // Project name field
    QPushButton *m_createButton; // Create button
    QPushButton *m_cancelButton; // Cancel button
};


#endif //FFI_RUST_NEWPROJECTWINDOW_H
