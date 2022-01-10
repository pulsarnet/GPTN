//
// Created by Николай Муравьев on 11.12.2021.
//

#include "../include/mainwindow.h"


#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QDir>
#include <fmt/format.h>
#include "../include/matrix_model.h"
#include "../include/synthesis/synthesis_program_item_delegate.h"
#include <QTableView>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    createToolBar();
    createMenuBar();
    createStatusBar();
    configureTab();

    connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    this->setCentralWidget(this->tabWidget);

    this->splitItems = new QListView;
    connect(this->splitItems, &QListView::clicked, this, &MainWindow::slotSplitChecked);

    QDockWidget* dock = new QDockWidget(this);
    dock->setWidget(this->splitItems);

    this->addDockWidget(Qt::RightDockWidgetArea, dock);
    tabChanged(-1);
}

void MainWindow::closeTab(int index) {
    auto tab = dynamic_cast<Tab*>(this->tabWidget->widget(index));
    if (tab->changed()) {
        auto answer = MainWindow::askSaveFile();
        if (answer == MainWindow::Cancel) return;

        if (answer == MainWindow::Save) {
            auto saveFile = chooseSaveFile();
            if (saveFile.isEmpty()) return;

            // Save to file
            if (tab->setFile(saveFile)) {
                tab->scene()->saveToFile(tab->file());
                tab->setChanged(false);
            }
            else {
                // TODO: Error Message
                return;
            }
        }
    }

    auto tab_object = dynamic_cast<Tab*>(this->tabWidget->widget(index));
    this->tabWidget->removeTab(index);
    tab_object->closeFile();
}

void MainWindow::positionChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Position : GraphicsView::A_Nothing);
    }
}

void MainWindow::transitionChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Transition : GraphicsView::A_Nothing);
    }
}

void MainWindow::moveChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Move : GraphicsView::A_Nothing);
    }
}

void MainWindow::connectChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Connect : GraphicsView::A_Nothing);
    }
}

void MainWindow::rotateChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Rotate : GraphicsView::A_Nothing);
    }
}

void MainWindow::removeChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Remove : GraphicsView::A_Nothing);
    }
}

void MainWindow::markerChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Marker : GraphicsView::A_Nothing);
    }
}


void MainWindow::configureTab() {

    this->tabWidget = new QTabWidget;
    this->tabWidget->setDocumentMode(false);
    this->tabWidget->setTabsClosable(true);

}

Tab* MainWindow::newTab() {
    auto tab = new Tab(this);
    auto index = tabWidget->addTab(tab, "New Tab");
    this->tabWidget->setCurrentIndex(index);
    return tab;
}

QAction *MainWindow::makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup) {
    auto action = new QAction(name);
    action->setIcon(icon);
    action->setCheckable(checkable);

    if (actionGroup) actionGroup->addAction(action);

    return action;
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar;

    auto file_menu = new QMenu("&File");
    {
        auto newMenu = new QMenu("New");

        auto new_action = new QAction("File");
        new_action->setShortcut(QKeySequence::fromString(tr("Ctrl+N")));
        newMenu->addAction(new_action);
        connect(new_action, &QAction::triggered, this, &MainWindow::newFile);

        file_menu->addMenu(newMenu);
    }

    auto save_action = new QAction("Save");
    save_action->setShortcut(tr("Ctrl+S"));
    connect(save_action, &QAction::triggered, this, &MainWindow::slotSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(tr("Ctrl+Shift+S"));
    connect(save_as_action, &QAction::triggered, this, &MainWindow::slotSaveAsFile);

    auto open_action = new QAction("Open");
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, &QAction::triggered, this, &MainWindow::slotOpenFile);

    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);

    menuBar->addMenu(file_menu);

    {
        auto action_menu = new QMenu("Actions");

        auto split = new QAction("Split");
        split->setShortcut(tr("F9"));
        connect(split, &QAction::triggered, this, &MainWindow::slotSplitAction);

        action_menu->addAction(split);

        menuBar->addMenu(action_menu);
    }

    this->setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::createToolBar() {
    toolBar = new QToolBar;
    this->addToolBar(Qt::LeftToolBarArea, toolBar);

    actionGroup = new QActionGroup(toolBar);

    position_action = makeAction("Position", QIcon(":/images/circle.png"), true, actionGroup);
    transition_action = makeAction("Transition", QIcon(":/images/rectangle.png"), true, actionGroup);
    move_action = makeAction("Move", QIcon(":/images/move.png"), true, actionGroup);
    connect_action = makeAction("Connect", QIcon(":/images/connect.png"), true, actionGroup);
    rotation_action = makeAction("Rotate", QIcon(":/images/rotation.png"), true, actionGroup);
    remove_action = makeAction("Remove", QIcon(":/images/remove.png"), true, actionGroup);
    marker_action = makeAction("Marker", QIcon(":/images/marker.png"), true, actionGroup);


    connect(position_action, &QAction::toggled, this, &MainWindow::positionChecked);
    connect(transition_action, &QAction::toggled, this, &MainWindow::transitionChecked);
    connect(move_action, &QAction::toggled, this, &MainWindow::moveChecked);
    connect(connect_action, &QAction::toggled, this, &MainWindow::connectChecked);
    connect(rotation_action, &QAction::toggled, this, &MainWindow::rotateChecked);
    connect(remove_action, &QAction::toggled, this, &MainWindow::removeChecked);
    connect(marker_action, &QAction::toggled, this, &MainWindow::markerChecked);


    toolBar->addAction(position_action);
    toolBar->addAction(transition_action);
    toolBar->addAction(move_action);
    toolBar->addAction(connect_action);
    toolBar->addAction(rotation_action);
    toolBar->addAction(remove_action);
    toolBar->addAction(marker_action);
}

