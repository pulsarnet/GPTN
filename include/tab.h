//
// Created by nmuravev on 12/16/2021.
//

#ifndef FFI_RUST_TAB_H
#define FFI_RUST_TAB_H

#include <QWidget>
#include <QFile>
#include "graphics_view.h"

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

public slots:

    void slotDocumentChanged();

private:

    GraphicsView* view = nullptr;

    bool m_changed = false;
    QFile m_file;

};


#endif //FFI_RUST_TAB_H
