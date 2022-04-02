//
// Created by nmuravev on 3/21/2022.
//

#ifndef FFI_RUST_DOCKTOOLBAR_H
#define FFI_RUST_DOCKTOOLBAR_H

#include <QWidget>
#include <DockWidget.h>
#include <DockAreaTitleBar.h>
#include <QLabel>
#include <QPushButton>

class QToolButton;

class DockToolbar : public ads::CDockAreaTitleBar {

    Q_OBJECT

public:

    explicit DockToolbar(ads::CDockAreaWidget *parent);
    //~DockToolbar() override;

    bool eventFilter(QObject* object, QEvent* event) override;

    bool isFullScreen() const;

    static QIcon minimizeIcon();
    static QIcon expandIcon();

public slots:

    void onWindowTitleChanged(const QString& title);
    void toggleFullScreen();

private:

    QLabel* m_label;
    QToolButton* m_fullScreenButton;

    ads::CDockWidget* m_parent;
    bool m_fullScreen = false;

};


class SpacerWidget : public QWidget {
    Q_OBJECT
public:
    explicit SpacerWidget(QWidget* parent = 0);
    virtual QSize sizeHint() const override { return QSize(0, 0); }
    virtual QSize minimumSizeHint() const override { return QSize(0, 0); }
};

#endif //FFI_RUST_DOCKTOOLB