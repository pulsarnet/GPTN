//
// Created by nmuravev on 1/20/2022.
//

#ifndef FFI_RUST_COLUMN_DELEGATE_TYPE_H
#define FFI_RUST_COLUMN_DELEGATE_TYPE_H

#include <QObject>

enum ColumnDelegateType {
    Value,
    ShowButton
};

Q_DECLARE_METATYPE(ColumnDelegateType)

#endif //FFI_RUST_COLUMN_DELEGATE_TYPE_H
