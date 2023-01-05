/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_controller.h"

#include <QGraphicsView>
#include <QGridLayout>
#include <QPainter>
#include <QString>
#include <QStyle>
#include <QStyleOption>

#include "general_widgets.h"
#include "graph_graphicsscene.h"
#include "graph_graphicsview.h"

namespace Graph {

/*
Initializer: Builds and connects the graph widget
  :parent: parent widget
*/
Controller::Controller(QWidget* parent) : QWidget(parent), graphics_scene(nullptr), graphics_view(nullptr), graphics_style(nullptr) {
  this->setContentsMargins(0, 0, 0, 0);
  this->setMinimumSize(300, 200);

  // Graph components
  this->graphics_scene = new Graph::GraphicsScene(Graph::Format::Settings(), this);
  this->graphics_view = new Graph::GraphicsView(graphics_scene, this);
  this->graphics_style = new Graph::Format::Style(this);

  // Set layout
  QVBoxLayout* controller_layout = new QVBoxLayout(this);
  controller_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(controller_layout);
  controller_layout->addWidget(this->graphics_view);

  // Connect
  QObject::connect(this->graphics_view, &Graph::GraphicsView::resizedView, this->graphics_scene, &Graph::GraphicsScene::resizeScene);
  QObject::connect(this, &Graph::Controller::sendGlobalEvent, this->graphics_view, &Graph::GraphicsView::receiveGlobalEvent);
  QObject::connect(this->graphics_view, &Graph::GraphicsView::globalMouseReleaseEvent, this->graphics_scene,
                   &Graph::GraphicsScene::globalMouseReleaseEvent);

  QObject::connect(this, &Graph::Controller::sendCacheState, this->graphics_scene, &Graph::GraphicsScene::syncSpectra);
  QObject::connect(this, &Graph::Controller::sendCacheUpdate, this->graphics_scene, &Graph::GraphicsScene::updateSpectra);
  QObject::connect(this, &Graph::Controller::sendGraphState, this->graphics_scene, &Graph::GraphicsScene::syncGraphState);

  QObject::connect(this->graphics_scene, &Graph::GraphicsScene::spectrumSelected, this, &Graph::Controller::sendCacheRequestUpdate);
  QObject::connect(this->graphics_scene, &Graph::GraphicsScene::plotSelected, this, &Graph::Controller::receivePlotSelected);

  QObject::connect(this->graphics_style, &Graph::Format::Style::styleChanged, this, &Graph::Controller::receiveStyleChanged);
  QObject::connect(this, &Graph::Controller::sendPainterUpdate, this->graphics_scene, &Graph::GraphicsScene::updatePainter);
}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
  :param event: paintevent
*/
void Controller::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);
  QStyleOption style_option;
  style_option.initFrom(this);
  QPainter painter(this);
  this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Slot: receives global events. This is used to handle selection & closing of menu's correctly
*/
void Controller::receiveGlobalEvent(QEvent* event) { emit this->sendGlobalEvent(event); }

/*
Slot: receives cache sync events for the graph
*/
void Controller::receiveCacheState(const std::vector<Cache::ID>& cache_state) { emit this->sendCacheState(cache_state); }

/*
Slot: receives cache update events for the graph
*/
void Controller::receiveCacheUpdate() { emit this->sendCacheUpdate(); }

/*
Slot: sets the selection state of the graph.
  :param state: the selection state to change into
*/
void Controller::setSelect(bool state) { this->graphics_scene->setPressed(state); }

/*
Slot: receives selection event from GraphicsScene
  :param state: selection (true) or deselection (false)
*/
void Controller::receivePlotSelected(bool state) { emit this->sendGraphSelect(this, state); }

/*
Slot: receives a graph state change event
*/
void Controller::receiveGraphState(const State::GraphState& state) { emit this->sendGraphState(state); }

/*
Slot: receives Graph::Format::Style changes
*/
void Controller::receiveStyleChanged() { emit this->sendPainterUpdate(this->graphics_style); }

