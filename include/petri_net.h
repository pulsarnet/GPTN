#ifndef FFI_RUST_PETRI_NET_H
#define FFI_RUST_PETRI_NET_H

#include <QString>

struct InnerPetriNet {
    QList<QString> elements;
    QList<std::tuple<QString, QString>> connections;
};

#endif