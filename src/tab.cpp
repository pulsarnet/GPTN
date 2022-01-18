//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "../include/tab.h"

#include "../include/mainwindow.h"
#include "../include/synthesis/synthesis_view.h"
#include "../include/ffi/rust.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    auto main_scene = new GraphicScene;
    main_scene->setAllowMods(GraphicScene::A_Default);
    this->edit_view = new GraphicsView(this);
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

    auto manager = new CDockManager;

    auto edit_docker = new CDockWidget("Main view");
    edit_docker->setWidget(edit_view);

    auto primitive_docker = new CDockWidget("Primitive view");
    primitive_docker->setWidget(primitive_view);

    auto lbf_docker = new CDockWidget("Lbf view");
    lbf_docker->setWidget(lbf_view);

    manager->addDockWidget(DockWidgetArea::OuterDockAreas, edit_docker);

    auto bottomArea = manager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, primitive_docker);
    manager->addDockWidgetTabToArea(lbf_docker, bottomArea);

    this->setLayout(new QGridLayout);
    this->layout()->addWidget(manager);
    this->layout()->setContentsMargins(QMargins());

    connect(main_scene, &QGraphicsScene::changed, this, &Tab::slotDocumentChanged);

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
    dynamic_cast<GraphicScene*>(edit_view->scene())->setMode(checked ? GraphicScene::A_Remove : GraphicScene::A_Nothing);
}


void Tab::splitAction() {

    auto synthesis_program = split_net(qobject_cast<GraphicScene*>(this->edit_view->scene())->net());

    // Добавим пустую программу чтобы не падало
    synthesis_program->add_program();
    auto result = split_finish(synthesis_program);

    auto scene = dynamic_cast<GraphicScene*>(primitive_view->scene());
    scene->removeAll();

    auto primitive_matrix = result.lbf_matrix;
    auto transitions_count = primitive_matrix.cols.count();
    int tran_rows = qMax(1., round((double)transitions_count / 3.));
    int tran_cols = 3;

    qDebug() << tran_rows << tran_cols;

    int start_x = 0;
    int start_y = 0;
    int offset_x = 120;
    int offset_y = 120;

    for (int i = 0; i < tran_rows; i++) {
        for (int j = 0; j < tran_cols; j++) {

            auto transition = i * tran_cols + j;

            if (transition >= primitive_matrix.cols.count()) break;

            PetriObject *first, *second;

            for (int p = 0; p < primitive_matrix.rows.count(); p++) {
                auto tmp = primitive_matrix(p, transition);
                if (tmp < 0) {
                    first = scene->addPosition(primitive_matrix.rows[p], QPointF(start_x, start_y));
                }
                else if (tmp > 0) {
                    second = scene->addPosition(primitive_matrix.rows[p], QPointF(start_x + offset_x * 2, start_y));
                }
            }

            auto center =
                    scene->addTransition(primitive_matrix.cols[transition], QPoint(start_x + offset_x, start_y));

            scene->connectItems(first, center);
            scene->connectItems(center, second);

            start_x += offset_x * 3 + 20;

        }

        start_x = 0;
        start_y += offset_y;
    }

    /// РИСУЕМ ЛОГИЧЕСКИЕ БАЗОВЫЕ ФРАГМЕНТЫ
    auto main_scene = dynamic_cast<GraphicScene*>(edit_view->scene());
    auto parents = result.parents.toVector();
    auto& lbf = result.fragments;
    QList<QGraphicsItemGroup*> groups;

    scene = dynamic_cast<GraphicScene*>(lbf_view->scene());
    scene->removeAll();

    for (auto& net : lbf) {
        QRectF fragmentRect(0, 0, 0, 0);
        QList<QGraphicsItem*> group;

        for (auto &element: net.elements) {
            auto index = element.mid(1).toInt();

            PetriObject *newElement;

            if (element.startsWith("p")) {
                newElement = scene->addPosition(element, QPoint(0, 0));
                auto it = std::find_if(parents.begin(), parents.end(), [&](FFIParent &p) {
                    return p.type == FFIVertexType::Position && p.child == index;
                });

                if (it == parents.end()) {
                    newElement->setPos(main_scene->getPositionPos(index));

                    auto childIt = std::find_if(parents.begin(), parents.end(), [&](FFIParent &p) {
                        return p.type == FFIVertexType::Position && p.parent == index;
                    });

                    if (childIt != parents.end()) {
                        newElement->setPos(newElement->scenePos() - QPointF(40., 0));
                    }
                }
                else {
                    newElement->setPos(main_scene->getPositionPos((*it).parent)  + QPointF(40., 0));
                }
            } else {
                newElement = scene->addTransition(element, QPoint(0, 0));
                auto it = std::find_if(parents.begin(), parents.end(), [&](FFIParent &p) {
                    return p.type == FFIVertexType::Transition && p.child == index;
                });

                auto common = main_scene->getTransition(it == parents.end() ? index : (*it).parent);
                newElement->setPos(common->scenePos());
                newElement->setRotation(common->rotation());
            }

            group.push_back(newElement);

            fragmentRect = fragmentRect.united(newElement->sceneBoundingRect());
        }

        groups.push_back(scene->createItemGroup(group));
        groups.last()->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

        for (auto connection: net.connections) {
            PetriObject *from;
            PetriObject *to;
            if (connection.first.startsWith("p")) {
                from = scene->getPosition(connection.first.mid(1).toInt());
                to = scene->getTransition(connection.second.mid(1).toInt());
            } else {
                from = scene->getTransition(connection.first.mid(1).toInt());
                to = scene->getPosition(connection.second.mid(1).toInt());
            }

            scene->connectItems(from, to);
        }
    }

    lbf_view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

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
    }

}

QVariant Tab::toData() {
    return qobject_cast<GraphicScene*>(this->edit_view->scene())->toVariant();
}

void Tab::setFileName(QString filename) {

    m_filename = filename;

    if (m_filename.isEmpty()) this->setWindowTitle("Untitled");
    else this->setWindowTitle(QFileInfo(filename).fileName());

}

void Tab::fromData(QVariant data) {
    qobject_cast<GraphicScene*>(this->edit_view->scene())->fromVariant(data);
    this->edit_view->fitInView(this->edit_view->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}
