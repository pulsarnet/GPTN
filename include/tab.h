//
// Created by nmuravev on 12/16/2021.
//

#ifndef FFI_RUST_TAB_H
#define FFI_RUST_TAB_H

#include <QWidget>
#include <QFile>
#include <QAction>
#include "graphics_view.h"
#include "split_list_model.h"

class Tab : public QWidget {

    Q_OBJECT

public:

    explicit Tab(QWidget* parent = nullptr);

    GraphicsView* scene();

    [[nodiscard]] bool changed() const { return m_changed; }

    /*
     * Mark document changed
     *
     * @param changed Boolean sign are file changed
     */
    void setChanged(bool changed);

    bool setFile(const QString&);

    QFile& file();

    void closeFile();

    void splitAction();

    SplitListModel* splitActions() { return m_split_actions; }

public slots:

    void slotDocumentChanged();

    void slotRemoveItem();

private:

    GraphicsView* view = nullptr;

    bool m_changed = false;
    QFile m_file;

    SplitListModel* m_split_actions;

};


#endif //FFI_RUST_TAB_H