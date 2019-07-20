/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_graphicsscene.h"

#include <QDebug>

#include <QGraphicsRectItem>
#include <QGraphicsItem>
#include <QPointF>

namespace Graph {

/*
Constructor for GraphicsScene
    :param parent: parent widget
*/
GraphicsScene::GraphicsScene(QWidget* parent) :
    QGraphicsScene(parent),
    settings(),
    item_background(nullptr),
    item_x_axis_title(nullptr),
    item_x_axis_labels(this),
    //item_x_axis_ticks(this),
    item_x_colorbar(nullptr),
    item_x_axis_lines(this),
    item_y_axis_title(nullptr),
    item_y_axis_labels(this),
    //item_y_axis_ticks(this),
    item_y_axis_lines(this),
    item_outline(nullptr)
{
    // Add items to heap, make sure to add to scene for proper memory management
    this->item_background = new Graph::GraphBackground();
    this->item_x_axis_title = new Graph::AxisTitleX(this->settings.x_title);
    this->item_x_colorbar = new Graph::Colorbar();
    this->item_y_axis_title = new Graph::AxisTitleY(this->settings.y_title);
    this->item_outline = new Graph::GraphOutline();

    // Add items to the scene, this gives the scene class the actual ownership
    this->addItem(this->item_background);
    this->addItem(this->item_x_axis_title);
    this->addItem(this->item_x_colorbar);
    this->addItem(this->item_y_axis_title);
    this->addItem(this->item_outline);

    // Startup calculations
    this->item_x_axis_labels.setTicks(this->settings);
    //this->item_x_axis_ticks.setTicks(this->settings);
    this->item_x_axis_lines.setTicks(this->settings);
    this->item_y_axis_labels.setTicks(this->settings);
    //this->item_y_axis_ticks.setTicks(this->settings);
    this->item_y_axis_lines.setTicks(this->settings);
}

/*
Calculates the x sizes of all items of the scene
    :param rect: the rect in which the scene
*/
void GraphicsScene::calculateSizes(const QSize& scene){
    // x values are the rightmost x values, except for x_start
    qreal x_start = 0;
    qreal x_title = x_start + this->item_y_axis_title->minimumWidth();
    qreal x_tick_labels = x_title + this->item_y_axis_labels.minimumWidth();
    //qreal x_tick_lines = x_tick_labels + this->item_y_axis_ticks.minimumWidth();
    qreal x_graph = x_tick_labels;
    qreal x_end = scene.width();
    //qDebug() << "X::" << x_start << ":" << x_title << ":" << x_tick_lines << ":" << x_end;

    // y values are the topmost y values, except for y_start
    qreal y_end = scene.height();
    qreal y_title = y_end - this->item_x_axis_title->minimumHeight();
    qreal y_tick_labels = y_title - this->item_x_axis_labels.minimumHeight();
    //qreal y_tick_lines = y_tick_labels - this->item_x_axis_ticks.minimumHeight(); 
    qreal y_bar = y_tick_labels - this->item_x_colorbar->minimumHeight();
    qreal y_graph = y_bar;
    qreal y_start = 0;
    //qDebug() << "Y::" << y_start << ":" << y_tick_lines << ":" << y_tick_labels << ":" << y_title << ":" << y_end;

    // Give regions to items
    this->item_background->setPosition(QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));

    this->item_y_axis_title->setPosition(QRectF(QPointF(x_start, y_start), QPointF(x_title, y_graph)));
    this->item_y_axis_labels.setPosition(this->settings, QRectF(QPointF(x_title, y_start), QPointF(x_tick_labels, y_graph)));
    //this->item_y_axis_ticks.setPosition(this->settings, QRectF(QPointF(x_tick_lines, y_start), QPointF(x_tick_lines, y_graph)));
    this->item_y_axis_lines.setPosition(this->settings, QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));

    this->item_x_axis_lines.setPosition(this->settings, QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));
    this->item_x_colorbar->setPosition(this->settings, QRectF(QPointF(x_graph, y_bar), QPointF(x_end, y_tick_labels)));
    //this->item_x_axis_ticks.setPosition(this->settings, QRectF(QPointF(x_graph, y_tick_lines), QPointF(x_end, y_tick_labels)));
    this->item_x_axis_labels.setPosition(this->settings, QRectF(QPointF(x_graph, y_tick_labels), QPointF(x_end, y_title)));
    this->item_x_axis_title->setPosition(QRectF(QPointF(x_graph, y_title), QPointF(x_end, y_end)));

    this->item_outline->setPosition(QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));
}

/*
Slot: restructures the scene upon resizing
    :param rect: the rect in which the scene can be drawn
*/
void GraphicsScene::resizeScene(const QSize& scene){
    this->calculateSizes(scene);
}

/*
Constructor: Adds line plotting to GraphicsScene
    :param parent: parent widget
*/
GraphicsPlot::GraphicsPlot(QWidget* parent) :
    GraphicsScene(parent)
{}

/*
Slot: restructures the scene upon resizing
    :param rect: the rect in which the scene can be drawn
*/
void GraphicsPlot::resizeScene(const QSize& scene){
    GraphicsScene::resizeScene(scene);

    // Resize line objects
}


} // Graph namespace
