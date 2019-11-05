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
    this->setMargins(0, 2, 0, 0);
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
    qreal x = coor_center.x() - (this->minimum_width / 2);
    qreal y = coor_center.y() - (this->minimum_height / 2);

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
    qreal x = coor_center.x() - (this->minimum_width / 2);
    qreal y = coor_center.y() + (this->minimum_height / 2);

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
    this->setMargins(2, 0, 3, 0);
    this->line_length = 5;

    // Setup done -> calculate minimum size
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the tick + margin size.
*/
void TicksX::calculateMinimumSize() { 
    if(this->items.empty()){
        this->minimum_width = 0;
        this->minimum_height = 0;
        return;
    }

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
void TicksX::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    this->setPos(space.topLeft());
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = 0 + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);
    qreal y_top = 0 + this->margins().top();
    qreal y_bottom = y_top + this->line_length;

    for(GridLine* item : this->items){
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

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
    if(this->items.empty()){
        this->minimum_width = 0;
        this->minimum_height = 0;
        return;
    }

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
void TicksY::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    this->setPos(space.topLeft());
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = 0 + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.width() - this->margins().right();
    qreal x_left = x_right - this->line_length;

    for(GridLine* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;

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
    this->setMargins(2, 1, 3, 1);
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
void GridLinesX::setPosition(const Graph::Format::Settings& settings, const QRectF& space) {
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately
    this->setPos(space.topLeft());

    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = 0 + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);
    qreal y_top = 0 + this->margins().top();
    qreal y_bottom = space.height() - this->margins().bottom();

    for(GridLine* item : this->items){
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

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
    this->setMargins(1, 2, 2, 2);
}

/*
Placeholder, doesnt do anything. Just here for completion sake, minimum size for lines doesnt really make sense...
*/
void GridLinesY::calculateMinimumSize() {
    return;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void GridLinesY::setPosition(const Graph::Format::Settings& settings, const QRectF& space) {
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    this->setPos(space.topLeft());
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = 0 + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.width() - this->margins().right();
    qreal x_left = 0 + this->margins().left();

    for(GridLine* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;

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
    this->setMargins(2, 0, 3, 0);
    this->space_offset = 1;
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
void GridLabelsX::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    // Set local coordinate system to topleft of the available space
    this->setPos(space.topLeft());
 
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }

    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = 0 + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);

    // Correct start location for offset

    qreal y_item = (space.height() * 0.5) - (this->minimum_height * 0.5);

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, leave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());

    for(GridLabel* item : this->items){
        // First calculate were the text item's middle should end up
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

        // Now calculate the offset to center the text on that position
        int item_width = font_metric.width(item->text());
        x_item -= (item_width * 0.5);

        // Make sure the x coordinate doesnt go outside of the assigned space
        x_item = std::max(0.0 - this->space_offset, x_item);
        x_item = std::min(space.width() - item_width, x_item);

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
    this->setMargins(0, 2, 2, 2);
    this->space_offset = 3;
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
void GridLabelsY::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the y_range and ticks are added appropriately 
    // Set local coordinate system to topleft of the available space
    this->setPos(space.topLeft());

    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = 0 + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.width() - this->margins().right();

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, leave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());
    int font_height = font_metric.height(); 

    for(GridLabel* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;
        y_item -= (font_height * 0.5);
        // tick line size correction & correction to fix disalignment from AxisTicksY
        y_item -= 1;

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
    minimum_height(0)
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
    this->setPen(style->penAxis());
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
    gradient()
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
void Colorbar::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    qreal stepsize = (space.right() - space.left() + this->margins().left() - this->margins().right()) / (settings.x_range.end - settings.x_range.begin);
    qreal x_380 = 380 - settings.x_range.begin;
    x_380 *= stepsize;
    x_380 += space.left() + this->margins().left();
    
    qreal x_780 = 780 - settings.x_range.begin;
    x_780 *= stepsize;
    x_780 += space.left() + this->margins().left();

    this->gradient.setStart(QPointF(x_380, space.top()));
    this->gradient.setFinalStop(QPointF(x_780, space.bottom()));
    this->setBrush(QBrush(gradient));
    
    this->setRect(space.marginsRemoved(this->margins()));
}

/*
Updates the pen used by the painter to the pen as build by the style
    :param style: pen factory
*/
void Colorbar::updatePainter(const Graph::Format::Style* style){
    this->setPen(style->penAxis());
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
    item_margins(2, 2, 3, 2),
    minimum_width(0),
    minimum_height(0),
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
    :param widget: (unused) if provided, paints to the widget being painted on
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
        if(this->spectrum_excitation.contains(this->mapFromScene(point))){
            return true;
        }
    }

    if(this->visible_emission){
        return this->spectrum_emission.contains(this->mapFromScene(point));
    }

    return false;
}


/*
Sets margins of the item, be carefull this should aline with all other gridlines/labels items
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void Spectrum::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& Spectrum::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int Spectrum::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int Spectrum::minimumHeight() const {
    return this->minimum_height;
}

/*
Calculates the minimum size of the item, doesnt do anything. Minimum size for a plotting region depending widget doesnt make sense.
*/
void Spectrum::calculateMinimumSize() { 
    return;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and minimumHeight.
    :param settings: the graph settings
    :param space: the allocated space
*/
void Spectrum::setPosition(const Graph::Format::Settings& settings, const QRectF& space){
    // Set source to the spectrum region of the graph
    this->setPos(space.topLeft());

    // Set bounding region
    // Bouding box can be more precise if we check the left and right values after scaling
    if(space != this->spectrum_space){
        this->spectrum_space = space;
        this->prepareGeometryChange();
    }

        // Adjust x and y ranges for margins
    // Note for later - correct for line thickness
    double x_begin = settings.x_range.begin;
    double x_end = settings.x_range.end;
    double x_fraction = (x_end - x_begin) / (space.width() - this->item_margins.left() - this->item_margins.right());
    x_begin -= (x_fraction * this->item_margins.left());
    x_end += (x_fraction * this->item_margins.right());

    double y_begin = settings.y_range.begin;
    double y_end = settings.y_range.end;
    double y_fraction = (y_end - y_begin) / (space.height() - this->item_margins.top() - this->item_margins.bottom());
    y_begin -= (y_fraction * this->item_margins.top());
    y_end += (y_fraction * this->item_margins.bottom());

    // Correct plotting space for line width
    QRectF plot_space = this->spectrum_space;
    double pen_adjust = this->pen_excitation.widthF() * 0.5;
    plot_space.adjust(pen_adjust, pen_adjust, -pen_adjust, -pen_adjust);

    // Scale excitation first
    this->spectrum_excitation.scale(
        this->spectrum_source.spectrum().excitation(),
        plot_space,
        x_begin,
        x_end,
        y_begin,
        y_end,
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
        x_begin,
        x_end,
        y_begin,
        y_end,
        this->intensity_coefficient
    );

    // Copy and close emission data into fill
    this->spectrum_emission_fill.copyCurve(this->spectrum_emission);
    this->spectrum_emission_fill.closeCurve(this->spectrum_space);
}

/*
Update the internal state to the source state
*/
void Spectrum::updateSpectrum(const Data::CacheSpectrum& cache_spectrum) {
    this->visible_excitation = cache_spectrum.visibleExcitation();
    this->visible_emission = cache_spectrum.visibleEmission();
    this->select_excitation = cache_spectrum.selectExcitation();
    this->select_emission = cache_spectrum.selectEmission();
    
    // Now plot the updated curve
    this->update(this->boundingRect());
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
Constructor: container for the specturm widgets. Handles mainly the adding and removing of the spectra.
    :param parent: parent widget
*/
Spectra::Spectra(QGraphicsItem* parent) :
    QGraphicsItem(parent),
    spectra_items(),
    spectra_style(nullptr),
    minimum_width(0),
    minimum_height(0),
    spectra_space(0.0, 0.0, 0.0, 0.0)
{
    this->spectra_items.reserve(25);
}

/*
Container class, doesnt contain its own object, so bounding rectangle is empty
    :returns: empty QRectF
*/
QRectF Spectra::boundingRect() const {
    return QRectF(0.0, 0.0, 0.0, 0.0);
}

/*
Container class, doesnt contain its own object to paint.
    :param painter: the painter
    :param option: the style options
    :param widget: (optional) if provided, paints to the widget being painted on
*/
void Spectra::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    return;
}

/*
Builds a vector of Graph::Spectrum items contain the point
    :param point: the point to contain in scene coordinates
    :returns: a vector of all contained Spectrum, can be empty!
*/
std::vector<Spectrum*> Spectra::containsSpectrum(const QPointF& point) const {
    if(!this->spectra_space.contains(this->mapFromScene(point))){
        return std::vector<Spectrum*>();
    }

    std::vector<Spectrum*> is_contained;
    is_contained.reserve(20);
    for(auto* item : this->spectra_items){
        if(item->contains(point)){
            is_contained.push_back(item);
        }
    }

    return is_contained;
}

/*
Returns the amount of entrees in the internal spectra_items vector
    :returns: the amount of spectra stored within the object
*/
std::size_t Spectra::size() const {
    return this->spectra_items.size();
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int Spectra::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int Spectra::minimumHeight() const {
    return this->minimum_height;
}

/*
Calculates the minimum size. Which is nothing, so does nothing.
*/
void Spectra::calculateMinimumSize() {
    return;
}

/*
Sets the position of all the spectrums stores within this container
    :param settings: the graph settings
    :param space: the space appointed to the widget
*/
void Spectra::setPosition(const Graph::Format::Settings& settings, const QRectF& space) {
    this->setPos(space.topLeft());

    // Localize the bounding box
    QRectF local_space = QRectF(0.0, 0.0, space.width(), space.height());
    this->spectra_space = local_space;

    for(Graph::Spectrum* spectrum : this->spectra_items){
        spectrum->setPosition(settings, local_space);
    }    
}

/*
Sets the select parameters of all contained spectrum to the specified value
    :param select: the select state
*/
void Spectra::setSelect(bool select) {
    for(Spectrum* item : this->spectra_items){
        item->setSelect(select);
    }
}

/*
Synchronizes all the spectra to the Cache state. Handles adding, order and removing of spectrum graphicsitems.
    :param input: a (ordered) vector of the cache state. Ordering is not required for proper functioning of the graph plotting
    :param settings: the graph settings, necessary for correct positioning of the new items
*/
void Spectra::syncSpectra(const std::vector<Cache::CacheID>& cache_state, const Graph::Format::Settings& settings){
    // Special case: cache is empty -> remove all
    if(cache_state.empty()){
        for(Graph::Spectrum* item : this->spectra_items){
            delete item;
        }
        this->spectra_items.clear();
    }

    // Iteration has to happen with indexes, as the .insert() can cause the vector to reallocate -> invalidates all iterators
    // Sort and add items
    std::size_t index_current = 0;
    for(const Cache::CacheID& id : cache_state){
        // First look for index (if available)
        std::size_t index_item = this->findIndex(*id.data, index_current);

        if(index_item >= this->spectra_items.size()){
            // item was not found - create new one
            Graph::Spectrum* item_new = new Graph::Spectrum(*id.data, this);

            // Give new item the correct properties
            if(this->spectra_style){    // Can be nullptr
                item_new->updatePainter(this->spectra_style);
            }
            item_new->setPosition(settings, this->spectra_space);

            this->spectra_items.insert(
                std::next(this->spectra_items.begin(), static_cast<int>(index_current)), 
                item_new
            );  
        }else if(index_item == index_current){
            // Pass, already sorted
        }else{
            // item was found - rotate into position
            std::size_t index_next = index_item + 1;
            std::rotate(
                std::next(this->spectra_items.begin(), static_cast<int>(index_current)),
                std::next(this->spectra_items.begin(), static_cast<int>(index_item)), 
                std::next(this->spectra_items.begin(), static_cast<int>(index_next))
            );
        }

        ++index_current;
    }

    // Remove obsolete items 
    if(this->spectra_items.size() != index_current){
        // Delete items
        for(std::size_t i = index_current; i<this->spectra_items.size(); ++i){
            delete this->spectra_items[i];
        }
        // Delete pointers
        this->spectra_items.erase(
            std::next(this->spectra_items.begin(), static_cast<int>(index_current)), 
            this->spectra_items.cend()
        );
    }

    // Now we have the correct spectra -> sync internal state (line visibility)
    this->updateSpectra(cache_state);
}

/*
Updates the internal spectra to the cache state. Assumes that the cache has been properly synced. Does not add or remove items.
    :param cache_state: the state to update to
*/
void Spectra::updateSpectra(const std::vector<Cache::CacheID>& cache_state){
    for(std::size_t i=0; i<this->spectra_items.size(); ++i){
        this->spectra_items[i]->updateSpectrum(*cache_state[i].data);
    }
}

/*
Stores the style object pointer, and updates current spectrum objects
    :param style: pen factory
*/
void Spectra::updatePainter(const Graph::Format::Style* style){
    this->spectra_style = style;

    for(Graph::Spectrum* item : this->spectra_items){
        item->updatePainter(style);
    }
}

/*
Finds the index of a Graph::Spectrum with identical .source() pointer in the spectra_items vector. 
Returns the index or if not found the size of the vector (if this->spectra_items is empty it returns 1)
    :param id: the data to compare to
    :param index_start: starts the index search at the specified start. Undefined behavior for values > size() -1
*/
std::size_t Spectra::findIndex(const Data::CacheSpectrum& id, std::size_t index_start) const {
    if(this->spectra_items.empty()){
        return 1;
    }

    std::size_t i;
	for(i=index_start; i < this->spectra_items.size(); ++i){
		if(&this->spectra_items[i]->source() == &id){
			return i;
		}
	}

	return i;
}


} // Graph namespace