/* ################################################################################################ */

/*
Contructor: builds the graph scroll layout scrollarea
*/
ScrollController::ScrollController(QWidget* parent)
    : QScrollArea(parent), graph_widgets(), graph_selected(nullptr), margin_scrollbar(0), columns_max(2), columns(1) {
  // Set Scrollarea properties
  this->setFocusPolicy(Qt::NoFocus);
  this->setWidgetResizable(true);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setFrameStyle(QFrame::NoFrame);

  // Setup widget and layout inside the scrollarea
  QWidget* widget_internal = new QWidget(this);

  QGridLayout* layout_internal = new QGridLayout(this);
  layout_internal->setContentsMargins(0, 0, 0, 0);
  layout_internal->setSpacing(0);
  // layout_internal->insertStretch(INT_MAX, 99);

  widget_internal->setLayout(layout_internal);
  this->setWidget(widget_internal);

  General::ScrollBar* vertical_scrollbar = new General::ScrollBar(this);
  this->setVerticalScrollBar(vertical_scrollbar);

  QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::showing, this,
                   &Graph::ScrollController::showingScrollBar);
  QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::hiding, this,
                   &Graph::ScrollController::hidingScrollBar);

  // Reserve vector space
  this->graph_widgets.reserve(10);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString ScrollController::layoutMarginsScrollBar() const { return QString::number(this->margin_scrollbar, 'f', 0); }

