//
// Created by darkp on 23.10.2022.
//

#include "NewProjectWindow.h"
#include <QFileDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <Qlabel>
#include <QSettings>
#include <QStandardPaths>

NewProjectWindow::NewProjectWindow(QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMinimumSize(400, 100);
    setModal(true);

    auto layout = new QGridLayout(this);
    setLayout(layout);

    // Dir layout
    auto dirLayout = new QHBoxLayout();
    layout->addLayout(dirLayout, 0, 0, 1, 2);

    auto dirLabel = new QLabel("Directory:", this);
    dirLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_dirPath = new QLineEdit(this);
    m_dirPath->setReadOnly(true);
    m_dirPath->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto browseAction = m_dirPath->addAction(
            QIcon(":/images/icons/folder-open.svg"),
            QLineEdit::TrailingPosition);

    // From QSettings load FS/LastOpenedPath
    QSettings settings;
    QString path = settings.value("FS/LastOpenedPath", QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    m_dirPath->setText(path);

    dirLayout->addWidget(dirLabel);
    dirLayout->addWidget(m_dirPath);

    // Name layout
    auto nameLayout = new QHBoxLayout();
    layout->addLayout(nameLayout, 1, 0, 1, 2);

    auto nameLabel = new QLabel("Project name:", this);
    nameLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_projectName = new QLineEdit(this);
    m_projectName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_projectName);

    // Spacer
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 2, 0, 1, 2);

    // Buttons
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignRight);
    layout->addLayout(buttonLayout, 3, 0, 1, 2);

    m_createButton = new QPushButton("Create", this);
    m_cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(m_createButton);
    buttonLayout->addWidget(m_cancelButton);

    // Connect
    connect(browseAction,
            &QAction::triggered,
            this,
            &NewProjectWindow::onBrowserButtonClicked);

    connect(m_createButton,
            &QPushButton::clicked,
            this,
            &NewProjectWindow::onCreateButtonClicked);

    connect(m_cancelButton,
            &QPushButton::clicked,
            this,
            &NewProjectWindow::onCancelButtonClicked);

    connect(m_projectName,
            &QLineEdit::textChanged,
            this,
            &NewProjectWindow::updateState);


    updateState();
}

void NewProjectWindow::onBrowserButtonClicked() {
    auto lastPath = m_dirPath->text();
    if (lastPath.isEmpty()) {
        lastPath = QDir::currentPath();
    }

    auto path = QFileDialog::getExistingDirectory(this,
                                                  "Select directory",
                                                  lastPath);
    if (path.isEmpty()) {
        return;
    }

    m_dirPath->setText(path);

    QSettings settings;
    settings.setValue("FS/LastOpenedPath", path);

    updateState();
}

void NewProjectWindow::onCreateButtonClicked() {
    emit createProject(QDir(m_dirPath->text()), m_projectName->text());
    close();
}

void NewProjectWindow::onCancelButtonClicked() {
    close();
}

void NewProjectWindow::updateState() {
    // Check if dir + project name.json is not exist
    auto dirName = m_dirPath->text();
    auto projectName = m_projectName->text();

    if (dirName.isEmpty() || projectName.isEmpty()) {
        m_createButton->setEnabled(false);
        return;
    }

    QString path = dirName + "/" + projectName.trimmed() + ".json";

    if (QFile::exists(path)) {
        m_createButton->setEnabled(false);
    } else {
        m_createButton->setEnabled(true);
    }
}
