//
// Created by Николай Муравьев on 12.01.2022.
//

#ifndef FFI_RUST_TOOLBOX_H
#define FFI_RUST_TOOLBOX_H

#include <QWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include "tool.h"

class ToolBox : public QWidget {

public:

    explicit ToolBox(QWidget* parent = nullptr) : QWidget(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);

        setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
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

    void addTool(QAction* tool) {
        auto button = new Tool;
        dynamic_cast<QVBoxLayout*>(this->layout())->addWidget(button);
        button->setDefaultAction(tool);
        button->setSize(m_buttonSize);
        m_buttons.push_back(button);

        this->setGeometry(
                20,
                60,
                m_buttonSize.width(),
                (m_buttonSize.height() + 6) * m_buttons.count()
                );
    }

    void resizeEvent(QResizeEvent *event) override {
        qDebug() << event;
        QWidget::resizeEvent(event);
    }

private:

    QList<Tool*> m_buttons;
    QSize m_buttonSize;

};

#endif //FFI_RUST_TOOLBOX_H
