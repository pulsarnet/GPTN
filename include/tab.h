//
// Created by nmuravev on 12/16/2021.
//

#ifndef FFI_RUST_TAB_H
#define FFI_RUST_TAB_H

#include <QWidget>
#include <QFile>
#include <QAction>
#include "view/graphics_view.h"
#include "split_list_model.h"
#include "view/graphics_scene.h"

class Tab : public QWidget {

    Q_OBJECT

    void setFileName(QString filename);

public:

    explicit Tab(QWidget* parent = nullptr);

    GraphicScene* scene() {
        return qobject_cast<GraphicScene*>(edit_view->scene());
    }

    GraphicsView* primitive() {
        return primitive_view;
    }

    GraphicsView* lbf() {
        return lbf_view;
    }

    /*
     * Mark document changed
     *
     * @param changed Boolean sign are file changed
     */
    void setChanged(bool changed);

    void splitAction();

    void saveToFile();
    void loadFromFile();

public slots:

    void slotDocumentChanged() {}

private:

    QVariant toData();
    void fromData(QVariant data);


private:

    QGraphicsView* edit_view = nullptr;
    GraphicsView* primitive_view = nullptr;
    GraphicsView* lbf_view = nullptr;

    SplitListModel* m_split_actions;

    PetriNet* m_net;

    QString m_filename;

};


#endif //FFI_RUST_TAB_H
