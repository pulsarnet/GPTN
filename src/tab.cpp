//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>

#include "tab.h"
#include "mainwindow.h"
#include "ffi/rust.h"

#include "elements/position.h"
#include "elements/transition.h"
#include "elements/arrow_line.h"

#include "graphviz/graphviz_wrapper.h"
#include "toolbox/toolbox.h"
#include "synthesis/synthesis_table.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    this->edit_view = nullptr;
    this->lbf_view = nullptr;
    this->primitive_view = nullptr;
    this->synthesis_result = nullptr;

    auto main_scene = new GraphicScene;
    main_scene->setAllowMods(GraphicScene::A_Default);
    this->edit_view = new GraphicsView();
    this->edit_view->setWindowTitle("Main view");
    this->edit_view->setScene(main_scene);

    auto primitive_scene = new GraphicScene;
    primitive_scene->setAllowMods(GraphicScene::A_Nothing);
    this->primitive_view = new GraphicsView(this);
    this->primitive_view->setWindowTitle("Primitive View");
    this->primitive_view->setScene(primitive_scene);

    auto lbf_scene = new GraphicScene;
    lbf_scene->setAllowMods(GraphicScene::A_Nothing | GraphicScene::A_Move);
    lbf_scene->setMode(GraphicScene::A_Move);
    this->lbf_view = new GraphicsView(this);
    this->lbf_view->setWindowTitle("Logical Base Fragments View");
    this->lbf_view->setScene(lbf_scene);

    // TODO: Удалить
    auto synthesis_scene = new GraphicScene;
    synthesis_scene->setAllowMods(GraphicScene::A_Nothing);
    this->synthesis_result = new GraphicsView(this);
    this->synthesis_result->setWindowTitle("Synthesis view");
    this->synthesis_result->setScene(synthesis_scene);

    m_manager = new CDockManager;

    auto edit_docker = new CDockWidget("Main view");
    edit_docker->setWidget(edit_view);

    auto primitive_docker = new CDockWidget("Primitive view");
    primitive_docker->setWidget(primitive_view);

    auto lbf_docker = new CDockWidget("Lbf view");
    lbf_docker->setWidget(lbf_view);

    // TODO: Удалить
    auto synthesis_docker = new CDockWidget("Synthesis view");
    synthesis_docker->setWidget(synthesis_result);

    m_manager->addDockWidget(DockWidgetArea::OuterDockAreas, edit_docker);

    auto bottomArea = m_manager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, primitive_docker);
    m_manager->addDockWidgetTabToArea(lbf_docker, bottomArea);

    // TODO: Удалить
    m_manager->addDockWidgetTabToArea(synthesis_docker, bottomArea);

    this->setLayout(new QGridLayout);
    this->layout()->addWidget(m_manager);
    this->layout()->setContentsMargins(QMargins());

    m_actionToggleMenu = new QMenu("View");
    m_actionToggleMenu->addAction(edit_docker->toggleViewAction());
    m_actionToggleMenu->addAction(primitive_docker->toggleViewAction());
    m_actionToggleMenu->addAction(lbf_docker->toggleViewAction());

    toolBar = new ToolBox;
    toolBar->setParent(this);
    toolBar->setVisible(true);
    toolBar->setButtonSize(QSize(40, 40));

    actionGroup = new QActionGroup(toolBar);

    position_action = makeAction("Position", QIcon(":/images/circle.png"), true, actionGroup);
    transition_action = makeAction("Transition", QIcon(":/images/rectangle.png"), true, actionGroup);
    move_action = makeAction("Move", QIcon(":/images/move.png"), true, actionGroup);
    connect_action = makeAction("Connect", QIcon(":/images/connect.png"), true, actionGroup);
    rotation_action = makeAction("Rotate", QIcon(":/images/rotation.png"), true, actionGroup);
    remove_action = makeAction("Remove", QIcon(":/images/remove.png"), true, actionGroup);
    marker_action = makeAction("Marker", QIcon(":/images/marker.png"), true, actionGroup);


    connect(position_action, &QAction::toggled, this, &Tab::positionChecked);
    connect(transition_action, &QAction::toggled, this, &Tab::transitionChecked);
    connect(move_action, &QAction::toggled, this, &Tab::moveChecked);
    connect(connect_action, &QAction::toggled, this, &Tab::connectChecked);
    connect(rotation_action, &QAction::toggled, this, &Tab::rotateChecked);
    connect(remove_action, &QAction::toggled, this, &Tab::removeChecked);
    connect(marker_action, &QAction::toggled, this, &Tab::markerChecked);


    toolBar->addTool(position_action);
    toolBar->addTool(marker_action);
    toolBar->addTool(transition_action);
    toolBar->addTool(connect_action);
    toolBar->addTool(remove_action);

    toolBar->addTool(move_action);
    toolBar->addTool(rotation_action);
}