/*
Receives layout scaling properties from the stylesheet
*/
void ScrollController::setLayoutMarginsScrollBar(QString layout_spacing_scroll_bar) {
  this->margin_scrollbar = layout_spacing_scroll_bar.toInt();
  if (this->verticalScrollBar()->isVisible()) {
    this->widget()->layout()->setContentsMargins(0, 0, this->margin_scrollbar, 0);
  } else {
    this->widget()->layout()->setContentsMargins(0, 0, 0, 0);
  }
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString ScrollController::layoutSpacing() const { return QString::number(this->widget()->layout()->spacing(), 'f', 0); }

/*
Receives layout scaling properties from the stylesheet
*/
void ScrollController::setLayoutSpacing(QString layout_spacing) { this->widget()->layout()->setSpacing(layout_spacing.toInt()); }

/*
Receives a viewport resize event. Makes sure the widget can fit in the width allocated
*/
void ScrollController::resizeEvent(QResizeEvent* event) {
  const int min_width = 300;
  int width = event->size().width();

  int columns_n = width / min_width;

  if (columns_n > this->columns_max) {
    columns_n = this->columns_max;
  } else if (columns_n < 1) {
    columns_n = 1;
  }

  // Rebuild if necessary
  if (this->columns != columns_n) {
    this->columns = columns_n;
    this->rebuildLayout();
  }

  QScrollArea::resizeEvent(event);
}

/*
Receives hiding signal from the vertical scrollbar and removes scrollbar margin
*/
void ScrollController::hidingScrollBar() { this->widget()->layout()->setContentsMargins(0, 0, 0, 0); }

/*
Receives showing signal from the vertical scrollbar and adds scrollbar margin
*/
void ScrollController::showingScrollBar() { this->widget()->layout()->setContentsMargins(0, 0, this->margin_scrollbar, 0); }

/*
Adds a graph to the layout
*/
void ScrollController::addGraph() {
  // Calculate new position
  int row = static_cast<int>(this->graph_widgets.size()) / this->columns;
  int col = static_cast<int>(this->graph_widgets.size()) % this->columns;

  // Construct new graph
  Graph::Controller* graph = new Graph::Controller(this);

  // Connect the signals
  QObject::connect(this, &Graph::ScrollController::sendGlobalEvent, graph, &Graph::Controller::receiveGlobalEvent);
  QObject::connect(this, &Graph::ScrollController::sendCacheState, graph, &Graph::Controller::receiveCacheState);
  QObject::connect(this, &Graph::ScrollController::sendCacheUpdate, graph, &Graph::Controller::receiveCacheUpdate);
  QObject::connect(graph, &Graph::Controller::sendCacheRequestUpdate, this, &Graph::ScrollController::receiveCacheRequestUpdate);
  QObject::connect(graph, &Graph::Controller::sendGraphSelect, this, &Graph::ScrollController::receiveGraphSelect);

  this->graph_widgets.push_back(graph);
  static_cast<QGridLayout*>(this->widget()->layout())->addWidget(graph, row, col, 1, 1);
}

/*
Removes the last graph from the layout
*/
void ScrollController::removeGraph() {
  if (this->graph_widgets.empty()) {
    return;
  }

  std::size_t index = this->graph_widgets.size() - 1;
  delete this->graph_widgets[index];
  this->graph_widgets.pop_back();
}

/*
Rebuilds the layout distribution of the graphs. Removes gaps in the layout
*/
void ScrollController::rebuildLayout() {
  // Removes the items from the layout and adds it to the correct location
  QGridLayout* layout = static_cast<QGridLayout*>(this->widget()->layout());

  for (std::size_t index = 0; index < this->graph_widgets.size(); ++index) {
    layout->removeWidget(this->graph_widgets[index]);

    int row = static_cast<int>(index) / this->columns;
    int col = static_cast<int>(index) % this->columns;

    layout->addWidget(this->graph_widgets[index], row, col, 1, 1);
  }
}

/*
Slot: receives global events. This is used to handle selection & closing of menu's correctly
*/
void ScrollController::receiveGlobalEvent(QEvent* event) { emit this->sendGlobalEvent(event); }

/*
Slot: receives cache sync events for the graph
*/
void ScrollController::receiveCacheState(const std::vector<Cache::ID>& cache_state) { emit this->sendCacheState(cache_state); }

/*
Slot: receives cache update events for the graph
*/
void ScrollController::receiveCacheUpdate() { emit this->sendCacheUpdate(); }

/*
Slot: receives the graph adding/removing, and adjust the amount of graphs
*/
void ScrollController::receiveGraphState(std::vector<State::GraphState>& state) {
  // Special case: graph state is empty -> remove all
  if (state.empty()) {
    for (Graph::Controller* item : this->graph_widgets) {
      delete item;
    }
    this->graph_widgets.clear();
    return;
  }

  // Any change of location, will (likely) force a paint/resize call
  // Complicated to detect which GraphState refers to which graphs, especially after graph deletion
  // So, lets keep it simple, and just directly change the Graph::Controller's and do not use std::rotate tricks

  // Set the correct amount of Graph::Controller widgets
  if (this->graph_widgets.size() < state.size()) {
    std::size_t to_add = state.size() - this->graph_widgets.size();
    for (std::size_t i = 0; i < to_add; ++i) {
      this->addGraph();
    }
  } else if (this->graph_widgets.size() > state.size()) {
    std::size_t to_remove = this->graph_widgets.size() - state.size();
    for (std::size_t i = 0; i < to_remove; ++i) {
      this->removeGraph();
    }
  }

  // Now synchronize the state
  for (std::size_t i = 0; i < state.size(); ++i) {
    this->graph_widgets[i]->receiveGraphState(state[i]);
  }
}

/*
Slot: receives cache update requests
*/
void ScrollController::receiveCacheRequestUpdate() { emit this->sendCacheRequestUpdate(); }

/*
Slot: receives graph selection, finds index, and (if found) emits graphselect signal
*/
void ScrollController::receiveGraphSelect(const Controller* graph, bool state) {
  // Find index
  for (std::size_t i = 0; i < this->graph_widgets.size(); ++i) {
    if (graph == this->graph_widgets[i]) {
      qDebug() << "Graph::ScrollController: emits select:" << i << state;

      emit this->sendGraphSelect(i, state);
      return;
    }
  }
}

}  // namespace Graph
