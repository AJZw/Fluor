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
    item_x_axis_label(nullptr),
    item_x_axis_gridlabels(nullptr),
    //item_x_axis_ticks(nullptr),
    item_x_axis_gridlines(nullptr),
    item_x_colorbar(nullptr),
    item_y_axis_label(nullptr),
    item_y_axis_gridlabels(nullptr),
    //item_y_axis_ticks(nullptr),
    item_y_axis_gridlines(nullptr),
    item_spectra(nullptr),
    item_outline(nullptr),
    scroll_count(0),
    size_current()
{
    // Add items to heap, make sure to add to scene for proper memory management
    this->item_background = new Graph::Background();
    this->item_x_axis_label = new Graph::Axis::LabelX(this->settings.x_axis.label);
    this->item_x_axis_gridlabels = new Graph::Axis::GridLabelsX();
    //this->item_x_axis_ticks = new Graph::Axis::TicksX();
    this->item_x_axis_gridlines = new Graph::Axis::GridLinesX();
    
    this->item_x_colorbar = new Graph::Colorbar();

    this->item_y_axis_label = new Graph::Axis::LabelY(this->settings.y_axis.label);
    this->item_y_axis_gridlabels = new Graph::Axis::GridLabelsY();
    //this->item_y_axis_ticks = new Graph::Axis::TicksY();
    this->item_y_axis_gridlines = new Graph::Axis::GridLinesY();

    this->item_spectra = new Graph::Spectra();
    this->item_outline = new Graph::Outline();

    // Add items to the scene, this gives the scene class the actual ownership
    this->addItem(this->item_background);
    this->addItem(this->item_x_axis_label);
    this->addItem(this->item_x_axis_gridlabels);
    // this->addItem(this->item_x_axis_ticks);
    this->addItem(this->item_x_axis_gridlines);

    this->addItem(this->item_x_colorbar);

    this->addItem(this->item_y_axis_label);
    this->addItem(this->item_y_axis_gridlabels);
    // this->addItem(this->item_y_axis_ticks);
    this->addItem(this->item_y_axis_gridlines);

    this->addItem(this->item_spectra);
    this->addItem(this->item_outline);

    // Startup calculations
    this->item_x_axis_gridlabels->setLabels(this->settings);
    //this->item_x_axis_ticks->setLines(this->settings);
    this->item_x_axis_gridlines->setLines(this->settings);
    this->item_y_axis_gridlabels->setLabels(this->settings);
    //this->item_y_axis_ticks->setLines(this->settings);
    this->item_y_axis_gridlines->setLines(this->settings);

}

/*
Destructor: Not really necessary, as addItem() gives ownership of the item to the scene
*/
GraphicsScene::~GraphicsScene(){
    delete this->item_background;
    delete this->item_x_axis_label;
    delete this->item_x_axis_gridlabels;
    //delete this->item_x_axis_ticks;
    delete this->item_x_axis_gridlines;
    
    delete this->item_x_colorbar;

    delete this->item_y_axis_label;
    delete this->item_y_axis_gridlabels;
    //delete this->item_y_axis_ticks;
    delete this->item_y_axis_gridlines;

    delete this->item_spectra;
    delete this->item_outline;
}

/*
Calculates the x sizes of all items of the scene
    :param rect: the rect in which the scene
*/
void GraphicsScene::calculateSizes(const QSize& scene){
    // x values are the rightmost x values, except for x_start
    qreal x_start = 0;
    qreal x_title = x_start + this->item_y_axis_label->minimumWidth();
    qreal x_tick_labels = x_title + this->item_y_axis_gridlabels->minimumWidth();
    //qreal x_tick_lines = x_tick_labels + this->item_y_axis_ticks->minimumWidth();
    qreal x_graph = x_tick_labels;
    qreal x_end = scene.width();
    //qDebug() << "X::" << x_start << ":" << x_title << ":" << x_tick_lines << ":" << x_end;

    // y values are the topmost y values, except for y_start
    qreal y_end = scene.height();
    qreal y_title = y_end - this->item_x_axis_label->minimumHeight();
    qreal y_tick_labels = y_title - this->item_x_axis_gridlabels->minimumHeight();
    //qreal y_tick_lines = y_tick_labels - this->item_x_axis_ticks->minimumHeight(); 
    qreal y_bar = y_tick_labels - this->item_x_colorbar->minimumHeight();
    qreal y_graph = y_bar;
    qreal y_start = 0;
    //qDebug() << "Y::" << y_start << ":" << y_tick_lines << ":" << y_tick_labels << ":" << y_title << ":" << y_end;

    // Give regions to items
    this->item_background->setPosition(QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));

    this->item_y_axis_label->setPosition(QRectF(QPointF(x_start, y_start), QPointF(x_title, y_graph)));
    this->item_y_axis_gridlabels->setPosition(this->settings, QRectF(QPointF(x_title, y_start), QPointF(x_tick_labels, y_graph)));
    //this->item_y_axis_ticks->setPosition(this->settings, QRectF(QPointF(x_tick_lines, y_start), QPointF(x_tick_lines, y_graph)));
    this->item_y_axis_gridlines->setPosition(this->settings, QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));

    this->item_x_axis_gridlines->setPosition(this->settings, QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));
    this->item_x_colorbar->setPosition(this->settings, QRectF(QPointF(x_graph, y_bar), QPointF(x_end, y_tick_labels)));
    //this->item_x_axis_ticks->setPosition(this->settings, QRectF(QPointF(x_graph, y_tick_lines), QPointF(x_end, y_tick_labels)));
    this->item_x_axis_gridlabels->setPosition(this->settings, QRectF(QPointF(x_graph, y_tick_labels), QPointF(x_end, y_title)));
    this->item_x_axis_label->setPosition(QRectF(QPointF(x_graph, y_title), QPointF(x_end, y_end)));

    this->item_spectra->setPosition(this->settings, QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));
    this->item_outline->setPosition(QRectF(QPointF(x_graph, y_start), QPointF(x_end, y_graph)));
    
    
    //qDebug() << this->items();
}

