/**** General **************************************************************
** Version:    v0.9.3
** Date:       2019-04-23
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_graphicsitems.h"

#include <QFont>
#include <QFontMetrics>
#include <QPointF>

#include <QBrush>
#include <QLinearGradient>

#include <QDebug>

namespace Graph {

namespace Axis {

/*
Constructor: abstract class for axis title text labels. Make sure to call calculateMinimumSize() after any size changes.
    :param text: text label
    :param parent: parent
*/
AbstractLabel::AbstractLabel(const QString& text, QGraphicsItem* parent) :
    QGraphicsSimpleTextItem(text, parent),
    item_margins(0, 0, 0, 0),
    minimum_width(0),
    minimum_height(0)
{}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AbstractLabel::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AbstractLabel::margins() const {
    return this->item_margins;
}

/*
Set font
    :param font: the new font
*/
void AbstractLabel::setFont(const QFont& font){
    QGraphicsSimpleTextItem::setFont(font);
}

/*
Set text
    :param text: the new text
*/
void AbstractLabel::setText(const QString& text){
    QGraphicsSimpleTextItem::setText(text);
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int AbstractLabel::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int AbstractLabel::minimumHeight() const {
    return this->minimum_height;
}

/*
Updates the brush used by the painter to the brush as build by the style
    :param style: brush factory
*/
void AbstractLabel::updatePainter(const Graph::Format::Style* style){
    this->setBrush(style->brushLabel());
    this->setFont(style->fontLabel());

    this->calculateMinimumSize();
}

/* ############################################################################################################## */

/*
Constructor: axis title for the x-axis
    :param text: axis label text
    :param parent: parent
*/
LabelX::LabelX(const QString& text, QGraphicsItem* parent) :
    Axis::AbstractLabel(text, parent)
{
    this->setMargins(1, 2, 0, 0);
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void LabelX::calculateMinimumSize() {
    QFontMetrics font_metric = QFontMetrics(this->font());
    
    int width = font_metric.width(this->text());
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.height();
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void LabelX::setPosition(const QRectF& space){
    // Aligns text to middle of space
    QPointF coor_center = space.center();
    qreal x = coor_center.x() - (this->minimum_width * 0.5) + this->item_margins.left();
    qreal y = coor_center.y() - (this->minimum_height * 0.5) + this->item_margins.top();

    this->setPos(x, y);
}

/* ############################################################################################################## */

/*
Constructor: axis title for the y-axis
    :param text: axis label text
    :param parent: parent
*/
LabelY::LabelY(const QString& text, QGraphicsItem* parent) :
    AbstractLabel(text, parent)
{
    this->setMargins(0, 0, 2, 0);
    this->setRotation(-90);
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void LabelY::calculateMinimumSize() {
    QFontMetrics font_metric = QFontMetrics(this->font());
    
    int width = font_metric.height();
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.width(this->text());
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void LabelY::setPosition(const QRectF& space){
    // Aligns text to middle of space
    QPointF coor_center = space.center();
    qreal x = coor_center.x() - (this->minimum_width * 0.5) + this->item_margins.left();
    qreal y = coor_center.y() + (this->minimum_height * 0.5) + this->item_margins.top();

    this->setPos(x, y);
}

/* ############################################################################################################## */

/* 
Constructor: a simple line graphicsitem
    :param location: location in 'real' value (x: nanometers/y: percentage)
    :param parent: parent
*/
GridLine::GridLine(int location, QGraphicsItem* parent) :
    QGraphicsLineItem(parent),
    line_location(location)
{}

/*
Set location attribute
    :param location: value to update location to
*/
void GridLine::setLocation(int location){
    this->line_location = location;
}

/*
Get line location
    :returns: location
*/
int GridLine::location() const {
    return this->line_location;
}

/*
Updates the pen used by the painter to the brush as build by the style
    :param style: brush factory
*/
void GridLine::updatePainter(const Graph::Format::Style* style){
    this->setPen(style->penAxis());
}

/* ############################################################################################################## */

/*
Constructor: Builds the base axis tick/gridline implementation. 
The positioning (calculateMinimumSize / setPosition) and building (setLines) have to be implemented in an inheriting class
Make sure to call calculateMinimumSize() after any size changes.
    :param parent: parent widget
*/
AbstractGridLines::AbstractGridLines(QGraphicsItem* parent) :
    QGraphicsItem(parent),
    items(),
    item_margins(0, 0, 0, 0),
    line_length(0),
    minimum_width(0),
    minimum_height(0)
{}

/*
Container class - doesnt have its own content, so no bounding rectangle
    :returns: bounding rectangle
*/
QRectF AbstractGridLines::boundingRect() const {
    return QRectF(0.0, 0.0, 0.0, 0.0);
}

/*
Container class - doesnt have its own content, so no painting necessary
    :param painter: the painter
    :param option: the style options
    :param widget: (optional) if provided, paints to the widget being painted on
*/
void AbstractGridLines::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    return;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AbstractGridLines::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AbstractGridLines::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AbstractGridLines::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AbstractGridLines::minimumHeight() const {
    return this->minimum_height;
}

/*
Forwards the brush update to its contained GridLabels items
    :param style: brush factory
*/
void AbstractGridLines::updatePainter(const Graph::Format::Style* style){
    for(Graph::Axis::GridLine* label : this->items){
        label->updatePainter(style);
    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
TicksX::TicksX(QGraphicsItem* parent) :
    Axis::AbstractGridLines(parent)
{
    this->setMargins(0, 0, 0, 0);
    this->line_length = 5;

    // Setup done -> calculate minimum size
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the tick + margin size.
*/
void TicksX::calculateMinimumSize() { 
    // Width for x ticks doesnt really make sense, could be the line thickness...
    // int width = this->item_margins.left();
    // width += this->item_margins.right();
    int width = 0;

    int height = this->line_length;
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param settings: the graph settings
    :param space: the allocated space
*/
void TicksX::setPosition(const PlotRectF& plotspace, const QRectF& space){
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    double y_top = space.top() - this->item_margins.top();
    double y_bottom = space.bottom() + this->item_margins.bottom();

    // item vector is not empty, so can safely read first entree
    double pen_width = this->items[0]->pen().widthF();
    pen_width *= 0.5;

    for(GridLine* item : this->items){
        qreal x_item = plotspace.toLocalX(item->location());
        x_item += pen_width;

        item->setLine(x_item, y_top, x_item, y_bottom);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
    :param settings: the graph settings
*/
void TicksX::setLines(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.x_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();

        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.x_ticks.index_begin; i<settings.x_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.x_ticks.ticks[i].location);
        }else{
            // Item is unavailable, add new GridLine QGraphicsLineItem, and take ownership
            GridLine* item = new GridLine(settings.x_ticks.ticks[i].location, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLine*>::difference_type>(tick_count)), this->items.end());
    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
TicksY::TicksY(QGraphicsItem* parent) :
    Axis::AbstractGridLines(parent)
{
    this->setMargins(0, 2, 0, 2);
    this->line_length = 5;

    // Setup done -> calculate minimum size
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void TicksY::calculateMinimumSize() {
    int width = this->line_length;
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void TicksY::setPosition(const PlotRectF& plotspace, const QRectF& space){
    if(this->items.empty()){
        return;
    }
    
    double x_left = space.left() + this->item_margins.left();
    double x_right = space.right() - this->item_margins.right();
    
    // item vector is not empty, so can safely read first entree
    double pen_width = this->items[0]->pen().widthF();
    pen_width *= 0.5;

    for(GridLine* item : this->items){
        qreal y_item = plotspace.toLocalY(item->location());
        y_item += pen_width;

        item->setLine(x_left, y_item, x_right, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void TicksY::setLines(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.y_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();

        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.y_ticks.index_begin; i<settings.y_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.y_ticks.ticks[i].location);
        }else{
            // Item is unavailable, add new TickLine QGraphicsLineItem and take ownership
            GridLine* item = new GridLine(settings.y_ticks.ticks[i].location, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLine*>::difference_type>(tick_count)), this->items.end());

    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
GridLinesX::GridLinesX(QGraphicsItem* parent) :
    Axis::AbstractGridLines(parent)
{
    this->setMargins(0, 1, 0, 1);
    this->setPos(0.0, 0.0);
}

/*
Placeholder, doesnt do anything. Just here for completion sake, minimum size for lines doesnt really make sense...
*/
void GridLinesX::calculateMinimumSize() {
    return;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void GridLinesX::setPosition(const PlotRectF& plotspace, const QRectF& space) {
    Q_UNUSED(space);
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    double y_top = plotspace.local().top() - this->item_margins.top();
    double y_bottom = plotspace.local().bottom() + this->item_margins.bottom();

    // item vector is not empty, so can safely read first entree
    double pen_width = this->items[0]->pen().widthF();
    pen_width *= 0.5;

    for(GridLine* item : this->items){
        qreal x_item = plotspace.toLocalX(item->location());
        x_item += pen_width;

        item->setLine(x_item, y_top, x_item, y_bottom);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
    :param settings: the graph settings
*/
void GridLinesX::setLines(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.x_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();

        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.x_ticks.index_begin; i<settings.x_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.x_ticks.ticks[i].location);
        }else{
            // Item is unavailable, add new GridLine QGraphicsLineItem, and take ownership
            GridLine* item = new GridLine(settings.x_ticks.ticks[i].location, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLine*>::difference_type>(tick_count)), this->items.end());
    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
GridLinesY::GridLinesY(QGraphicsItem* parent) :
    Axis::AbstractGridLines(parent)
{
    this->setMargins(1, 0, 1, 0);
    this->setPos(0.0, 0.0);
}

/*
Placeholder, doesnt do anything. Just here for completion sake, minimum size for lines doesnt really make sense...
*/
void GridLinesY::calculateMinimumSize() {
    return;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param settings: the plot region's settings
    :param space: the allocated space
*/
void GridLinesY::setPosition(const PlotRectF& plotspace, const QRectF& space) {
    Q_UNUSED(space);
    
    if(this->items.empty()){
        return;
    }
    
    double x_left = plotspace.local().left() + this->item_margins.left();
    double x_right = plotspace.local().right() - this->item_margins.right();
    
    // item vector is not empty, so can safely read first entree
    double pen_width = this->items[0]->pen().widthF();
    pen_width *= 0.5;

    for(GridLine* item : this->items){
        qreal y_item = plotspace.toLocalY(item->location());
        y_item += pen_width;

        item->setLine(x_left, y_item, x_right, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void GridLinesY::setLines(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.y_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();

        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.y_ticks.index_begin; i<settings.y_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.y_ticks.ticks[i].location);
        }else{
            // Item is unavailable, add new TickLine QGraphicsLineItem and take ownership
            GridLine* item = new GridLine(settings.y_ticks.ticks[i].location, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLine*>::difference_type>(tick_count)), this->items.end());
    }
}

/* ############################################################################################################## */

/* 
Constructor: construct a label for a specific axis grid/tick location
    :param location: location in 'real' value (x: nanometers/y: percentage)
    :param parent: parent
*/
GridLabel::GridLabel(int location, QString label, QGraphicsItem* parent) :
    QGraphicsSimpleTextItem(parent),
    label_location(location)
{
    this->setText(label);
}

/*
Set location attribute
    :param location: value to update location to
*/
void GridLabel::setLocation(int location){
    this->label_location = location;
}

/*
Get location
    :returns: location
*/
int GridLabel::location() const {
    return this->label_location;
}

/*
Updates the brush used by the painter to the brush as build by the style
    :param style: brush factory
*/
void GridLabel::updatePainter(const Graph::Format::Style* style){
    this->setBrush(style->brushGridLabel());
    this->setFont(style->fontGridLabel());
}

/* ############################################################################################################## */

/*
Constructor: Abstract class for grid/tick labels
The positioning (calculateMinimumSize / setPosition) and building (setLines) have to be implemented in an inheriting class
Make sure to call calculateMinimumSize() after any size changes.
*/
AbstractGridLabels::AbstractGridLabels(QGraphicsItem* parent) : 
    QGraphicsItem(parent),
    items(),
    item_margins(0, 0, 0, 0),
    space_offset(0),
    minimum_width(0),
    minimum_height(0)
{}

/*
Container class, doesnt contain its own object, so bounding rectangle is empty
    :returns: empty QRectF
*/
QRectF AbstractGridLabels::boundingRect() const {
    return QRectF(0.0, 0.0, 0.0, 0.0);
}

/*
Container class, doesnt contain its own object to paint.
    :param painter: the painter
    :param option: the style options
    :param widget: (optional) if provided, paints to the widget being painted on
*/
void AbstractGridLabels::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    return;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AbstractGridLabels::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AbstractGridLabels::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AbstractGridLabels::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AbstractGridLabels::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the font on all internal labels
    :param font: the new font
*/
void AbstractGridLabels::setFont(const QFont& font){
    for(Axis::GridLabel* label : this->items){
        label->setFont(font);
    }
}

/*
Forwards the brush update to its contained GridLabels items
    :param style: brush factory
*/
void AbstractGridLabels::updatePainter(const Graph::Format::Style* style){
    for(Graph::Axis::GridLabel* label : this->items){
        label->updatePainter(style);
    }
    this->calculateMinimumSize();
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
GridLabelsX::GridLabelsX(QGraphicsItem* parent) :
    Axis::AbstractGridLabels(parent)
{
    this->setMargins(1, 0, 0, 0);
    this->space_offset = 1;

    this->setPos(0.0, 0.0);
}

/*
Calculates the minimum size of the item, aka the text + margin size
As the ticks are in order, the highest / longest label is likely the final entree
This saves having to check each tick individually, should be changed if the text differs too much from this
*/
void GridLabelsX::calculateMinimumSize() {
    if(this->items.empty()){
        this->minimum_width = 0;
        this->minimum_height = 0;
        return;
    }

    GridLabel* last_label = this->items[this->items.size() -1];
    
    QFontMetrics font_metric = QFontMetrics(last_label->font());

    int width = font_metric.width(last_label->text());
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.height();
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void GridLabelsX::setPosition(const PlotRectF& plotspace, const QRectF& space){
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }

    qreal y_item = (space.height() * 0.5) + space.top(); 
    y_item -= (this->minimum_height * 0.5) + this->margins().top();

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, leave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());

    for(GridLabel* item : this->items){
        // First calculate were the text item's middle should end up
        qreal x_item = plotspace.toLocalX(item->location());

        // Now calculate the offset to center the text on that position
        int item_width = font_metric.width(item->text());
        x_item -= (item_width * 0.5);
        x_item += this->margins().left();
        x_item -= this->margins().right();

        // Make sure the x coordinate doesnt go outside of the assigned space
        x_item = std::max(space.left() - this->space_offset, x_item);
        x_item = std::min(space.right() - item_width, x_item);

        item->setPos(x_item, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void GridLabelsX::setLabels(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.x_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();
        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.x_ticks.index_begin; i<settings.x_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        if(settings.x_ticks.ticks[i].label.isNull()){
            continue;
        }

        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.x_ticks.ticks[i].location);
            this->items[tick_count-1]->setText(settings.x_ticks.ticks[i].label);
        }else{
            // Item is unavailable, add new TickLine QGraphicsLineItem and give ownership to self
            Axis::GridLabel* item = new Axis::GridLabel(settings.x_ticks.ticks[i].location, settings.x_ticks.ticks[i].label, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLabel*>::difference_type>(tick_count)), this->items.end());
    }

    // In the case of the labels we also need to calculate the minimum text width as that can change with the ticks
    this->calculateMinimumSize();
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
GridLabelsY::GridLabelsY(QGraphicsItem* parent) :
    AbstractGridLabels(parent)
{
    this->setMargins(0, 0, 2, 0);
    this->space_offset = 3;
    this->setPos(0.0, 0.0);
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void GridLabelsY::calculateMinimumSize() { 
    if(this->items.empty()){
        this->minimum_width = 0;
        this->minimum_height = 0;
        return;
    }

    Axis::GridLabel* label = this->items[this->items.size() - 1];

    QFontMetrics font_metric = QFontMetrics(label->font());

    int width = font_metric.width(label->text());
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.height();
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param settings: the graph settings
    :param space: the allocated space
*/
void GridLabelsY::setPosition(const PlotRectF& plotspace, const QRectF& space){
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal x_right = space.right() - this->margins().right();

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, leave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());
    int font_height = font_metric.height(); 

    for(GridLabel* item : this->items){
        qreal y_item = plotspace.toLocalY(item->location());
        y_item -= (font_height * 0.5);
        // gridline pen width correction & correction to fix disalignment with gridlines
        y_item += 0.5;

        // Make sure the y coordinate doesnt go outside of the assigned space
        y_item = std::max(0.0 - this->space_offset, y_item);
        y_item = std::min(space.height() - font_height + this->space_offset, y_item);

        // Calculate x so that the text is aligned to the right
        int font_width = font_metric.width(item->text());
        qreal x_item = x_right - font_width;

        item->setPos(x_item, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void GridLabelsY::setLabels(const Graph::Format::Settings& settings) {
    // Check if tick indexes are valid, if not clear ticks
    if(!settings.y_ticks.valid){
        // Delete all Tick GraphicItems from graphicsscene
        for(QGraphicsItem* item : this->items){
            // Destructor of QGraphicsItem removes the item from the scene, and the items children
            delete item;
        }

        // Erase pointers from vector
        this->items.clear();

        return;
    }

    std::size_t tick_count = 0;
    for(std::size_t i=settings.y_ticks.index_begin; i<settings.y_ticks.index_end; ++i){
        // Check if it needs a new item, in this case every Tick needs a item
        if(settings.y_ticks.ticks[i].label.isNull()){
            continue;
        }

        // Use counter to keep track of how many items I need
        ++tick_count;

        // Check if we need to build a new item or can reuse a previous one
        if(tick_count <= this->items.size()){
            // Item is available, so edit Tick
            this->items[tick_count-1]->setLocation(settings.y_ticks.ticks[i].location);
            this->items[tick_count-1]->setText(settings.y_ticks.ticks[i].label);
        }else{
            // Item is unavailable, add new GridLabel QGraphicsLineItem, and take ownership
            GridLabel* item = new GridLabel(settings.y_ticks.ticks[i].location, settings.y_ticks.ticks[i].label, this);

            // Add pointer to vector
            this->items.push_back(item);
        }
    }

    // Now check if the scene contains too many items
    if(tick_count < this->items.size()){
        // Deconstruct items from scene
        for(std::size_t i=tick_count; i<this->items.size(); ++i){
            delete this->items[i];
        }

        // Erase pointers from vector
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<GridLabel*>::difference_type>(tick_count)), this->items.end());
    }

    // In the case of the labels we also need to calculate the minimum text width as that can change with the ticks
    this->calculateMinimumSize();
}

} // Axis namespace

/* ############################################################################################################## */

/*
Constructor: background of the actual graph area
    :param parent: parent
*/
Background::Background(QGraphicsItem* parent) :
    QGraphicsRectItem(parent),
    item_margins(1, 1, 1, 0),
    minimum_width(0),
    minimum_height(0)
{
    this->setPen(Qt::NoPen);
    this->setBrush(Qt::NoBrush);
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void Background::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& Background::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int Background::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int Background::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void Background::setPosition(const QRectF& space){
    this->setRect(space.marginsRemoved(this->margins()));
}

/*
Updates the brush used by the painter to the brush as build by the style
    :param style: brush factory
*/
void Background::updatePainter(const Graph::Format::Style* style){
    this->setBrush(style->brushBackground());
}

/* ############################################################################################################## */

/*
Constructor: outline of the graph area
    :param parent: parent
*/
Outline::Outline(QGraphicsItem* parent) :
    QGraphicsRectItem(parent),
    item_margins(0, 0, 1, 0),
    minimum_width(0),
    minimum_height(0),
    is_hover(false),
    is_pressed(false),
    is_selected(false),
    pen_default(Qt::NoPen),
    pen_hover(Qt::NoPen),
    pen_pressed(Qt::NoPen)
{
    // Create empty brush to not fill the background
    QBrush item_brush(Qt::NoBrush);
    this->setBrush(item_brush);
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void Outline::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& Outline::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int Outline::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int Outline::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void Outline::setPosition(const QRectF& space){
    this->setRect(space.marginsRemoved(this->margins()));
}

/*
Updates the pen used by the painter to the pen as build by the style
    :param style: pen factory
*/
void Outline::updatePainter(const Graph::Format::Style* style){
    this->pen_default = style->penAxis();
    this->pen_hover = style->penAxisHover();
    this->pen_pressed = style->penAxisPress();

    if(this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Set hover state of the outline
    :param hover: the state to be set
*/
void Outline::setHover(bool hover){
    this->is_hover = hover;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Returns the outline's pressed state
*/
bool Outline::isHover() const {
    return this->is_hover;
}

/*
Set pressed state of the outline
    :param press: the state to be set
*/
void Outline::setPressed(bool press){
    this->is_pressed = press;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Return the outline's pressed state
*/
bool Outline::isPressed() const {
    return this->is_pressed;
}

/*
Set selection state of the outline
    :param press: the state to be set
*/
void Outline::setSelected(bool select){
    this->is_selected = select;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Return the outline's pressed state
*/
bool Outline::isSelected() const {
    return this->is_selected;
}

/* ############################################################################################################## */

/*
Constructor: outline of the graph area
    :param parent: parent
*/
Colorbar::Colorbar(QGraphicsItem* parent) :
    QGraphicsRectItem(parent),
    item_margins(0, 0, 1, 0),
    minimum_width(0),
    minimum_height(10),
    is_hover(false),
    is_pressed(false),
    is_selected(false),
    gradient(),
    pen_default(Qt::NoPen),
    pen_hover(Qt::NoPen),
    pen_pressed(Qt::NoPen)
{
    // Fill gradient with correct colors
    // Padded on both ends with black for proper black spread
    this->gradient.setColorAt(0.0, QColor(0, 0, 0));
    this->gradient.setColorAt(0.00001, QColor(77, 0, 77));
    this->gradient.setColorAt(0.1, QColor(85, 0, 255));
    this->gradient.setColorAt(0.15, QColor(0, 0, 255));
    this->gradient.setColorAt(0.275, QColor(0, 255, 255));
    this->gradient.setColorAt(0.325, QColor(0, 255, 0));
    this->gradient.setColorAt(0.5, QColor(255, 255, 0));
    this->gradient.setColorAt(0.6625, QColor(255, 0, 0));
    this->gradient.setColorAt(0.8, QColor(255, 0, 0));
    this->gradient.setColorAt(0.99999, QColor(77, 0, 0));
    this->gradient.setColorAt(1.0, QColor(0, 0, 0));

    this->setBrush(QBrush(this->gradient));
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void Colorbar::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& Colorbar::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int Colorbar::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int Colorbar::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void Colorbar::setPosition(const PlotRectF& settings, const QRectF& space){
    double x_380 = settings.toLocalX(380);
    double x_780 = settings.toLocalX(780);

    this->gradient.setStart(QPointF(x_380, space.top()));
    this->gradient.setFinalStop(QPointF(x_780, space.top()));
    this->setBrush(QBrush(this->gradient));
    
    this->setRect(space.marginsRemoved(this->margins()));
}

/*
Updates the pen used by the painter to the pen as build by the style
    :param style: pen factory
*/
void Colorbar::updatePainter(const Graph::Format::Style* style){
    this->pen_default = style->penAxis();
    this->pen_hover = style->penAxisHover();
    this->pen_pressed = style->penAxisPress();

    if(this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Set hover state of the colorbar border
    :param hover: the state to be set
*/
void Colorbar::setHover(bool hover){
    this->is_hover = hover;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Returns the colorbar border's pressed state
*/
bool Colorbar::isHover() const {
    return this->is_hover;
}

/*
Set pressed state of the colorbar border
    :param press: the state to be set
*/
void Colorbar::setPressed(bool press){
    this->is_pressed = press;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Return the colorbar border's pressed state
*/
bool Colorbar::isPressed() const {
    return this->is_pressed;
}

/*
Sets the selection state of the colorbar border
    :param select: the state to be set
*/
void Colorbar::setSelected(bool select){
    this->is_selected = select;

    if(this->is_selected || this->is_pressed){
        this->setPen(this->pen_pressed);
    }else if(this->is_hover){
        this->setPen(this->pen_hover);
    }else{
        this->setPen(this->pen_default);
    }
}

/*
Returns the colorbar border's selection state
*/
bool Colorbar::isSelected() const {
    return this->is_selected;
}

/* ############################################################################################################## */

/*
Constructor: builds a Spectrum - contains two QPolygonF curves. Handles all drawing of this curve
    :param data: source data, the curves uses this as base for most calculations
    :param parent: parent
*/
Spectrum::Spectrum(Data::CacheSpectrum& data, QGraphicsItem* parent) :
    QGraphicsItem(parent),
    spectrum_source(data),
    spectrum_excitation(this->spectrum_source.spectrum().excitation()),
    spectrum_emission(this->spectrum_source.spectrum().emission()),
    spectrum_emission_fill(this->spectrum_source.spectrum().emission()),
    spectrum_space(0.0, 0.0, 0.0, 0.0),
    visible_excitation(true),
    visible_emission(true),
    select_excitation(false),
    select_emission(false),
    pen_excitation(Qt::NoPen),
    pen_emission(Qt::NoPen),
    brush_emission(Qt::NoBrush),
    pen_excitation_select(Qt::NoPen),
    pen_emission_select(Qt::NoPen),
    brush_emission_select(Qt::NoBrush),
    intensity_coefficient(1.0)
{
    this->setPos(0.0, 0.0);
}

/*
Returns the bounding rectangle of the entire plotting region, might need to reconsider this one
    :returns: bounding rectangle
*/
QRectF Spectrum::boundingRect() const {
    return this->spectrum_space;
}

/*
Paints the excitation and emission curve
    :param painter: the painter
    :param option: (unused) the style options
    :param widget: (unused) if provided, points to the widget being painted on
*/
void Spectrum::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    
    if(this->visible_excitation){
        if(this->select_excitation){
            painter->setPen(this->pen_excitation_select);
        }else{
            painter->setPen(this->pen_excitation);
        }
        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(this->spectrum_excitation.polygon());
    }

    if(this->visible_emission){
        if(this->select_emission){
            painter->setPen(this->pen_emission_select);
        }else{
            painter->setPen(this->pen_emission);
        }
        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(this->spectrum_emission.polygon());

        painter->setPen(Qt::NoPen);
        if(this->select_emission){
            painter->setBrush(this->brush_emission_select);
        }else{
            painter->setBrush(this->brush_emission);
        }
        painter->drawPolygon(this->spectrum_emission_fill.polygon());
    }

    painter->restore();
}

/*
Function is called to determine if this Spectrum is underneath the cursor.
Note: this function is scaling unaware, so in the end a binary search is used for lookup.
    :param point: point to determine if it is contained in the object in scene coordinates.
    :returns: true if the item contains point, otherwise false is returned
*/
bool Spectrum::contains(const QPointF& point) const {
    // Quick check is it within the boundingRect?
    if(!this->spectrum_space.contains(this->mapFromScene(point))){
        return false;
    }

    // Within bounding box, now determine if it is on or below the excitation/emission curves
    if(this->visible_excitation){
        if(this->spectrum_excitation.contains(this->mapFromScene(point), this->pen_excitation.widthF())){
            return true;
        }
    }

    if(this->visible_emission){
        return this->spectrum_emission.contains(this->mapFromScene(point), this->pen_emission.widthF());
    }

    return false;
}

/*
Function is called to determine if this Spectrum is underneath the cursor.
    :param point: point to determine if it is contained in the object in scene coordinates.
    :returns: true if the item contains point, otherwise false is returned
*/
bool Spectrum::contains(const PlotRectF& space, const QPointF& point) const {
    // Quick check is it within the boundingRect?
    if(!this->spectrum_space.contains(this->mapFromScene(point))){
        return false;
    }

    // Within bounding box, now determine if it is on or below the excitation/emission curves
    if(this->visible_excitation){
        if(this->spectrum_excitation.contains(this->mapFromScene(point), this->pen_excitation.widthF(), space.toGlobalXFunction())){
            return true;
        }
    }

    if(this->visible_emission){
        return this->spectrum_emission.contains(this->mapFromScene(point), this->pen_emission.widthF(), space.toGlobalXFunction());
    }

    return false;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and minimumHeight.
    :param space: the allocated space
*/
void Spectrum::setPosition(const PlotRectF& space){
    // Set bounding region
    // Bounding box can be more precise if we check the left and right values after scaling
    if(space.local() != this->spectrum_space){
        this->spectrum_space = space.local();
        this->prepareGeometryChange();
    }

    // Correct plotting space for line width
    QRectF plot_space = this->spectrum_space;
    double pen_adjust = this->pen_excitation.widthF() * 0.5;
    plot_space.adjust(pen_adjust, pen_adjust, -pen_adjust, -pen_adjust);

    // Scale excitation first
    this->spectrum_excitation.scale(
        this->spectrum_source.spectrum().excitation(),
        plot_space,
        space.toLocalXFunction(),
        space.toLocalYFunction(),
        1.0
    );

    // Correct plotting space for line width
    plot_space = this->spectrum_space;
    pen_adjust = this->pen_emission.widthF() * 0.5;
    plot_space.adjust(pen_adjust, pen_adjust, -pen_adjust, -pen_adjust);
    // Scale emission second
    this->spectrum_emission.scale(
        this->spectrum_source.spectrum().emission(),
        plot_space,
        space.toLocalXFunction(),
        space.toLocalYFunction(),
        this->intensity_coefficient
    );

    // Copy and close emission data into fill
    this->spectrum_emission_fill.copyCurve(this->spectrum_emission);
    this->spectrum_emission_fill.closeCurve(this->spectrum_space);
}

/*
Update the internal state to the source state
*/
void Spectrum::updateSpectrum() {
    this->visible_excitation = this->spectrum_source.visibleExcitation();
    this->visible_emission = this->spectrum_source.visibleEmission();
    this->select_excitation = this->spectrum_source.selectExcitation();
    this->select_emission = this->spectrum_source.selectEmission();
    
    // Now plot the updated curve
    this->update(this->boundingRect());
}

/*
Updates the emission intensity to the excitation efficiency
    :param lasers: the lasers to use for efficiency calculation
*/
void Spectrum::updateIntensity(const std::vector<Data::Laser>& lasers){
    if(lasers.empty()){
        this->intensity_coefficient = 1.0;
        return;
    }

    double intensity = 0.0;
    for(const Data::Laser& laser : lasers){
        intensity += (this->spectrum_source.excitationAt(laser.wavelength()) * 0.01);
    }

    if(intensity < this->spectrum_source.intensityCutoff()){
        intensity = 0.0;
    }

    this->intensity_coefficient = intensity;
}

/*
Updates the pen's and brushes used by the painter
    :param style: pen factory
*/
void Spectrum::updatePainter(const Graph::Format::Style* style){
    if(this->spectrum_source.absorptionFlag()){
        this->pen_excitation = style->penAbsorption(this->spectrum_emission.color());
        this->pen_emission_select = style->penAbsorptionSelect(this->spectrum_emission.color());
    }else{
        this->pen_excitation = style->penExcitation(this->spectrum_emission.color());
        this->pen_excitation_select = style->penExcitationSelect(this->spectrum_emission.color());
    }
    this->pen_emission = style->penEmission(this->spectrum_emission.color());
    this->pen_emission_select = style->penEmissionSelect(this->spectrum_emission.color());

    this->brush_emission = style->brushEmission(this->spectrum_emission.color());
    this->brush_emission_select = style->brushEmissionSelect(this->spectrum_emission.color());
}

/*
Getter for the source data
    :returns: pointer to source data
*/
Data::CacheSpectrum& Spectrum::source() const {
    return this->spectrum_source;
}

/*
Sets selection state of the curves
    :param select: the state to change into
*/
void Spectrum::setSelect(bool selection) {
    this->select_excitation = selection;
    this->select_emission = selection;
    this->source().setSelectExcitation(selection);
    this->source().setSelectEmission(selection);
}

/* ############################################################################################################## */

/*
Constructor: constructs a graphicsitem that represents a laser in the scene.
    :param wavelength: the wavelength the laser represents
    :param parent: parent widget
*/
Laser::Laser(QGraphicsItem* parent) :
    QGraphicsLineItem(parent),
    laser_wavelength(0.0)
{
    this->setPos(0.0, 0.0);
}

/*
Constructor: constructs a graphicsitem that represents a laser in the scene.
    :param wavelength: the wavelength the laser represents
    :param parent: parent widget
*/
Laser::Laser(double wavelength, QGraphicsItem* parent) :
    QGraphicsLineItem(parent),
    laser_wavelength(wavelength)
{
}

/*
Setter: sets the wavelength of the laser
    :param wavelength: the wavelength to set
*/
void Laser::setWavelength(double wavelength){
    this->laser_wavelength = wavelength;
}

/*
Getter: gets the wavelength
    :returns: the wavelength of the laser
*/
double Laser::wavelength() const {
    return this->laser_wavelength;
}

/*
Optimized contain function. Only needs to check the x-location of the point
    :param point: point to determine if it is contained in the object in scene coordinates.
    :returns: true if the item contains point, otherwise false is returned
*/
bool Laser::contains(const QPointF& point) const {
    QPointF point_local = this->mapFromScene(point);
    
    double pen_width = this->pen().widthF() * 0.5;

    double left = this->line().x1() - pen_width;
    double right = this->line().x1() + pen_width;

    if(point_local.x() >= left && point_local.x() <= right){
        return true;
    }else{
        return false;
    }
}

/*
Update the painter's pen based on the graphs style
    :param style: the graphs style
*/
void Laser::updatePainter(const Graph::Format::Style* style){
    this->setPen(style->penLaser(Data::Polygon::visibleSpectrum(this->laser_wavelength)));
}

/*
Sets the position of the Laser object based on the settings of the graph within the alloted space
    :param space: the plotting region of the graph
*/
void Laser::setPosition(const PlotRectF& space){
    if(this->laser_wavelength < space.global().left() || this->laser_wavelength > space.global().right()){
        // wavelength is outside of plotting range, so line is invisible
        this->setVisible(false);
    }else{
        // Make sure it is visible
        this->setVisible(true);
    }

    double x_pos = space.toLocalX(this->laser_wavelength);

    this->setLine(x_pos, space.local().top(), x_pos, space.local().bottom());
}

/* ############################################################################################################## */

/*
Constructor: default constructor, builds a valid but empty Filter object
*/
Filter::Filter(QGraphicsItem* parent) :
    QGraphicsItem(parent),
    wavelength_left(0.0),
    wavelength_right(0.0),
    item_left(),
    item_right(),
    item_top(),
    detector_space()
{
    this->item_top.reserve(30);
}

/*
Constructor: constructs a Filter object. Still needs the position to be set before it can be plotted correctly
*/
Filter::Filter(double wavelength_left, double wavelength_right, QGraphicsItem* parent) :
    QGraphicsItem(parent),
    wavelength_left(wavelength_left),
    wavelength_right(wavelength_right),
    item_left(),
    item_right(),
    item_top(),
    detector_space()
{
    this->item_top.reserve(30);
}

/*
Returns the bounding rectangle of the entire plotting region
    :returns: bounding rectangle
*/
QRectF Filter::boundingRect() const {
    return this->detector_space;
}

/*
Paints the Filter lines/curve
    :param painter: the painter
    :param option: (unused) the style options
    :param widget: (unused) if provided, points to the widget being painted on
*/
void Filter::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if(!this->item_left.isNull()){
        painter->setPen(this->pen_left);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(this->item_left);
    }

    if(!this->item_right.isNull()){
        painter->setPen(this->pen_right);
        painter->setBrush(Qt::NoBrush);
        painter->drawLine(this->item_right);
    }

    if(!this->item_top.isEmpty()){
        painter->setPen(this->pen_top);
        painter->setBrush(Qt::NoBrush);
        painter->drawPolyline(this->item_top);
    }

    painter->restore();
}

/*
Optimized contain function. Only needs to check the x-location of the point
    :param point: point to determine if it is contained in the object in scene coordinates.
    :returns: true if the item contains point, otherwise false is returned
*/
bool Filter::contains(const QPointF& point) const {
    QPointF point_local = this->mapFromScene(point);
    
    if(this->item_top.empty()){
        return false;
    }

    //double pen_width = this->pen().widthF() * 0.5;
    double left = this->item_top[0].x(); //- pen_width;
    double right = this->item_top[this->item_top.size()-1].x(); // + pen_width;

    if(point_local.x() >= left && point_local.x() <= right){
        return true;
    }else{
        return false;
    }
}

/*
Sets the position of the Filter object based on the settings of the graph within the alloted space
    :param settings: the graphs settings
    :param space: the plotting region of the graph
*/
void Filter::setPosition(const PlotRectF& space){
    // Set bounding region
    // Bounding box can be more precise if we check the left and right values after scaling
    if(space.local() != this->detector_space){
        this->detector_space = space.local();
        this->prepareGeometryChange();
    }
    
    // Parameters for the bevels
    double bevel_size_y = 10;
    double bevel_size_x = 10;
    std::array<double, 15> math_sin = {0.00, 0.00, 0.01, 0.03, 0.08, 0.13, 0.21, 0.29, 0.39, 0.50, 0.62, 0.74, 0.87, 0.93, 1.00};
    std::array<double, 15> math_cos = {0.00, 0.07, 0.13, 0.26, 0.38, 0.50, 0.61, 0.71, 0.79, 0.87, 0.92, 0.97, 0.99, 1.00, 1.00};
    double offset_pen = 0.5 * this->pen_top.widthF();
    double offset = offset_pen + 1 ; // space.margins().top() <- other option

    // Get boundaries 
    double left = space.toLocalX(this->wavelength_left);
    double right = space.toLocalX(this->wavelength_right);

    // If out of bounds empty the internal items
    if(left > space.local().right() || right < space.local().left()){
        this->item_left = QLineF();
        this->item_right = QLineF();
        this->item_top.clear();
        return;
    }

    // Correct bevel size (if necessary)
    double width = right - left;
    if(width < (2 * bevel_size_x)){
        if(this->bevel_left == BevelShape::Round && this->bevel_right == BevelShape::Round){
            bevel_size_x = (width) * 0.5;
        }else{
            bevel_size_x = std::min(bevel_size_x, (width));
        }
    }
    // Technically the y could be too short to show a full quarter circle, but a graph that small has more issues then just this

    // Calculate the left and right lines (if applicable)
    if(left > space.local().left()){
        switch(this->bevel_left){
        case BevelShape::Square:
            this->item_left = QLineF(left, space.local().bottom(), left, space.local().top() + offset + offset_pen);
            break;
        case BevelShape::Round:
            this->item_left = QLineF(left, space.local().bottom(), left, space.local().top() + bevel_size_y + offset);
            break;
        default:
            qFatal("Detector::setPosition: Unknown Left BevelShape");
        }
    }else{
        this->item_left = QLineF();
    }

    if(right < space.local().right()){
        switch(this->bevel_right){
        case BevelShape::Square:
            this->item_right = QLineF(right, space.local().bottom(), right, space.local().top() + offset + offset_pen);
            break;
        case BevelShape::Round:
            this->item_right = QLineF(right, space.local().bottom(), right, space.local().top() + bevel_size_y + offset);
            break;
        default:
            qFatal("Detector::setPosition: Unknown Right BevelShape");
        }
    }else{
        this->item_right = QLineF();
    }

    // Now construct top polygon
    // Size the QPolygonF to proper size
    if(this->bevel_left == BevelShape::Round && this->bevel_right == BevelShape::Round){
        this->item_top.resize(30);
    }else if(this->bevel_left == BevelShape::Square && this->bevel_right == BevelShape::Square){
        this->item_top.resize(2);
    }else{
        this->item_top.resize(16);
    }

    // Now in place construct corners
    int index = 0;
    switch(this->bevel_left){
    case BevelShape::Square:{
        this->item_top[0].setX(left - offset_pen);
        this->item_top[0].setY(space.local().top() + offset);
        index += 1;
        break;
    }
    case BevelShape::Round:{
        for(std::size_t i=0; i<15; ++i){
            double x = math_sin[i];
            x *= bevel_size_x;
            x += left;

            this->item_top[index].setX(x);

            double y = math_cos[i];
            y *= bevel_size_y;
            y = space.local().top() + bevel_size_y - y + offset;

            this->item_top[index].setY(y);
            index += 1;
        }
        break;
    }
    default:
        qFatal("Detector::setPosition: Unknown Left BevelShape");
    }

    switch(this->bevel_right){
    case BevelShape::Square:{
        this->item_top[index].setX(right + offset_pen);
        this->item_top[index].setY(space.local().top() + offset);
        index += 1;
        break;
    }
    case BevelShape::Round:{
        for(std::size_t i=15; i>0; --i){
            double x = math_sin[i-1];
            x *= bevel_size_x;
            x = right - x;

            this->item_top[index].setX(x);

            double y = math_cos[i-1];
            y *= bevel_size_y;
            y = space.local().top() + bevel_size_y - y + offset;

            this->item_top[index].setY(y);
            index += 1;
        }
        break;
    }
    default:
        qFatal("Detector::setPosition: Unknown Right BevelShape");
    }

    // Check for out of bounds (x), we can check left/right to find out without comparison
    if(this->item_left.isNull()){
        for(int i=0; i < this->item_top.size(); ++i){
            if(this->item_top[i].x() > space.local().left() + offset_pen){
                // set last out of bound point to just inbound 
                // Of note, now is independent from pensize, so maybe add that later
                this->item_top[i-1].setX(space.local().left() + offset_pen);

                auto clear_to = this->item_top.begin();
                clear_to += i-1;
                this->item_top.erase(this->item_top.begin(), clear_to);

                break;
            }
        }
    }

    if(this->item_right.isNull()){
        for(int i=this->item_top.size()-1; i<=0; --i){
            if(this->item_top[i].x() > space.local().right() - offset_pen){
                this->item_top[i+1].setX(space.local().right() - offset_pen);

                auto clear_from = this->item_top.begin();
                clear_from += i+1;
                this->item_top.erase(clear_from, this->item_top.end());

                break;
            }
        }
    }

}

/*
Update the painter's pen based on the graphs style
    :param style: the graphs style
*/
void Filter::updatePainter(const Graph::Format::Style* style){
    this->pen_left = style->penFilter(this->style_left);
    this->pen_right = style->penFilter(this->style_right);
    this->pen_top = style->penFilter(Qt::SolidLine);
}

/*
Returns the wavelenght of the left (minimum) most wavelength 
*/
double Filter::wavelengthLeft() const {
    return this->wavelength_left;
}

/*
Return the wavelenght of the right (maximum) most wavelength
*/
double Filter::wavelengthRight() const {
    return this->wavelength_right;
}

/*
Sets the left and right wavelength limits of the detector. To update the position please call setPosition()
    :param left: the leftmost limit (smaller then right)
    :param right: the rightmost limit (bigger then left)
*/
void Filter::setWavelengths(double left, double right){
    this->wavelength_left = left;
    this->wavelength_right = right;
}

/*
Sets the left and right penstyle. To update the style please call setPosition()
    :param left: the left style
    :param right: the right style
*/
void Filter::setLineStyle(Qt::PenStyle left, Qt::PenStyle right){
    this->style_left = left;
    this->style_right = right;
}

/*
Sets the left and right bevel. To update the bevel please call setPosition()
    :param left: the left bevel
    :param right: the right bevel
*/
void Filter::setBevel(BevelShape left, BevelShape right){
    this->bevel_left = left;
    this->bevel_right = right;
}

/* ############################################################################################################## */

/*
Constructor: container for any collection of items that have to be plotted within the main plot.
    :param parent: parent widget
*/
template<typename ITEM>
AbstractCollection<ITEM>::AbstractCollection(const PlotRectF& rect, QGraphicsItem* scene) : 
    QGraphicsItem(scene),
    items(),
    style(nullptr),
    items_space(rect),
    minimum_width(0),
    minimum_height(0)
{
    static_assert(std::is_base_of<QGraphicsItem, ITEM>::value, "AbstractCollection<ITEM>: ITEM must inherit QGraphicsItem");
    this->setPos(0.0, 0.0);
}

/*
Container class,  the container object itself does not contain anything paintable. Therefore the bounding rectangle is empty
    :returns: empty QRectF
*/
template<typename ITEM>
QRectF AbstractCollection<ITEM>::boundingRect() const {
    return QRectF(0.0, 0.0, 0.0, 0.0);
}


/*
Builds a vector of ITEM objects that are contained with the given point. Uses scaling agnostic lookup.
    :param point: the point to contain in scene coordinates
    :returns: a vector of all contained items, can be empty!
*/
template<typename ITEM>
std::vector<ITEM*> AbstractCollection<ITEM>::containsItems(const QPointF& point) const {
    if(!this->items_space.local().contains(this->mapFromScene(point))){
        return std::vector<ITEM*>();
    }

    std::vector<ITEM*> is_contained;
    is_contained.reserve(5);
    for(ITEM* item : this->items){
        if(item->contains(point)){
            is_contained.push_back(item);
        }
    }

    return is_contained;
}

/*
Container class, the container object itself does not contain anything paintable. Therefore painting does nothing.
*/
template<typename ITEM>
void AbstractCollection<ITEM>::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    return;
}

/*
Container class, the container object itself does not contain anything paintable.
    :param painter: the painter
    :param option: the style options
    :param widget: (optional) if provided, paints to the widget being painted on
*/
template<typename ITEM>
void AbstractCollection<ITEM>::updatePainter(const Graph::Format::Style* style){
    this->style = style;

    // If given a nullptr, do nothing
    if(!this->style){
        return;
    }

    for(ITEM* item : this->items){
        item->updatePainter(style);
    }
}

/*
Returns the amount of entrees in the internal items vector
    :returns: the amount of ITEMs stored within the object
*/
template<typename ITEM>
std::size_t AbstractCollection<ITEM>::size() const {
    return this->items.size();
}

/*
Calculates the minimum size. The container object itself does not contain anything paintable. 
Therefore, the minimum size is always constants.
*/
template<typename ITEM>
void AbstractCollection<ITEM>::calculateMinimumSize() {
    return;
}

/*
Return the minimum width of the object. This is zero for a container class.
    :returns: height
*/
template<typename ITEM>
int AbstractCollection<ITEM>::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the object. This is zero for a container class.
    :returns: height
*/
template<typename ITEM>
int AbstractCollection<ITEM>::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the position of all the ITEMs stores within this container by calling the items setPosition() function.
*/
template<typename ITEM>
void AbstractCollection<ITEM>::setPosition() { 
    for(ITEM* item : this->items){
        item->setPosition(this->items_space);
    }
}

template class AbstractCollection<Spectrum>;
template class AbstractCollection<Laser>;
template class AbstractCollection<Filter>;

/* ############################################################################################################## */

/*
Constructor: container for the spectrum widgets. Handles mainly the adding and removing of the spectra.
    :param parent: parent widget
*/
SpectrumCollection::SpectrumCollection(const PlotRectF& rect, QGraphicsItem* parent) :
    AbstractCollection(rect, parent)
{
    this->items.reserve(25);
}

/*
Sets the select parameters of all contained spectrum to the specified value
    :param select: the select state
*/
void SpectrumCollection::setSelect(bool select) {
    for(Spectrum* item : this->items){
        item->setSelect(select);
    }
}

/*
Synchronizes all the spectra to the Cache state. Handles adding, order and removing of spectrum graphicsitems.
    :param input: a (ordered) vector of the cache state. Ordering is not required for proper functioning of the graph plotting
    :param settings: the graph settings, necessary for correct positioning of the new items
*/
void SpectrumCollection::syncSpectra(const std::vector<Cache::CacheID>& cache_state, const std::vector<Data::Laser>& lasers){
    // Special case: cache is empty -> remove all
    if(cache_state.empty()){
        for(Graph::Spectrum* item : this->items){
            delete item;
        }
        this->items.clear();
        return;
    }

    // Iteration has to happen with indexes, as the .insert() can cause the vector to reallocate -> invalidates all iterators
    // Sort and add items
    std::size_t index_current = 0;
    for(const Cache::CacheID& id : cache_state){
        // First look for index (if available)
        std::size_t index_item = this->findIndex(*id.data, index_current);

        if(index_item >= this->items.size()){
            // item was not found - create new one
            Graph::Spectrum* item_new = new Graph::Spectrum(*id.data, this);

            // Give new item the correct properties
            if(this->style){    // Can be nullptr
                item_new->updatePainter(this->style);
            }

            // all items are repositioned after synchronizing
            item_new->updateIntensity(lasers);
            item_new->setPosition(this->items_space);

            this->items.insert(
                std::next(this->items.begin(), static_cast<int>(index_current)), 
                item_new
            );  
        }else if(index_item == index_current){
            // Pass, already sorted
        }else{
            // item was found - rotate into position
            std::size_t index_next = index_item + 1;
            std::rotate(
                std::next(this->items.begin(), static_cast<int>(index_current)),
                std::next(this->items.begin(), static_cast<int>(index_item)), 
                std::next(this->items.begin(), static_cast<int>(index_next))
            );
        }

        ++index_current;
    }

    // Remove obsolete items 
    if(this->items.size() != index_current){
        // Delete items
        for(std::size_t i = index_current; i<this->items.size(); ++i){
            delete this->items[i];
        }
        // Delete pointers
        this->items.erase(
            std::next(this->items.begin(), static_cast<int>(index_current)), 
            this->items.cend()
        );
    }

    // Now we have the correct spectra -> sync internal state (line visibility)
    this->updateSpectra();
}

/*
Updates the internal spectra to the cache state. Uses the Spectrum internal source to update the drawing state
*/
void SpectrumCollection::updateSpectra(){
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->updateSpectrum();
    }
}

/*
Updates the intensity of the spectrum
    :param lasers: the laser data to use for intensity calculation
*/
void SpectrumCollection::updateIntensity(const std::vector<Data::Laser>& lasers){
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->updateIntensity(lasers);
    }
}

/*
Builds a vector of Spectrum items that are contained with the given point
    :param space: the plotting region, internally the scaling properties are used for quick lookup
    :param point: the point to contain in scene coordinates
    :returns: a vector of all contained items, can be empty!
*/
std::vector<Spectrum*> SpectrumCollection::containsItems(const PlotRectF& space, const QPointF& point) const {
    if(!this->items_space.local().contains(this->mapFromScene(point))){
        return std::vector<Spectrum*>();
    }

    std::vector<Spectrum*> is_contained;
    is_contained.reserve(5);
    for(Spectrum* item : this->items){
        if(item->contains(space, point)){
            is_contained.push_back(item);
        }
    }

    return is_contained;
}

/*
Finds the index of a Graph::Spectrum with identical .source() pointer in the spectra_items vector. 
Returns the index or if not found the size of the vector (if this->spectra_items is empty it returns 1)
    :param id: the data to compare to
    :param index_start: starts the index search at the specified start. Undefined behavior for values > size() -1
*/
std::size_t SpectrumCollection::findIndex(const Data::CacheSpectrum& id, std::size_t index_start) const {
    if(this->items.empty()){
        return 1;
    }

    std::size_t i;
	for(i=index_start; i < this->items.size(); ++i){
		if(&this->items[i]->source() == &id){
			return i;
		}
	}

	return i;
}

/* ############################################################################################################## */

/*
Constructor: container for Laser widgets. This mainly handles the adding and removing of the Lasers.
    :param rect: the plotting rectangle.
    :param parent: parent widget
*/
LaserCollection::LaserCollection(const PlotRectF& rect, QGraphicsItem* parent) :
    AbstractCollection(rect, parent)
{
    this->items.reserve(5);
}

/*
Returns a list of the Data::Laser representation of the internal Graph::Laser objects. 
Data::Laser objects are mostly empty!!! Not all data is stored inside the Graph::Laser objects. 
*/
const std::vector<Data::Laser> LaserCollection::lasers() const {
    std::vector<Data::Laser> laser_wavelengths;
    
    for(const Graph::Laser* item : this->items){
        laser_wavelengths.push_back(Data::Laser(item->wavelength()));
    }

    return laser_wavelengths;
}

/*
Synchronizes the collection to contain zero or one Graph::Laser.
    :param wavelength: the wavelength (in nm) the Graph::Laser should show. A negative value will result in removal of the item
    :param visible: the visibility state of the laser
*/
void LaserCollection::syncLaser(double wavelength){
    // If wavelength is below zero, all items will be removed
    if(wavelength < 0){
        for(Graph::Laser* item : this->items){
            delete item;
        }
        this->items.clear();
        return;
    }

    if(this->items.empty()){
        Laser* item = new Laser(wavelength, this);
        this->items.push_back(item);

        if(this->style){    // Can be nullptr
            item->updatePainter(this->style);
        }
        item->setPosition(this->items_space);

        return;
    }

    // Not empty so atleast one item exists
    this->items[0]->setWavelength(wavelength);
    this->items[0]->setPosition(this->items_space);

    // Make sure only 1 laser is constructed
    if(this->items.size() > 1){
        for(std::size_t i=1; i > this->items.size(); ++i){
            delete this->items[i];
        }
        this->items.erase(
            std::next(this->items.begin(), 1), 
            this->items.cend()
        );
    }
}

/*
Synchronizes the laser_state to the container laser items
    :param lasers: the lasers to synchronize
    :param visible: whether the lasers are visible or not
*/
void LaserCollection::syncLasers(const std::vector<Data::Laser>& lasers){
    // Special clear state
    if(lasers.empty()){
        for(Graph::Laser* item : this->items){
            delete item;
        }
        this->items.clear();
        return;
    }
    
    // Construct / Destruct the necessary amount of item
    if(this->items.size() < lasers.size()){
        std::size_t to_add = lasers.size() - this->items.size();
        for(std::size_t i=0; i < to_add; ++i){
            Laser* item = new Laser(this);
            this->items.push_back(item);
        }
    }else if(this->items.size() > lasers.size()){
        for(std::size_t i=lasers.size(); i < this->items.size(); ++i){
            delete this->items[i];
        }
        this->items.erase(
            std::next(this->items.begin(), static_cast<int>(lasers.size())), 
            this->items.cend()
        );
    }

    // Synchronize the wavelength
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->setWavelength(lasers[i].wavelength());
        if(this->style){    // Can be nullptr, has to be synchronized before setPosition, as the size of the line changes the location
            this->items[i]->updatePainter(this->style);
        }
        this->items[i]->setPosition(this->items_space);
    }
}

/*
Update the visibility state of the laser items
    :param visible: the visibility state
*/
void LaserCollection::updateLasers(bool visible){
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->setVisible(visible);
    }
}

/* ############################################################################################################## */

/*
Constructor: container for the Detector widgets. This mainly handles the adding and removing of detectors.
    :param rect: the plotting rectangle.
    :param parent: parent widget
*/
FilterCollection::FilterCollection(const PlotRectF& rect, QGraphicsItem* parent) : 
    AbstractCollection(rect, parent)
{
    this->items.reserve(6);
}

/*
Synchronizes the filters to the graph
    :param filters: the filter to synchronize
    :param visible: the visibility state of the filters
*/
void FilterCollection::syncFilters(const std::vector<Data::Filter>& filters){  
    // Special clear state
    if(filters.empty()){
        for(Graph::Filter* item : this->items){
            delete item;
        }
        this->items.clear();
        return;
    }
    
    // Construct / Destruct the necessary amount of item
    if(this->items.size() < filters.size()){
        std::size_t to_add = filters.size() - this->items.size();
        for(std::size_t i=0; i < to_add; ++i){
            Filter* item = new Filter(this);
            item->setBevel(Filter::BevelShape::Round, Filter::BevelShape::Round);
            item->setLineStyle(Qt::SolidLine, Qt::SolidLine);
            this->items.push_back(item);
        }
    }else if(this->items.size() > filters.size()){
        for(std::size_t i=filters.size(); i < this->items.size(); ++i){
            delete this->items[i];
        }
        this->items.erase(
            std::next(this->items.begin(), static_cast<int>(filters.size())), 
            this->items.cend()
        );
    }

    // Synchronize the wavelength
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->setWavelengths(filters[i].wavelengthMin(), filters[i].wavelengthMax()); 
        if(this->style){    // Can be nullptr
            this->items[i]->updatePainter(this->style);
        }
        this->items[i]->setPosition(this->items_space);
    }
}

/*
Update the visibility state of the laser items
    :param visible: the visibility state
*/
void FilterCollection::updateFilters(bool visible){
    for(std::size_t i=0; i<this->items.size(); ++i){
        this->items[i]->setVisible(visible);
    }
}

} // Graph namespace