GraphicsView *MainWindow::currentScene() {
    if (auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex())); tab) {
        return tab->scene();
    }

    return nullptr;
}

void MainWindow::newFile(bool trigger) {
    Q_UNUSED(trigger);
    this->newTab();
}

void MainWindow::tabChanged(int index) {
    auto checked = this->actionGroup->checkedAction();

    if (index == -1) {
        if (checked) checked->toggle();
        this->toolBar->setEnabled(false);
        this->splitItems->setModel(nullptr);
        return;
    }

    this->toolBar->setEnabled(true);

    auto currentAction = currentScene()->currentAction();

    if (checked) checked->toggle();

    switch (currentAction) {
        case GraphicsView::A_Position:
            position_action->toggle();
            break;
        case GraphicsView::A_Transition:
            transition_action->toggle();
            break;
        case GraphicsView::A_Connect:
            connect_action->toggle();
            break;
        case GraphicsView::A_Move:
            move_action->toggle();
            break;
        case GraphicsView::A_Rotate:
            rotation_action->toggle();
            break;
        case GraphicsView::A_Remove:
            remove_action->toggle();
            break;
        case GraphicsView::A_Marker:
            marker_action->toggle();
            break;
        case GraphicsView::A_Nothing:
            break;
    }

    // Update list

    this->splitItems->setModel(dynamic_cast<Tab*>(this->tabWidget->currentWidget())->splitActions());
}

MainWindow::SaveFileAnswer MainWindow::askSaveFile() {

    QMessageBox ask_dialog;
    ask_dialog.setText("The document has been modified.");
    ask_dialog.setInformativeText("Do you want to save your changes?");
    ask_dialog.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    ask_dialog.setDefaultButton(QMessageBox::Save);

    switch (ask_dialog.exec()) {
        case QMessageBox::Save:
            return MainWindow::Save;
        case QMessageBox::Discard:
            return MainWindow::NoSave;
        case QMessageBox::Cancel:
        default:
            return MainWindow::Cancel;
    }

}

QString MainWindow::chooseSaveFile() {

    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setLabelText(QFileDialog::FileName, "Save file name...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(QFileDialog::ReadOnly, true);
    dialog.setDefaultSuffix("ptn");

    if (dialog.exec()) {
        auto files = dialog.selectedFiles();
        return files.count() == 0 ? QString() : files[0];
    }

    return {};
}

void MainWindow::slotSaveFile(bool checked) {
    auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!tab) return;

    auto& file = tab->file();
    if (!file.isOpen()) {
        auto filename = chooseSaveFile();
        if (filename.isEmpty() || !tab->setFile(filename)) return;
    }

    tab->scene()->saveToFile(file);
    tab->setChanged(false);
}

void MainWindow::slotSaveAsFile(bool checked) {
    auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!tab) return;

    if (auto filename = chooseSaveFile(); !filename.isEmpty() && tab->setFile(filename)) {
        tab->scene()->saveToFile(tab->file());
        tab->setChanged(false);
    }
}

