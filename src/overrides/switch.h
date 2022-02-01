#ifndef SWITCH_H
#define SWITCH_H

#include <QAbstractButton>
#include <QFontMetrics>
#include <QMargins>

class QPropertyAnimation;

class Switch final : public QAbstractButton
{

    Q_OBJECT
    Q_PROPERTY(int offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    explicit Switch(QWidget* parent = nullptr);
    explicit Switch(const QString& text, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    int offset() const {
        return m_offset;
    }

    void setOffset(int offset) {
        m_offset = offset;
        update();
    }

    QSize sizeHint() const override;

    void resizeEvent(QResizeEvent* event) override;

    QRect buttonRect() const;

    QRect textRect() const;

Q_SIGNALS:
    void offsetChanged(int offset);

protected:

    void checkStateSet() override;

    void nextCheckState() override;

    void toggle(Qt::CheckState);

private:

    int m_radius = 8;
    int m_offset = 0;
    QMargins m_textMargins;
    QMargins m_trackMargins;

    QPropertyAnimation* m_animation = nullptr;

};

// Thumb pos:
// x, y (start 0, 0) - animation
#endif // SWITCH_H
