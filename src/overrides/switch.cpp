#include "switch.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QApplication>
#include <QStyle>

static const QBrush checkedBrush = QBrush(QColor(0x22, 0xc8, 0x74));
static const QBrush uncheckedBrush = QBrush(Qt::gray);
static const QBrush d_checkedBrush = QBrush(QColor(0x1F, 0x6D, 0x46));
static const QBrush d_uncheckedBrush = QBrush(Qt::lightGray);

Switch::Switch(QWidget* parent) : QAbstractButton(parent)
  ,m_animation(new QPropertyAnimation(this, "offset", this))
{
    setContentsMargins(QMargins());
    setCheckable(true);
    setChecked(false);
    m_textMargins = QMargins(8, 0, 8, 0);
    m_trackMargins = QMargins(8, 0, 8, 0);
    setMaximumHeight(20);
    setMinimumHeight(20);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    auto plt = palette();
    plt.setColor(QPalette::Active, QPalette::ButtonText, Qt::black);
    plt.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::lightGray);
    setPalette(plt);
}

Switch::Switch(const QString& text, QWidget* parent) : Switch(parent)
{
    setText(text);
}

void Switch::paintEvent(QPaintEvent* event) {

    Q_UNUSED(event);

    QBrush trackColor;
    QColor textColor;
    QBrush thumbColor;

    if (isEnabled()) {
        trackColor = isChecked() ? checkedBrush : uncheckedBrush;
        thumbColor = Qt::white;
        textColor = palette().color(QPalette::Active, QPalette::ButtonText);
    }
    else {
        trackColor = isChecked() ? d_checkedBrush : d_uncheckedBrush;
        thumbColor = QColor(0xD9, 0xE0, 0xDD);
        textColor = palette().color(QPalette::Disabled, QPalette::ButtonText);
    }

    auto btnRect = buttonRect();
    auto noMarginBtnRect = btnRect.marginsRemoved(m_trackMargins);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // draw track
    painter.setOpacity(1.0);
    painter.setBrush(isChecked() ? checkedBrush : uncheckedBrush);
    painter.drawRoundedRect(noMarginBtnRect, m_radius, m_radius);

    // draw thumb
    auto thumbCenter = QPoint(noMarginBtnRect.x() + m_radius + offset(), noMarginBtnRect.y() + noMarginBtnRect.height() / 2);
    noMarginBtnRect.setX(noMarginBtnRect.x() + offset() - m_radius);
    painter.setBrush(Qt::white);
    painter.drawEllipse(thumbCenter, m_radius, m_radius);

    if (text().isEmpty()) return;

    painter.setPen(QApplication::palette().text().color());
    painter.drawText(textRect(), Qt::AlignLeft | Qt::AlignVCenter, text());

}

QSize Switch::sizeHint() const
{
    auto text_w = m_textMargins.left() + fontMetrics().boundingRect(text()).width() + m_textMargins.right();
    auto h = 20;
    auto w = m_trackMargins.left() + 2 * h + m_trackMargins.right() + (text().isEmpty() ? 0 : text_w);
    return QSize(w, h);
}

void Switch::checkStateSet()
{
    const auto state = isChecked() ? Qt::Checked : Qt::Unchecked;
    toggle(state);
}

void Switch::nextCheckState()
{
    QAbstractButton::nextCheckState();
    checkStateSet();
}

void Switch::toggle(Qt::CheckState state)
{
    if (m_animation->state() == QPropertyAnimation::Running) {
        m_animation->stop();
    }

    if (state == Qt::Checked) {
        m_animation->setStartValue(0);
        m_animation->setDuration(isVisible() ? 300 : 0);
        m_animation->setEndValue(2 * m_radius);
        m_animation->setEasingCurve(QEasingCurve::OutBounce);
        m_animation->start();
    }
    else {
        m_animation->setStartValue(2 * m_radius);
        m_animation->setDuration(isVisible() ? 120 : 0);
        m_animation->setEndValue(0);
        m_animation->setEasingCurve(QEasingCurve::Linear);
        m_animation->start();
    }
}

void Switch::resizeEvent(QResizeEvent *event) {
    m_radius = height() / 2;
    QAbstractButton::resizeEvent(event);
}

QRect Switch::buttonRect() const
{
    auto w = m_trackMargins.left() + 2 * height() + m_trackMargins.right();
    return QRect(width() - w, 0, w, height());
}

QRect Switch::textRect() const
{
    auto width = m_textMargins.left() + 2 * height() + m_textMargins.right();
    return rect().marginsRemoved(QMargins(0, 0, width, 0));
}
