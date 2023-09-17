//
// Created by darkp on 17.09.2023.
//

#ifndef GPTN_GRAPHICSCENEACTIONS_H
#define GPTN_GRAPHICSCENEACTIONS_H

#include <QAction>

class GraphicsScene;

class GraphicSceneActions : public QObject {

public:

    explicit GraphicSceneActions(GraphicsScene* scene);

    [[nodiscard]] QAction* undoAction() const { return m_undoAction; }
    [[nodiscard]] QAction* redoAction() const { return m_redoAction; }

private:

    QAction* m_undoAction;
    QAction* m_redoAction;
};


#endif //GPTN_GRAPHICSCENEACTIONS_H
