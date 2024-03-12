//
// Created by Николай Муравьев on 12.01.2022.
//

#ifndef FFI_RUST_TOOL_H
#define FFI_RUST_TOOL_H

#include <QToolButton>
#include <QPainterPath>
#include <QPainter>
#include <QCursor>
#include <QToolTip>
#include <QStyleOptionToolButton>
#include <QGuiApplication>
#include <QApplication>

#include <QScreen>

class Tool : public QToolButton {

public:

    explicit Tool(QAction* action, const QString& description = QString(), QWidget* parent = nullptr)
        : QToolButton(parent)
        , mDescription(description)
    {
        this->setDefaultAction(action);
        this->setMouseTracking(true);
        this->setAttribute(Qt::WA_Hover);
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

        QColor highlightedBack(0, 166, 251);
        QColor back(94, 94, 94); // E6E6E9
        QColor hover(135, 134, 134);

        if (defaultAction() && defaultAction()->isChecked()) painter.setBackground(QBrush(highlightedBack));
        else if (opts.state & QStyle::StateFlag::State_MouseOver) painter.setBackground(QBrush(hover));
        else painter.setBackground(QBrush(back));

        painter.setOpacity(0.7);

        QPainterPath path;
        path.addRoundedRect(rect(), 5, 5);
        painter.fillPath(path, painter.background());

        painter.setOpacity(1.);

        QRect iconRect = rect().adjusted(5, 5, -5, -5);
        if (defaultAction()) defaultAction()->icon().paint(&painter, iconRect);
    }

protected:
    bool event(QEvent *e) override {
        if (e->type() == QEvent::ToolTip) {
            auto tooltipPos = QPoint(
                    this->pos().x() + this->geometry().width() + 5,
                    this->pos().y() - 16
                    );

            QString toolTipText = QString("<b>%1</b>").arg(toolTip());
            if (!mDescription.isEmpty()) {
                toolTipText.append(QString("<p>%1</p>").arg(mDescription));
            }
            if (QAction* action = this->defaultAction(); action && !action->shortcut().isEmpty()) {
                toolTipText.append(QString("<p><i>Shortcut: %1<i></p>").arg(action->shortcut().toString(QKeySequence::NativeText)));
            }

            QToolTip::showText(this->parentWidget()->mapToGlobal(tooltipPos), toolTipText, this, QRect(), toolTipDuration());
            return true;
        }

        return QToolButton::event(e);
    }

    void enterEvent(QEnterEvent *) override {
        setCursor(Qt::PointingHandCursor);
    }

    void leaveEvent(QEvent *) override {
        setCursor(Qt::ArrowCursor);
    }

private:

    QString mDescription;

};

#endif //FFI_RUST_TOOL_H
