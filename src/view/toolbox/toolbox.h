//
// Created by Николай Муравьев on 12.01.2022.
//

#ifndef FFI_RUST_TOOLBOX_H
#define FFI_RUST_TOOLBOX_H

#include <QFrame>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMenu>
#include "tool.h"

class ToolBox : public QFrame {

    Q_OBJECT

public:

    enum ToolArea {
        TopLeft = 0,
        TopRight,
        BottomLeft,
        BottomRight
    };

    explicit ToolBox(QWidget* parent = nullptr) : QFrame(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

        setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
        setAttribute(Qt::WA_Resized, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        setAttribute(Qt::WA_TranslucentBackground, true);

        this->setLayout(new QVBoxLayout);
        this->layout()->setContentsMargins(0, 0, 0, 0);
        this->setContentsMargins(0, 3, 0, 3);
        this->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
    }

    void setButtonSize(const QSize& size) {
        m_buttonSize = size;
    }

    void setToolArea(ToolArea area) {
        m_toolArea = area;
    }

    void addTool(QAction* tool) {
        auto button = new Tool;
        dynamic_cast<QVBoxLayout*>(this->layout())->addWidget(button);
        button->setDefaultAction(tool);
        button->setSize(m_buttonSize);
        m_buttons.push_back(button);

        resizeEvent(nullptr);
    }

    void addTool(QMenu* menu) {
        auto button = new Tool;
        menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint);
        menu->setAttribute(Qt::WA_TranslucentBackground);

        dynamic_cast<QVBoxLayout*>(this->layout())->addWidget(button);
        button->setDefaultAction(new QAction);
        button->setMenu(menu);
        button->setSize(m_buttonSize);
        button->setPopupMode(QToolButton::InstantPopup);
        m_buttons.push_back(button);

        resizeEvent(nullptr);
    }

    QPointF areaPoint() {
        switch (m_toolArea) {
            case TopLeft:
                return QPointF(
                        (parentWidget() ? parentWidget()->geometry().topLeft().x() : 0) + 20,
                        (parentWidget() ? parentWidget()->geometry().topLeft().y() : 0) + 20
                        );
            case TopRight:
                return QPointF(
                        (parentWidget() ? parentWidget()->geometry().topRight().x() : 0) - 20 - m_buttonSize.width(),
                        (parentWidget() ? parentWidget()->geometry().topRight().y() : 0) + 20
                );
            case BottomLeft:
                return QPointF(
                        (parentWidget() ? parentWidget()->geometry().bottomLeft().x() : 0) + 20,
                        (parentWidget() ? parentWidget()->geometry().bottomLeft().y() : 0) - 20 - qreal((m_buttonSize.height() + 6) * m_buttons.count())
                );
            case BottomRight:
                return QPointF(
                        (parentWidget() ? parentWidget()->geometry().bottomRight().x() : 0) - 20 - m_buttonSize.width(),
                        (parentWidget() ? parentWidget()->geometry().bottomRight().y() : 0) - 20 - qreal((m_buttonSize.height() + 6) * m_buttons.count())
                );
        }
    }

    void resizeEvent(QResizeEvent *event) override {
        auto point = areaPoint();
        this->setGeometry(
                point.x(),
                point.y(),
                m_buttonSize.width(),
                (m_buttonSize.height() + 6) * m_buttons.count()
        );

        QWidget::resizeEvent(event);
    }

private:

    QList<Tool*> m_buttons;
    QSize m_buttonSize;
    ToolArea m_toolArea = TopLeft;

};

#endif //FFI_RUST_TOOLBOX_H
