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

signals:

    void signalFullScreen();

public slots:

    void onWindowTitleChanged(const QString& title);

private:

    QLabel* m_label;
    QToolButton* m_fullScreenButton;

    ads::CDockWidget* m_parent;

};


#endif //FFI_RUST_DOCKTOOLB