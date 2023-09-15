//
// Created by darkp on 13.09.2022.
//

#ifndef FFI_RUST_SYNTHESIS_WINDOW_H
#define FFI_RUST_SYNTHESIS_WINDOW_H

#include <QWidget>
#include <QItemSelection>

class SynthesisTable;
class GraphicsView;
class DockWidget;

namespace ffi {
    struct DecomposeContext;
}

class SynthesisWindow : public QWidget {

    Q_OBJECT

public:

    explicit SynthesisWindow(ffi::DecomposeContext* m_ctx, QVector<size_t> programs, QWidget* parent = nullptr);

public slots:

    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    SynthesisTable* table();
    const SynthesisTable* table() const;

    GraphicsView* view();
    const GraphicsView* view() const;

    QLabel* label();
    const QLabel* label() const;

private:

    ffi::DecomposeContext* m_ctx;
    ads::CDockManager* m_manager;

    DockWidget* m_table;
    DockWidget* m_view;
    DockWidget* m_program;
};


#endif //FFI_RUST_SYNTHESIS_WINDOW_H
