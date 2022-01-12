//
// Created by Николай Муравьев on 12.01.2022.
//

#ifndef FFI_RUST_TOOL_H
#define FFI_RUST_TOOL_H

#include <QToolButton>

class Tool : public QToolButton {

public:

    explicit Tool(QWidget* parent = nullptr) : QToolButton(parent) {
        this->setMouseTracking(true);
        this->setAttribute(Qt::WA_Hover);
        this->setToolTipDuration(500);
    }

    void setSize(const QSize& size) {
        this->setFixedSize(size);
        this->setIconSize(size);
    }

    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QStyleOptionToolButton opts;
        initStyleOption(&opts);

        QColor highlightedBack(86, 128, 194);
        QColor back(49, 49, 49);
        QColor hover(69, 69, 69);

        if (defaultAction()->isChecked()) painter.setBackground(QBrush(highlightedBack));
        else if (opts.state & QStyle::StateFlag::State_MouseOver) painter.setBackground(QBrush(hover));
        else painter.setBackground(QBrush(back));

        qDebug() << opts.state;

        QPainterPath path;
        path.addRoundedRect(rect(), 5, 5);
        painter.fillPath(path, painter.background());

        QRect iconRect = rect().adjusted(5, 5, -5, -5);
        defaultAction()->icon().paint(&painter, iconRect);

    }

};

#endif //FFI_RUST_TOOL_H
