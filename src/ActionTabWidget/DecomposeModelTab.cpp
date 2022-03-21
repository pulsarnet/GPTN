//
// Created by nmuravev on 3/20/2022.
//

#include "DecomposeModelTab.h"
#include "../view/graphic_scene.h"
#include "../synthesis/synthesis_table.h"
#include "WrappedLayoutWidget.h"
#include <QGridLayout>
#include <QSplitter>

DecomposeModelTab::DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent) : QWidget(parent)
    , m_netModelingTab(mainTab)
{
    m_ctx = mainTab->ctx()->decompose_ctx();

    auto horizontalSplitter1 = new QSplitter(Qt::Horizontal, this);
    auto horizontalSplitter2 = new QSplitter(Qt::Horizontal, this);
    auto verticalSplitter = new QSplitter(Qt::Vertical, this);

    auto linear_base_fragments_scene = new GraphicScene(m_ctx->linear_base_fragments());
    auto linear_base_fragments_view = new GraphicsView;
    linear_base_fragments_view->setScene(linear_base_fragments_scene);
    linear_base_fragments_view->setWindowTitle("Линейно-базовые фрагменты");
    linear_base_fragments_view->setToolBoxVisibility(false);


    auto primitive_net_scene = new GraphicScene(m_ctx->primitive_net());
    auto primitive_net_view = new GraphicsView;
    primitive_net_view->setScene(primitive_net_scene);
    primitive_net_view->setWindowTitle("Примитивная система");
    primitive_net_view->setToolBoxVisibility(false);

    auto programs_table = new SynthesisTable(m_ctx);


    auto primitive_net_scene1 = new GraphicScene(m_ctx->primitive_net());
    auto primitive_net_view1 = new GraphicsView;
    primitive_net_view1->setScene(primitive_net_scene1);
    primitive_net_view1->setWindowTitle("Hello");
    primitive_net_view1->setToolBoxVisibility(false);


    horizontalSplitter1->addWidget(new WrappedLayoutWidget(linear_base_fragments_view, this));
    horizontalSplitter1->addWidget(new WrappedLayoutWidget(primitive_net_view, this));

    horizontalSplitter2->addWidget(new WrappedLayoutWidget(programs_table, this));
    horizontalSplitter2->addWidget(new WrappedLayoutWidget(primitive_net_view1, this));

    verticalSplitter->addWidget(horizontalSplitter1);
    verticalSplitter->addWidget(horizontalSplitter2);

    setLayout(new QGridLayout(this));
    layout()->addWidget(verticalSplitter);
}