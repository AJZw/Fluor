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
GraphicsScene::GraphicsScene(Format::Settings settings, QWidget* parent) :
    QGraphicsScene(parent),
    settings(std::move(settings)),
    plot_rect(),
    item_background(nullptr),
    item_x_axis_label(nullptr),
    item_x_axis_gridlabels(nullptr),
    item_x_axis_ticks(nullptr),
    item_x_axis_gridlines(nullptr),
    item_x_colorbar(nullptr),
    item_y_axis_label(nullptr),
    item_y_axis_gridlabels(nullptr),
    item_y_axis_ticks(nullptr),
    item_y_axis_gridlines(nullptr),
    item_spectra(nullptr),
    item_lasers(nullptr),
    item_detectors(nullptr),
    item_outline(nullptr),
    scroll_count(0),
    size_current()
{
    this->plot_rect.setSettings(QRectF(QPointF(this->settings.x_range.begin, this->settings.y_range.begin), QPointF(this->settings.x_range.end, this->settings.y_range.end)));
    
    // Add items to heap, make sure to add to scene for proper memory management
    this->item_background = new Graph::Background();
    
    if(this->settings.enable_labels){this->item_x_axis_label = new Graph::Axis::LabelX(this->settings.x_axis.label);}
    if(this->settings.enable_gridlabels){this->item_x_axis_gridlabels = new Graph::Axis::GridLabelsX();}
    if(this->settings.enable_ticks){this->item_x_axis_ticks = new Graph::Axis::TicksX();}
    if(this->settings.enable_gridlines){this->item_x_axis_gridlines = new Graph::Axis::GridLinesX();}
    
    if(this->settings.enable_colorbar){this->item_x_colorbar = new Graph::Colorbar();}

    if(this->settings.enable_labels){this->item_y_axis_label = new Graph::Axis::LabelY(this->settings.y_axis.label);}
    if(this->settings.enable_gridlabels){this->item_y_axis_gridlabels = new Graph::Axis::GridLabelsY();}
    if(this->settings.enable_ticks){this->item_y_axis_ticks = new Graph::Axis::TicksY();}
    if(this->settings.enable_gridlines){this->item_y_axis_gridlines = new Graph::Axis::GridLinesY();}

    this->item_spectra = new Graph::SpectrumCollection(this->plot_rect);
    this->item_lasers = new Graph::LaserCollection(this->plot_rect);
    this->item_detectors = new Graph::DetectorCollection(this->plot_rect);
    this->item_outline = new Graph::Outline();

    // Add items to the scene, this gives the scene class the actual ownership
    this->addItem(this->item_background);
    
    if(this->settings.enable_labels){this->addItem(this->item_x_axis_label);}
    if(this->settings.enable_gridlabels){this->addItem(this->item_x_axis_gridlabels);}
    if(this->settings.enable_ticks){this->addItem(this->item_x_axis_ticks);}
    if(this->settings.enable_gridlines){this->addItem(this->item_x_axis_gridlines);}

    if(this->settings.enable_colorbar){this->addItem(this->item_x_colorbar);}

    if(this->settings.enable_labels){this->addItem(this->item_y_axis_label);}
    if(this->settings.enable_gridlabels){this->addItem(this->item_y_axis_gridlabels);}
    if(this->settings.enable_ticks){this->addItem(this->item_y_axis_ticks);}
    if(this->settings.enable_gridlines){this->addItem(this->item_y_axis_gridlines);} 

    this->addItem(this->item_spectra);
    this->addItem(this->item_lasers);
    this->addItem(this->item_detectors);
    this->addItem(this->item_outline);

    // Startup calculations
    if(this->settings.enable_gridlabels){this->item_x_axis_gridlabels->setLabels(this->settings);}
    if(this->settings.enable_ticks){this->item_x_axis_ticks->setLines(this->settings);}
    if(this->settings.enable_gridlines){this->item_x_axis_gridlines->setLines(this->settings);}
    if(this->settings.enable_gridlabels){this->item_y_axis_gridlabels->setLabels(this->settings);}
    if(this->settings.enable_ticks){this->item_y_axis_ticks->setLines(this->settings);}
    if(this->settings.enable_gridlines){this->item_y_axis_gridlines->setLines(this->settings);}

    //this->item_lasers->syncLasers({300, 350, 400, 450, 500, 550, 600, 650, 700, 750});
}