void MainWindow::slotOpenFile(bool checked) {
    Q_UNUSED(checked);

    auto tab = newTab();

    auto filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(QDir::currentPath()), tr("PTN Files (*.ptn)"));
    QFile file(filename);
    file.open(QIODeviceBase::ReadWrite);

    tab->scene()->openFile(file);
}

void MainWindow::slotSplitAction(bool checked) {

    auto current_tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!current_tab) return;

    current_tab->splitAction();

}




void MainWindow::slotSplitChecked(const QModelIndex& index) {
    auto result = index.data(Qt::UserRole);
    auto list = result.toList();
    auto currentTab = dynamic_cast<Tab*>(tabWidget->currentWidget());

    std::for_each(currentTab->scene()->getItems().begin(), currentTab->scene()->getItems().end(), [](QGraphicsItem* item) {
        dynamic_cast<PetriObject*>(item)->setColored(false);
    });

    for (auto item : list) {
        auto str = item.toString();
        auto inner_index = str.right(str.length() - 1).toInt();
        auto find_type = str.startsWith('p') ? PetriObject::Position : PetriObject::Transition;

        auto items = currentTab->scene()->getItems();
        auto it = std::find_if(items.begin(), items.end(), [=](QGraphicsItem* item) {
            auto object = dynamic_cast<PetriObject*>(item);
            return object->objectType() == find_type && object->index() == inner_index;
        });

        if (it != items.end()) dynamic_cast<PetriObject*>(*it)->setColored(true);
    }
}

void showTable(QAbstractTableModel* model, QString title, int sectionSize) {
    QTableView* c_view = new QTableView;
    c_view->setWindowTitle(title);
    QHeaderView* vert = c_view->verticalHeader();
    vert->setSectionResizeMode(QHeaderView::Fixed);
    vert->setDefaultSectionSize(sectionSize);
    c_view->setItemDelegate(new ProgramItemDelegate);

    QHeaderView* horz = c_view->horizontalHeader();
    horz->setSectionResizeMode(QHeaderView::Fixed);
    horz->setDefaultSectionSize(sectionSize);

    c_view->setModel(model);
    c_view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    c_view->adjustSize();
    c_view->show();
}

void MainWindow::addTabFromNet(InnerCommonResult common_result, Tab* current) {

    auto net = std::move(common_result.net);

    auto items = current->scene()->getItems();
    auto points = std::map<QString, PetriObject*>();
    for (auto item : items) {
        if (auto position = dynamic_cast<Position*>(item); position) {
            points.insert({QString::fromStdString(fmt::format("p{}", position->index())), position});
        }
        else if (auto transition = dynamic_cast<Transition*>(item); transition) {
            points.insert({QString::fromStdString(fmt::format("t{}", transition->index())), transition});
        }
    }

    auto tab_index = tabWidget->addTab(new Tab(this), "Результат синтеза");
    auto tab = dynamic_cast<Tab*>(tabWidget->widget(tab_index));
    auto scene = tab->scene();

    auto added_objects = std::map<QString, PetriObject*>();
    for (int i = 0; i < net.elements.size(); i++) {
        auto element = net.elements[i];
        auto position = points.find(element) != points.end() ? (*points.find(element)).second : nullptr;
        auto point = position ? position->pos() : QPointF(150.0, 150.0);

        if (element.startsWith('p')) {
            auto added_position = PetriObject::castTo<Position>(scene->addPosition(element, point));
            added_position->setMarkers(net.markers[i]);
            added_objects.insert({element, added_position});
        }
        else {
            auto rotation = position ? position->rotation() : 0.;
            auto added = scene->addTransition(element, point);
            added->setRotation(rotation);
            added_objects.insert({element, added});
        }
    }

    for (auto& conn : net.connections) {
        auto s1 = (*added_objects.find(get<0>(conn))).second;
        auto s2 = (*added_objects.find(get<1>(conn))).second;

        scene->newConnection(s1, s2);
    }

    tabWidget->setCurrentIndex(tab_index);

    /// C_MATRIX
    showTable(MatrixModel::loadFromMatrix(common_result.c_matrix), "Тензор преобразования", 25);
//    showTable(NamedMatrixModel::loadFromMatrix(common_result.d_input), "D input",35);
//    showTable(NamedMatrixModel::loadFromMatrix(common_result.d_output), "D output", 35);
//    showTable(NamedMatrixModel::loadFromMatrix(common_result.lbf_matrix), "Примитивная система", 35);

}