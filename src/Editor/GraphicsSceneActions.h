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

private:

    QAction* m_undoAction;
    QAction* m_redoAction;
};


#endif //GPTN_GRAPHICSSCENEACTIONS_H
