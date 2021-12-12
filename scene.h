//
// Created by Николай Муравьев on 12.12.2021.
//

#ifndef FFI_RUST_SCENE_H
#define FFI_RUST_SCENE_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class Scene : public QWidget {

public:

    explicit Scene(QWidget* parent = nullptr): QWidget(parent) {

    }

    QSize sizeHint() const override {
        return QSize(400, 200);
    }

    void mousePressEvent(QMouseEvent* event) override {
        this->x1 = event->pos().x();
        this->y1 = event->pos().y();

        this->x2 = this->x1;
        this->y2 = this->y1;

        this->isDrawing = true;

        update();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (this->isDrawing) {
            this->x2 = event->pos().x();
            this->y2 = event->pos().y();

            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        this->isDrawing = false;
        this->x2 = event->pos().x();
        this->y2 = event->pos().y();

        update();
    }

    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setPen(QPen(Qt::white));
        painter.setBrush(QBrush(Qt::black));
        painter.setRenderHint(QPainter::Antialiasing);

        painter.drawLine(x1, y1, x2, y2);

    }

private:

    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    bool isDrawing = false;

};


#endif //FFI_RUST_SCENE_H
