//
// Created by Николай Муравьев on 12.12.2021.
//

#ifndef FFI_RUST_SCENE_H
#define FFI_RUST_SCENE_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "drawable_object.h"
#include "circle.h"
#include "rectangle.h"

class Scene : public QWidget {

public:

    explicit Scene(QWidget* parent = nullptr): QWidget(parent) {

    }

    QSize sizeHint() const override {
        return QSize(400, 200);
    }

    void mousePressEvent(QMouseEvent* event) override {

        QPointF pos = event->position();
        for (int i = 0; i < this->objects.size(); i++) {
            if (objects[i]->contains(pos)) {
                drag = objects[i];
                state = State::Move;
                break;
            }
        }

        if (state != State::Move) {
            switch (event->button()) {
                case Qt::MouseButton::LeftButton:
                    objects.push_back(new Circle(pos, 10));
                    break;
                case Qt::MouseButton::RightButton:
                default:
                    objects.push_back(new Rectangle(pos, 10, 40));
                    break;
            }

            drag = objects.back();
            state = State::Move;
        }

        update();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (state == State::Move) {

            if (!drag) return;

            drag->setPos(event->position());

            update();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        state = State::Nothing;
        drag = nullptr;

        update();
    }

    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setPen(QPen(Qt::black));
        painter.setBrush(QBrush(Qt::black));
        painter.setRenderHint(QPainter::Antialiasing);

        for (int i = 0; i < this->objects.size(); i++) {
            objects[i]->paint(&painter);
        }
    }

private:

    std::vector<DrawableObject*> objects;

    DrawableObject* drag = nullptr;

    enum State {
        Move,
        Nothing
    };

    State state = State::Nothing;

};

#endif //FFI_RUST_SCENE_H