/*
Destructor: Not really necessary, as addItem() gives ownership of the item to the scene
*/
GraphicsScene::~GraphicsScene(){
    delete this->item_background;
    
    if(this->settings.enable_labels){delete this->item_x_axis_label;}
    if(this->settings.enable_gridlabels){delete this->item_x_axis_gridlabels;}
    if(this->settings.enable_ticks){delete this->item_x_axis_ticks;}
    if(this->settings.enable_gridlines){delete this->item_x_axis_gridlines;}
    
    if(this->settings.enable_colorbar){delete this->item_x_colorbar;}

    if(this->settings.enable_labels){delete this->item_y_axis_label;}
    if(this->settings.enable_gridlabels){delete this->item_y_axis_gridlabels;}
    if(this->settings.enable_ticks){delete this->item_y_axis_ticks;}
    if(this->settings.enable_gridlines){delete this->item_y_axis_gridlines;}

    delete this->item_spectra;
    delete this->item_lasers;
    delete this->item_detectors;
    delete this->item_outline;
}

/*
Calculates the x sizes of all items of the scene
    :param rect: the rect in which the scene
*/
void GraphicsScene::calculateSizes(const QSize& scene){
    // x values are the rightmost x values, except for x_start
    qreal x_start = 0;
    qreal x_label = this->settings.enable_labels ? x_start + this->item_y_axis_label->minimumWidth() : x_start;
    qreal x_gridlabels = this->settings.enable_gridlabels ? x_label + this->item_y_axis_gridlabels->minimumWidth() : x_label;
    qreal x_ticks = this->settings.enable_ticks ? x_gridlabels + this->item_y_axis_ticks->minimumWidth() : x_gridlabels;
    qreal x_plot = x_ticks;
    qreal x_end = scene.width();

    // y values are the topmost y values, except for y_start
    qreal y_end = scene.height();
    qreal y_label = this->settings.enable_labels ? y_end - this->item_x_axis_label->minimumHeight() : y_end;
    qreal y_gridlabels = this->settings.enable_gridlabels ? y_label - this->item_x_axis_gridlabels->minimumHeight() : y_label;
    qreal y_ticks = this->settings.enable_ticks ? y_gridlabels - this->item_x_axis_ticks->minimumHeight() : y_gridlabels; 
    qreal y_colorbar = this->settings.enable_colorbar ? y_ticks - this->item_x_colorbar->minimumHeight() : y_ticks;
    qreal y_plot = y_colorbar;
    qreal y_start = 0;

    // Sets the plotting area. Must happen first for gridlabels, ticks, gridlines internal item localisation
    this->plot_rect.setLocal(QRectF(QPointF(x_plot, y_start), QPointF(x_end, y_plot)));
    
    // Give regions to items
    this->item_background->setPosition(QRectF(QPointF(x_plot, y_start), QPointF(x_end, y_plot)));

    if(this->settings.enable_labels){
        this->item_y_axis_label->setPosition(QRectF(QPointF(x_start, y_start), QPointF(x_label, y_plot)));
    }
    if(this->settings.enable_gridlabels){
        this->item_y_axis_gridlabels->setPosition(this->plot_rect, QRectF(QPointF(x_label, y_start), QPointF(x_gridlabels, y_plot)));
    }
    if(this->settings.enable_ticks){
        this->item_y_axis_ticks->setPosition(this->plot_rect, QRectF(QPointF(x_gridlabels, y_start), QPointF(x_ticks, y_plot)));
    }
    if(this->settings.enable_gridlines){
        this->item_y_axis_gridlines->setPosition(this->plot_rect, QRectF(QPointF(x_plot, y_start), QPointF(x_end, y_plot)));
    }

    if(this->settings.enable_gridlines){
        this->item_x_axis_gridlines->setPosition(this->plot_rect, QRectF(QPointF(x_plot, y_start), QPointF(x_end, y_plot)));
    }
    if(this->settings.enable_colorbar){
        this->item_x_colorbar->setPosition(this->plot_rect, QRectF(QPointF(x_plot, y_colorbar), QPointF(x_end, y_ticks)));
    }
    if(this->settings.enable_ticks){
        this->item_x_axis_ticks->setPosition(this->plot_rect, QRectF(QPointF(x_plot, y_ticks), QPointF(x_end, y_gridlabels)));
    }
    if(this->settings.enable_gridlabels){
        this->item_x_axis_gridlabels->setPosition(this->plot_rect, QRectF(QPointF(x_plot, y_gridlabels), QPointF(x_end, y_label)));
    }
    if(this->settings.enable_labels){
        this->item_x_axis_label->setPosition(QRectF(QPointF(x_plot, y_label), QPointF(x_end, y_end)));
    }

    this->item_spectra->setPosition();
    this->item_lasers->setPosition();
    this->item_detectors->setPosition();
    
    this->item_outline->setPosition(QRectF(QPointF(x_plot, y_start), QPointF(x_end, y_plot)));

}

