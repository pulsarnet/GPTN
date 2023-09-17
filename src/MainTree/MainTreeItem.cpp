#include "MainTreeItem.h"
#include <QIcon>
#include <utility>

MainTreeItem::MainTreeItem(QString name, QIcon icon, MainTreeItem *parent)
    : QObject(nullptr)
    , m_parentItem(parent)
    , m_name(std::move(name))
    , m_icon(std::move(icon))
    , m_widget(nullptr)
{
    if (parent) {
        // Добавим текущий элемент как потомок parent
        parent->insertChild(parent->childCount(), this);
    }
}

MainTreeItem::MainTreeItem(QString name, QIcon icon, QWidget *widget, MainTreeItem *parent)
    : QObject(nullptr)
    , m_parentItem(parent)
    , m_name(std::move(name))
    , m_icon(std::move(icon))
    , m_widget(widget)
{
    if (parent) {
        // Добавим текущий элемент как потомок parent
        parent->insertChild(parent->childCount(), this);
    }
}

int MainTreeItem::childCount() const noexcept {
    return (int)m_childItems.size();
}

int MainTreeItem::columnCount() const noexcept {
    // По умолчанию одна колонка
    return 1;
}

int MainTreeItem::childNumber() const noexcept  {
    //Если есть родитель - найти свой номер в списке его потомков
    if (m_parentItem) return (int)m_parentItem->m_childItems.indexOf(const_cast<MainTreeItem*>(this));
    return 0; //Иначе вернуть 0
}

bool MainTreeItem::insertChild(int row, MainTreeItem *item) {
    if (!item)
        return false;

    m_childItems.insert(row, item);
    return true;
}

bool MainTreeItem::removeChildren(int position, int count) noexcept {
    if (position < 0 || position + count > m_childItems.size()) {
        return false;
    }

    for (int row = 0; row < count; ++row) {
        delete m_childItems.takeAt(position);
    }

    return true;
}

QVariant MainTreeItem::data(int column) const noexcept {
    if (column != 0) return {};
    return m_name;
}

QIcon MainTreeItem::icon() const noexcept {
    return m_icon;
}

QWidget *MainTreeItem::widget() const noexcept {
    return m_widget;
}

MainTreeItem::~MainTreeItem() {
    qDeleteAll(m_childItems);
}
