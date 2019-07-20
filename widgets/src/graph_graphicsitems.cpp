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

/*
Constructor: axis title for the x-axis
    :param text: axis label text
    :param parent: parent
*/
AxisTitleX::AxisTitleX(const QString& text, QGraphicsItem* parent) :
    QGraphicsSimpleTextItem(text, parent),
    item_margins(0, 2, 0, 0),
    minimum_width(0),
    minimum_height(0)
{
    // Now everything has been build properly, recalculate size
    this->calculateMinimumSize();
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTitleX::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTitleX::margins() const {
    return this->item_margins;
}

/*
Set font, and recalculates the minimum width and height
    :param font: the new font
*/
void AxisTitleX::setFont(const QFont& font){
    QGraphicsSimpleTextItem::setFont(font);

    // Recalculate size specifics
    this->calculateMinimumSize();
}

/*
Set text, and recalculates the minimum width and height
    :param text: the new text
*/
void AxisTitleX::setText(const QString& text){
    QGraphicsSimpleTextItem::setText(text);

    // Recalculate size specifics
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTitleX::calculateMinimumSize() {
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
Getter for minimum width of the item.
    :returns: width in pixels
*/
int AxisTitleX::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int AxisTitleX::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void AxisTitleX::setPosition(const QRectF& space){
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
AxisTitleY::AxisTitleY(const QString& text, QGraphicsItem* parent) :
    QGraphicsSimpleTextItem(text, parent),
    item_margins(0, 0, 2, 0),
    minimum_width(0),
    minimum_height(0)
{
    this->setRotation(-90);
    // Now everything has been build properly, recalculate size
    this->calculateMinimumSize();

}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTitleY::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTitleY::margins() const {
    return this->item_margins;
}

/*
Set font, and recalculates the minimum width and height
    :param font: the new font
*/
void AxisTitleY::setFont(const QFont& font){
    QGraphicsSimpleTextItem::setFont(font);

    // Recalculate size specifics
    this->calculateMinimumSize();
}

/*
Set text, and recalculates the minimum width and height
    :param text: the new text
*/
void AxisTitleY::setText(const QString& text){
    QGraphicsSimpleTextItem::setText(text);

    // Recalculate size specifics
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTitleY::calculateMinimumSize() {
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
Getter for minimum width of the item.
    :returns: width in pixels
*/
int AxisTitleY::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int AxisTitleY::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void AxisTitleY::setPosition(const QRectF& space){
    // Aligns text to middle of space
    QPointF coor_center = space.center();
    qreal x = coor_center.x() - (this->minimum_width / 2);
    qreal y = coor_center.y() + (this->minimum_height / 2);

    this->setPos(x, y);
}

/* ############################################################################################################## */

/* 
Constructor: a simple line graphicsitem
    :param location: location in 'real' 
    :param parent: parent
*/
TickLine::TickLine(int location, QGraphicsItem* parent) :
    QGraphicsLineItem(parent),
    tick_location(location)
{}

/*
Set location attribute
    :param location: value to update location to
*/
void TickLine::setLocation(int location){
    this->tick_location = location;
}

/*
Get location
    :returns: location
*/
int TickLine::location() const {
    return this->tick_location;
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisTicksX::AxisTicksX(QGraphicsScene* scene) :
    graphics_scene(scene),
    items(),
    item_margins(2, 0, 3, 0), // Right margin needs to be atleast one, otherwise rightmost line is outside viewport
    tick_length(5),
    minimum_width(0),
    minimum_height(0)
{
    // Setup done -> calculate minimum size
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTicksX::calculateMinimumSize() { 
    int width = this->item_margins.left();
    width += this->item_margins.right();

    int height = this->tick_length;
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTicksX::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTicksX::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AxisTicksX::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AxisTicksX::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisTicksX::setPosition(const Graph::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = space.left() + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);
    qreal y_top = space.top() + this->margins().top();
    qreal y_bottom = y_top + this->tick_length;

    for(TickLine* item : this->items){
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

        item->setLine(x_item, y_top, x_item, y_bottom);
    }

}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void AxisTicksX::setTicks(const Graph::Settings& settings) {
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
            // Item is unavailable, add new TickLine QGraphicsLineItem
            TickLine* item = new TickLine(settings.x_ticks.ticks[i].location);
            // Give ownership of item to GraphicsScene
            this->graphics_scene->addItem(item);
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
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<TickLine*>::difference_type>(tick_count)), this->items.end());

    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisTicksY::AxisTicksY(QGraphicsScene* scene) :
    graphics_scene(scene),
    items(),
    item_margins(0, 2, 0, 2),
    tick_length(5),
    minimum_width(0),
    minimum_height(0)
{
    // Setup done -> calculate minimum size
    this->calculateMinimumSize();
}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTicksY::calculateMinimumSize() {
    int width = this->tick_length;
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTicksY::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTicksY::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AxisTicksY::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AxisTicksY::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisTicksY::setPosition(const Graph::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = space.top() + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.right() - this->margins().right();
    qreal x_left = x_right - this->tick_length;

    for(TickLine* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;

        item->setLine(x_left, y_item, x_right, y_item);
    }

}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void AxisTicksY::setTicks(const Graph::Settings& settings) {
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
            // Item is unavailable, add new TickLine QGraphicsLineItem
            TickLine* item = new TickLine(settings.y_ticks.ticks[i].location);
            // Give ownership of item to GraphicsScene
            this->graphics_scene->addItem(item);
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
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<TickLine*>::difference_type>(tick_count)), this->items.end());

    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisLinesX::AxisLinesX(QGraphicsScene* scene) :
    AxisTicksX(scene)
{
    this->setMargins(2, 1, 3, 1);
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisLinesX::setPosition(const Graph::Settings& settings, const QRectF& space) {
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = space.left() + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);
    qreal y_top = space.top() + this->margins().top();
    qreal y_bottom = space.bottom() - this->margins().bottom();

    for(TickLine* item : this->items){
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

        item->setLine(x_item, y_top, x_item, y_bottom);
    }
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisLinesY::AxisLinesY(QGraphicsScene* scene) :
    AxisTicksY(scene)
{
    this->setMargins(1, 2, 2, 2);
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisLinesY::setPosition(const Graph::Settings& settings, const QRectF& space) {
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = space.top() + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.right() - this->margins().right();
    qreal x_left = space.left() + this->margins().left();

    for(TickLine* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;

        item->setLine(x_left, y_item, x_right, y_item);
    }
}

/* ############################################################################################################## */

/* 
Constructor: a simple line graphicsitem
    :param location: location in 'real' 
    :param parent: parent
*/
TickLabel::TickLabel(int location, QString label, QGraphicsItem* parent) :
    QGraphicsSimpleTextItem(parent),
    tick_location(location)
{
    this->setText(label);
}

/*
Set location attribute
    :param location: value to update location to
*/
void TickLabel::setLocation(int location){
    this->tick_location = location;
}

/*
Get location
    :returns: location
*/
int TickLabel::location() const {
    return this->tick_location;
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisTickLabelsX::AxisTickLabelsX(QGraphicsScene* scene) :
    graphics_scene(scene),
    items(),
    item_margins(2, 0, 3, 0),
    space_offset(1),
    minimum_width(0),
    minimum_height(0)
{}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTickLabelsX::calculateMinimumSize(const QFont& font, const QString& text) { 
    QFontMetrics font_metric = QFontMetrics(font);

    int width = font_metric.width(text);
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.height();
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTickLabelsX::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTickLabelsX::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AxisTickLabelsX::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AxisTickLabelsX::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisTickLabelsX::setPosition(const Graph::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }

    qreal x_start_location = settings.x_range.begin;
    qreal x_start_coor = space.left() + this->margins().left();
    qreal x_stepsize = space.width() - this->margins().left() - this->margins().right();
    x_stepsize /= (settings.x_range.end - settings.x_range.begin);
    qreal y_item = space.center().y() - (this->minimum_height / 2);

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, heave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());

    for(TickLabel* item : this->items){
        // First calculate were the text item's middle should end up
        qreal x_item = (item->location() - x_start_location);
        x_item *= x_stepsize;
        x_item += x_start_coor;

        // Now calculate the offset to center the text on that position
        int width = font_metric.width(item->text());
        x_item -= (width / 2);

        // Make sure the x coordinate doesnt go outside of the assigned space
        x_item = std::max(space.left() - this->space_offset, x_item);
        x_item = std::min(space.right() - width + this->space_offset, x_item);

        item->setPos(x_item, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void AxisTickLabelsX::setTicks(const Graph::Settings& settings) {
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
            // Item is unavailable, add new TickLine QGraphicsLineItem
            TickLabel* item = new TickLabel(settings.x_ticks.ticks[i].location, settings.x_ticks.ticks[i].label);
            // Give ownership of item to GraphicsScene
            this->graphics_scene->addItem(item);
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
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<TickLine*>::difference_type>(tick_count)), this->items.end());
    }

    // In the case of the labels we also need to calculate the minimum text width as that can change with the ticks
    // As the ticks are in order, the highest / longest label is likely the final entree
    // This saves having to check each tick individually, should be changed if the text differs too much from this
    this->calculateMinimumSize(this->items[this->items.size()-1]->font(), this->items[this->items.size()-1]->text());
}

/* ############################################################################################################## */

/*
Constructor: holds a list of pointers to a list of QGraphicItems, constructs / destructs items if necessary
    :param scene: graphicsscene to add the items to. This is required, do NOT parse nullptr, items must be added to a scene for proper lifetime handling
*/
AxisTickLabelsY::AxisTickLabelsY(QGraphicsScene* scene) :
    graphics_scene(scene),
    items(),
    item_margins(0, 2, 2, 2),
    space_offset(3),
    minimum_width(0),
    minimum_height(0)
{}

/*
Calculates the minimum size of the item, aka the text + margin size
*/
void AxisTickLabelsY::calculateMinimumSize(const QFont& font, const QString& text) { 
    QFontMetrics font_metric = QFontMetrics(font);

    int width = font_metric.width(text);
    width += this->item_margins.left();
    width += this->item_margins.right();

    int height = font_metric.height();
    height += this->item_margins.top();
    height += this->item_margins.bottom();

    this->minimum_width = width;
    this->minimum_height = height;
}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void AxisTickLabelsY::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& AxisTickLabelsY::margins() const {
    return this->item_margins;
}

/*
Returns the minimum width of (one) ticks
    :returns int: width
*/
int AxisTickLabelsY::minimumWidth() const {
    return this->minimum_width;
}

/*
Return the minimum height of the ticks
    :returns: height
*/
int AxisTickLabelsY::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth
    :param space: the allocated space
*/
void AxisTickLabelsY::setPosition(const Graph::Settings& settings, const QRectF& space){
    // Allocated width will be (linearly) divided over the x_range and ticks are added appropriately 
    
    // If no ticks, do nothing
    if(this->items.empty()){
        return;
    }
    
    qreal y_start_location = settings.y_range.begin;
    qreal y_start_coor = space.top() + this->margins().top();
    qreal y_stepsize = space.height() - this->margins().top() - this->margins().bottom();
    y_stepsize /= (settings.y_range.end - settings.y_range.begin);
    qreal x_right = space.right() - this->margins().right();

    // Get font metrics of first item, assume all other ticks use same font (they should)
    // This wouldnt change upon a resize, so when implementing the style, heave this over to a style change property
    QFontMetrics font_metric = QFontMetrics(this->items[0]->font());
    int font_height = font_metric.height(); 

    for(TickLabel* item : this->items){
        qreal y_item = (item->location() - y_start_location);
        y_item *= y_stepsize;
        y_item += y_start_coor;
        y_item -= (font_height / 2);
        // tick line size correction & correction to fix disalignment from AxisTicksY
        y_item -= 1 + 0.5;

        // Make sure the y coordinate doesnt go outside of the assigned space
        y_item = std::max(space.top() - this->space_offset, y_item);
        y_item = std::min(space.bottom() - font_height + this->space_offset, y_item);

        // Calculate x so that the text is aligned to the right
        int font_width = font_metric.width(item->text());
        qreal x_item = x_right - font_width;

        item->setPos(x_item, y_item);
    }
}

/*
Calculates and sets the amount of ticks necessary according to the Graph::Settings
*/
void AxisTickLabelsY::setTicks(const Graph::Settings& settings) {
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
            // Item is unavailable, add new TickLine QGraphicsLineItem
            TickLabel* item = new TickLabel(settings.y_ticks.ticks[i].location, settings.y_ticks.ticks[i].label);
            // Give ownership of item to GraphicsScene
            this->graphics_scene->addItem(item);
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
        this->items.erase(std::next(this->items.begin(), static_cast<std::vector<TickLine*>::difference_type>(tick_count)), this->items.end());
    }

    // In the case of the labels we also need to calculate the minimum text width as that can change with the ticks
    // As the ticks are in order, the highest / longest label is likely the first entree (graphs y-axis is inversed)
    // This saves having to check each tick individually, should be changed if the text differs too much from this
    this->calculateMinimumSize(this->items[this->items.size()-1]->font(), this->items[this->items.size()-1]->text());
}

/* ############################################################################################################## */

/*
Constructor: background of the actual graph area
    :param parent: parent
*/
GraphBackground::GraphBackground(QGraphicsItem* parent) :
    QGraphicsRectItem(parent),
    item_margins(1, 1, 1, 0),
    minimum_width(0),
    minimum_height(0)
{
    // Create empty pen to block outline from drawing
    QPen item_pen{};
    item_pen.setStyle(Qt::NoPen);
    this->setPen(item_pen);

    // Create brush to fill the background
    QBrush item_brush(QColor("#E0E0E0"));
    this->setBrush(item_brush);

}

/*
Sets margins of the item
    param left: margin in pixels to the left
    param top: margin in pixels to the top
    param right: margin in pixels to the right
    param bottom: margin in pixels to the bottom
*/
void GraphBackground::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& GraphBackground::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int GraphBackground::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int GraphBackground::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void GraphBackground::setPosition(const QRectF& space){
    this->setRect(space.marginsRemoved(this->margins()));
}

/* ############################################################################################################## */

/*
Constructor: outline of the graph area
    :param parent: parent
*/
GraphOutline::GraphOutline(QGraphicsItem* parent) :
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
void GraphOutline::setMargins(int left, int top, int right, int bottom){
    this->item_margins.setLeft(left);
    this->item_margins.setTop(top);
    this->item_margins.setRight(right);
    this->item_margins.setBottom(bottom);
}

/*
Get margins
    :returns: margins of the item
*/
const QMargins& GraphOutline::margins() const {
    return this->item_margins;
}

/*
Getter for minimum width of the item.
    :returns: width in pixels
*/
int GraphOutline::minimumWidth() const {
    return this->minimum_width;
}

/*
Getter for minimum height of the item.
    :returns: height in pixels
*/
int GraphOutline::minimumHeight() const {
    return this->minimum_height;
}

/*
Sets the location of the item within the space allocated, assumes enough space to adhere to minimumWidth and Height
    :param space: the allocated space
*/
void GraphOutline::setPosition(const QRectF& space){
    this->setRect(space.marginsRemoved(this->margins()));
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
void Colorbar::setPosition(const Graph::Settings& settings, const QRectF& space){
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

} // Graph namespace