QAction* Tab::makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup) {
    auto action = new QAction(name);
    action->setIcon(icon);
    action->setCheckable(checkable);

    if (actionGroup) actionGroup->addAction(action);

    return action;
}

void Tab::positionChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Position : GraphicScene::A_Nothing);
}

void Tab::transitionChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Transition : GraphicScene::A_Nothing);
}

void Tab::moveChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Move : GraphicScene::A_Nothing);
}

void Tab::connectChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Connection : GraphicScene::A_Nothing);
}

void Tab::rotateChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Rotation : GraphicScene::A_Nothing);
}

void Tab::removeChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Remove : GraphicScene::A_Nothing);
}

void Tab::markerChecked(bool checked) {
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Marker : GraphicScene::A_Nothing);
}

void Tab::drawSynthesisedNet(ffi::PetriNet *net) {
    dynamic_cast<GraphicScene*>(synthesis_result->scene())->loadFromNet(net, "neato");
}

void Tab::splitAction() {
    auto synthesisContext = ffi::SynthesisContext::init(dynamic_cast<GraphicScene*>(edit_view->scene())->net());
    dynamic_cast<GraphicScene*>(primitive_view->scene())->loadFromNet(synthesisContext->primitive_net(), "neato");

    auto lbf_scene = dynamic_cast<GraphicScene*>(lbf_view->scene());
    lbf_scene->loadFromNet(synthesisContext->linear_base_fragments());
    lbf_view->fitInView(lbf_scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    synthesisContext->add_program();
    synthesisContext->set_program_value(0, 0, 1);
    synthesisContext->set_program_value(0, 1, 1);

    auto synthesis_view = new SynthesisTable(synthesisContext, this);
    auto dock_synthesis_view = new CDockWidget("Synthesis program table");
    dock_synthesis_view->setWidget(synthesis_view);
    m_manager->addDockWidgetTab(DockWidgetArea::BottomDockWidgetArea, dock_synthesis_view);

    connect(synthesis_view, &SynthesisTable::signalSynthesisedProgram, this, &Tab::drawSynthesisedNet);
}

bool Tab::saveOnExit() {

    if (!m_changed)
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::question(
            this,
            m_filename,
            tr("The document has been changed. \n"
               "Do you want to save changes?"),
               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
            );

    switch (ret) {
        case QMessageBox::Save:
            saveToFile();
            return true;
        case QMessageBox::Cancel:
            return false;
    }

    return true;

}

void Tab::saveToFile() {

    auto filename = !m_filename.isEmpty() ? m_filename :
            QFileDialog::getSaveFileName(this, tr("Save project file"), tr("Petri network (*.ptn)"));

    if (filename.isEmpty())
        return;
    else {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_6_0);
        stream << this->toData();

        setFileName(filename);
    }
}

void Tab::loadFromFile() {

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Petri Network project"), "",
                                                    tr("Petri Network File (*.ptn)"));

    if (filename.isEmpty())
        return;
    else {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_6_0);

        QVariant data;
        stream >> data;
        fromData(data);

        setFileName(filename);

        m_changed = false;

        qDebug() << "Loaded";
    }

}

QVariant Tab::toData() {
    return qobject_cast<GraphicScene*>(this->edit_view->scene())->toVariant();
}

void Tab::setFileName(QString filename) {

    m_filename = filename;

    if (m_filename.isEmpty()) this->setWindowTitle("Untitled");
    else this->setWindowTitle(QFileInfo(filename).fileName());

    update();

}

void Tab::fromData(QVariant data) {
    qobject_cast<GraphicScene*>(this->edit_view->scene())->fromVariant(data);
    this->edit_view->fitInView(this->edit_view->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void Tab::slotDocumentChanged() {
    m_changed = true;
}