/*
Slot: receives synchronisation requests forwards it to the spectra object
    :param cache_state: the state of the cache
*/
void GraphicsScene::sync(const std::vector<Cache::CacheID>& cache_state){
    this->item_spectra->syncSpectra(cache_state);
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
    
    if(this->settings.enable_labels){this->item_x_axis_label->updatePainter(style);}
    if(this->settings.enable_gridlabels){this->item_x_axis_gridlabels->updatePainter(style);}
    if(this->settings.enable_ticks){this->item_x_axis_ticks->updatePainter(style);}
    if(this->settings.enable_gridlines){this->item_x_axis_gridlines->updatePainter(style);}
    if(this->settings.enable_colorbar){this->item_x_colorbar->updatePainter(style);}
    
    if(this->settings.enable_labels){this->item_y_axis_label->updatePainter(style);}
    if(this->settings.enable_gridlabels){this->item_y_axis_gridlabels->updatePainter(style);}
    if(this->settings.enable_ticks){this->item_y_axis_ticks->updatePainter(style);}
    if(this->settings.enable_gridlines){this->item_y_axis_gridlines->updatePainter(style);}

    this->item_spectra->updatePainter(style);
    this->item_lasers->updatePainter(style);
    this->item_detectors->updatePainter(style);

    this->item_outline->updatePainter(style);

    // Painter updates can change the size requirements of the item
    this->calculateSizes(this->size_current);
}

/*
Implements the mouse press event. Selects a Graph::Spectrum item
    :param event: the mouse press event
*/
void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->buttons() != Qt::LeftButton){
        return;
    }
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Implements the mouse double click event. Selects a Graph::Spectrum item
    :param event: the mouse double click event
*/
void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    if(event->buttons() != Qt::LeftButton){
        return;
    }
    this->selectSpectrum(event->scenePos(), this->scroll_count);
}

/*
Implements the mouse move event handling. Selects a Graph::Spectrum item
    :param event: the mouse move event
*/
void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if(event->buttons() != Qt::LeftButton){
        return;
    }
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
        this->selectSpectrum(event->scenePos(), this->scroll_count);
    }
}

/*
Finds the Spectrum items (contained in Spectra) that contains the point and selects the curve based on the index
    :param point: the point to contain in scene coordinates
    :param index: if there are multiple items that fit the curve, the index specifies the one to be selected
*/
void GraphicsScene::selectSpectrum(const QPointF& point, std::size_t index) {
    std::vector<Graph::Spectrum*> is_contained = this->item_spectra->containsItems(this->plot_rect, point);

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
