#ifndef FFI_RUST_SYNTHESIS_TABLE_H
#define FFI_RUST_SYNTHESIS_TABLE_H

#include <QWidget>

namespace ptn::net {
    struct PetriNet;
}

namespace ptn::modules::decompose {
    struct DecomposeContext;
}

class QAction;
class QTableView;
class Switch;
class SynthesisModel;

class SynthesisTable : public QWidget {

    Q_OBJECT

public:

    explicit SynthesisTable(ptn::modules::decompose::DecomposeContext* ctx, QVector<size_t> programs, QWidget* parent = nullptr);

    SynthesisModel* model() const;

    QTableView* table();
    const QTableView* table() const;
private:

    QTableView* m_table = nullptr;
};

#endif //FFI_RUST_SYNTHESIS_TABLE_H