/*
Slot: receives synchronisation requests forwards it to the spectra object
    :param cache_state: the state of the cache
*/
void GraphicsScene::sync(const std::vector<Cache::CacheID>& cache_state){
    this->item_spectra->syncSpectra(cache_state, this->settings);
}

/*
Slot: receives update requests forwards it to the spectra object
    :param cache_state: the state of the cache
*/
void GraphicsScene::update(const std::vector<Cache::CacheID>& cache_state){
    this->item_spectra->updateSpectra(cache_state);
}

/*
Slot: restructures the scene upon resizing
    :param rect: the rect in which the scene can be drawn
*/
void GraphicsScene::resizeScene(const QSize& scene){
    this->size_current = scene;
    this->calculateSizes(scene);
}

/*
Slot: updates the brush and pen for the painter
    :param style: the factory where the brush and pen are requested from
*/
void GraphicsScene::updatePainter(const Graph::Format::Style* style){
    this->setBackgroundBrush(style->brushScene());

    this->item_background->updatePainter(style);
    
    this->item_x_axis_label->updatePainter(style);
    this->item_x_axis_gridlabels->updatePainter(style);
    this->item_x_axis_gridlines->updatePainter(style);
    this->item_x_colorbar->updatePainter(style);
    this->item_y_axis_label->updatePainter(style);
    this->item_y_axis_gridlabels->updatePainter(style);
    this->item_y_axis_gridlines->updatePainter(style);

    this->item_outline->updatePainter(style);

    this->item_spectra->updatePainter(style);

    // Painter updates can change the size requirements of the item
    this->calculateSizes(this->size_current);
}

/*
Implements the mouse press event. Selects a Graph::Spectrum item
    :param event: the mouse press event
*/
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Implements the mouse double click event. Selects a Graph::Spectrum item
    :param event: the mouse double click event
*/
void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Implements the mouse move event handling. Selects a Graph::Spectrum item
    :param event: the mouse move event
*/
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Implements the mouse release event handling. Selects a Graph::Spectrum item
    :param event: the mouse release event
*/
void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);
    this->item_spectra->setSelect(false);
    this->scroll_count = 0;
    emit this->spectrumSelected();
}

/*
Implements the mouse wheel event handling. Select the Graph::Spectrum item selection index.
    :param event: the mouse wheel event
*/
void GraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* event){
    if(event->buttons() == Qt::LeftButton){
        int scroll_move = event->delta() / 120;
        if(scroll_move >= 0){
            this->scroll_count += static_cast<std::size_t>(scroll_move);
        }else{
            this->scroll_count -= static_cast<std::size_t>(scroll_move);
        }
    }
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Finds the Spectrum items (contained in Spectra) that contains the point and selects the curve based on the index
    :param point: the point to contain in scene coordinates
    :param index: if there are multiple items that fit the curve, the index specifies the one to be selected
*/
void GraphicsScene::selectSpectrum(const QPointF& point, std::size_t index) {
    std::vector<Graph::Spectrum*> is_contained = this->item_spectra->containsSpectrum(point);

    // Deselect all
    this->item_spectra->setSelect(false);
    
    if(is_contained.empty()){
        emit this->spectrumSelected();
        return;
    }

    // Calculate the index, while allowing 'rotating' through all the indexes
    index %= is_contained.size();

    Graph::Spectrum* contained_graph = is_contained[index];

    contained_graph->setSelect(true);

    emit this->spectrumSelected();
    return;
}

} // Graph namespace
