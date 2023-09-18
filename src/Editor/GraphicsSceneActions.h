//
// Created by darkp on 17.09.2023.
//

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

private:

    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_hAlignment;
    QAction* m_vAlignment;
};


#endif //GPTN_GRAPHICSSCENEACTIONS_H
