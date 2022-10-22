//
// Created by darkp on 22.10.2022.
//

#ifndef FFI_RUST_QWTVERTEXSCALEDRAW_H
#define FFI_RUST_QWTVERTEXSCALEDRAW_H

#include <QwtScaleDraw>
#include <QHash>

class QwtVertexScaleDraw : public QwtScaleDraw {

public:

    explicit QwtVertexScaleDraw();

    void setData(const QHash<int, QString>& data);

    QwtText label(double) const override;

    void drawLabel(QPainter *, double value) const override;

private:

    QHash<int, QString> m_data;
};


#endif //FFI_RUST_QWTVERTEXSCALEDRAW_H
