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

    auto linearBaseFragmentsScene = new GraphicScene(m_ctx->linear_base_fragments());
    m_linearBaseFragmentsView = new GraphicsView;
    m_linearBaseFragmentsView->setScene(linearBaseFragmentsScene);
    m_linearBaseFragmentsView->setWindowTitle("Линейно-базовые фрагменты");
    m_linearBaseFragmentsView->setToolBoxVisibility(false);


    auto primitiveNetScene = new GraphicScene(m_ctx->primitive_net());
    m_primitiveNetView = new GraphicsView;
    m_primitiveNetView->setScene(primitiveNetScene);
    m_primitiveNetView->setWindowTitle("Примитивная система");
    m_primitiveNetView->setToolBoxVisibility(false);

    m_synthesisTable = new SynthesisTable(m_ctx);
    connect(m_synthesisTable, &SynthesisTable::signalSynthesisedProgram, this, &DecomposeModelTab::slotSynthesisedProgram);


    m_synthesisedProgramView = new GraphicsView;
    m_synthesisedProgramView->setWindowTitle("Синтезированная структура");
    m_synthesisedProgramView->setToolBoxVisibility(false);


    horizontalSplitter1->addWidget(new WrappedLayoutWidget(m_linearBaseFragmentsView, this));
    horizontalSplitter1->addWidget(new WrappedLayoutWidget(m_primitiveNetView, this));
    horizontalSplitter1->setSizes(QList<int>({INT_MAX, INT_MAX}));

    horizontalSplitter2->addWidget(new WrappedLayoutWidget(m_synthesisTable, this));
    horizontalSplitter2->addWidget(new WrappedLayoutWidget(m_synthesisedProgramView, this));
    horizontalSplitter2->setSizes(QList<int>({INT_MAX, INT_MAX}));

    verticalSplitter->addWidget(horizontalSplitter1);
    verticalSplitter->addWidget(horizontalSplitter2);
    verticalSplitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    setLayout(new QGridLayout(this));
    layout()->addWidget(verticalSplitter);
}

void DecomposeModelTab::slotSynthesisedProgram(ffi::PetriNet *net, int index) {
    auto newScene = new GraphicScene(net);
    auto oldScene = m_synthesisedProgramView->scene();

    m_synthesisedProgramView->setScene(newScene);
    delete oldScene;
}