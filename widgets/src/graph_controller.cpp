/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_controller.h"
#include "graph_graphicsscene.h"
#include "graph_graphicsview.h"

#include <QVBoxLayout>

#include <QStyleOption>
#include <QPainter>
#include <QStyle>

#include <QGraphicsView>

namespace Graph {

/*
Initializer: Builds and connects the graph widget
    :parent: parent widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent),
    graphics_scene(nullptr),
    graphics_view(nullptr),
    graphics_style(nullptr)
{
    this->setContentsMargins(0, 0, 0, 0);

    // Should contain a scrollwidget but for now just contain the graphics_view + scene
    this->graphics_scene = new Graph::GraphicsScene(this);
    this->graphics_view = new Graph::GraphicsView(graphics_scene, this);
    this->graphics_style = new Graph::Format::Style(this);
    
    // Set layout
    QVBoxLayout* controller_layout = new QVBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(controller_layout);
    controller_layout->addWidget(this->graphics_view);

    // Connect
    QObject::connect(this->graphics_view, &Graph::GraphicsView::resizedView, this->graphics_scene, Graph::GraphicsScene::resizeScene);
    QObject::connect(this, &Graph::Controller::sendCacheSync, this->graphics_scene, &Graph::GraphicsScene::sync);
    QObject::connect(this, &Graph::Controller::sendCacheUpdate, this->graphics_scene, &Graph::GraphicsScene::update);

    QObject::connect(this->graphics_scene, &Graph::GraphicsScene::spectrumSelected, this, &Graph::Controller::sendCacheRequestUpdate);

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
Slot: receives global events
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/* 
Slot: receives cache sync events for the graph
*/
void Controller::receiveCacheSync(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheSync(cache_state);
}

/* 
Slot: receives cache update events for the graph
*/
void Controller::receiveCacheUpdate(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheUpdate(cache_state);
}

/*
Slot: receives Graph::Format::Style changes
*/
void Controller::receiveStyleChanged(){
    emit this->sendPainterUpdate(this->graphics_style);
}

} // Fluor namespace