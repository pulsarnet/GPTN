//
// Created by Николай Муравьев on 11.12.2021.
//

#ifndef FFI_RUST_MAINWINDOW_H
#define FFI_RUST_MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QListView>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QWheelEvent>
#include <QActionGroup>
#include <QTabWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QMessageBox>

class ActionTabWidget;


class MainWindow : public QMainWindow {

    enum SaveFileAnswer {
        Save = 0,
        NoSave,
        Cancel
    };

public:
    MainWindow() = delete;
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    explicit MainWindow(QWidget *parent = nullptr);

    void setFileName(const QString& name);
    bool saveAs();
    bool save();
    bool open();

    bool saveFile(const QString& filename);
    QMessageBox::StandardButton onSaveFileAsk();

public slots:

    void slotSaveFile(bool checked);

    void slotSaveAsFile(bool checked);

    void slotOpenFile(bool checked);

    void onDocumentChanged();
protected:

    void closeEvent(QCloseEvent* event) override;

    bool saveOnExit();

private:

    void createMenuBar();

    void createStatusBar();

private:



    QStatusBar* statusBar = nullptr;
    QMenuBar* menuBar = nullptr;
    ActionTabWidget* m_tabWidget = nullptr;

    QString m_filename;
    bool m_changed;
};

#endif //FFI_RUST_MAINWINDOW_H