//
// Created by nmuravev on 12/16/2021.
//

#ifndef FFI_RUST_TAB_H
#define FFI_RUST_TAB_H

#include <QWidget>
#include <QFile>
#include <QAction>
#include <DockManager.h>
#include "view/graphics_view.h"
#include "view/graphic_scene.h"

class ToolBox;
struct PetriNet;

class Tab : public QWidget {

    Q_OBJECT

    void setFileName(QString filename);

public:

    explicit Tab(QWidget* parent = nullptr);

    void splitAction();
    void dotVisualization(char* algorithm);
    bool saveOnExit();

    void saveToFile();
    void loadFromFile();

    QMenu* menuOfDockToggle() {
        return m_actionToggleMenu;
    }

public slots:

    void slotDocumentChanged();

    void positionChecked(bool checked);
    void transitionChecked(bool checked);
    void moveChecked(bool checked);
    void connectChecked(bool checked);
    void rotateChecked(bool checked);
    void removeChecked(bool checked);
    void markerChecked(bool checked);

private:

    QVariant toData();
    void fromData(QVariant data);

    QAction* makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup);

private:

    GraphicsView* edit_view;
    GraphicsView* primitive_view;
    GraphicsView* lbf_view;
    QMenu* m_actionToggleMenu = nullptr;

    PetriNet* m_net;

    QString m_filename;

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;
    QAction* rotation_action = nullptr;
    QAction* remove_action = nullptr;
    QAction* marker_action = nullptr;
    QActionGroup* actionGroup = nullptr;

    ToolBox* toolBar = nullptr;

    ads::CDockManager* m_manager = nullptr;

    bool m_changed = false;

};


#endif //FFI_RUST_TAB_H
