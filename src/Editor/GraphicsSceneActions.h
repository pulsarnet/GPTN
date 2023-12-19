#ifndef GPTN_GRAPHICSSCENEACTIONS_H
#define GPTN_GRAPHICSSCENEACTIONS_H

#include <QAction>

class GraphicsScene;

class GraphicsSceneActions : public QObject {

public:

    explicit GraphicsSceneActions(GraphicsScene* scene);

    [[nodiscard]] QAction* undoAction() const { return m_undoAction; }
    [[nodiscard]] QAction* redoAction() const { return m_redoAction; }
    [[nodiscard]] QAction* hAlignmentAction() const { return m_hAlignment; }
    [[nodiscard]] QAction* vAlignmentAction() const { return m_vAlignment; }
    [[nodiscard]] QMenu* graphViz() const { return m_graphVizMenu; }

private:

    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_hAlignment;
    QAction* m_vAlignment;

    QMenu* m_graphVizMenu;
};


#endif //GPTN_GRAPHICSSCENEACTIONS_H
