/**** General **************************************************************
** Version:    v0.9.10
** Date:       2020-10-13
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_graphicsview.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QResizeEvent>
#include <QSize>

#include "graph_format.h"

namespace Graph {

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) : QGraphicsView(scene, parent) {
  // Set scene rect
  // this->setSceneRect(0, 0, 100, 100);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  this->setMouseTracking(true);
}

void GraphicsView::resizeEvent(QResizeEvent* event) {
  // Get new size
  QSize event_size = event->size();

  // Set view size
  this->setSceneRect(0, 0, event_size.width(), event_size.height());

  // Connect to scene (to reposition items)
  emit this->resizedView(event_size);

  // Propagate
  QGraphicsView::resizeEvent(event);
}

/*
Receives global mouse click events. Use this to synchronize the selection and closing of popup windows.
  :param event: the event propagated
*/
void GraphicsView::receiveGlobalEvent(QEvent* event) {
  // Make sure it is the correct mouse event
  if (event->type() == QEvent::MouseButtonPress) {
    // Can now cast statically
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

    if (mouse_event->button() != Qt::LeftButton) {
      return;
    }

    // Store starting location for mouse release/move events
    this->button_down_scene_position = this->mapToScene(this->mapFromGlobal(mouse_event->globalPos()));
    this->button_down_screen_position = mouse_event->globalPos();

    // No further handling required
    return;

  } else if (event->type() == QEvent::MouseButtonRelease) {
    // See MouseButtonDoubleClick
    if (this->ignore_next_global_mouse_release) {
      this->ignore_next_global_mouse_release = false;
      return;
    }

    // Can now cast statically
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

    if (mouse_event->button() != Qt::LeftButton) {
      return;
    }

    // See if the click event propagated from within this widget, otherwise no need to propagate
    if (!this->sceneRect().contains(this->mapFromGlobal(mouse_event->globalPos()))) {
      return;
    }

    // Create a QGraphicsSceneMouseEvent for propagation; Constructed according to qgraphicsview.cpp
    QGraphicsSceneMouseEvent s_mouse_event(QEvent::GraphicsSceneMouseRelease);
    s_mouse_event.setWidget(this->viewport());

    // to be able to know the move positions I would need to store this information myself
    // at this moment I am not going to use this info, so I am ignoring it.
    // s_mouse_event.setLastScenePos(QPointF());
    // s_mouse_event.setLastScreenPos(QPoint());

    s_mouse_event.setButtonDownScenePos(Qt::LeftButton, this->button_down_scene_position);
    s_mouse_event.setButtonDownScreenPos(Qt::LeftButton, button_down_screen_position);

    s_mouse_event.setScenePos(this->mapToScene(this->mapFromGlobal(mouse_event->globalPos())));
    s_mouse_event.setScreenPos(mouse_event->globalPos());

    s_mouse_event.setButtons(mouse_event->buttons());
    s_mouse_event.setButton(mouse_event->button());
    s_mouse_event.setModifiers(mouse_event->modifiers());
    s_mouse_event.setAccepted(false);

    emit this->globalMouseReleaseEvent(&s_mouse_event);

  } else if (event->type() == QEvent::MouseButtonDblClick) {
    // To prevent additional sending of graphselect events, block the release event after dbl click
    this->ignore_next_global_mouse_release = true;
  }
}

}  // namespace Graph
