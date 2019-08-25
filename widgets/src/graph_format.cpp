/**** General **************************************************************
** Version:    v0.9.4
** Date:       2019-07-24
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_format.h"

#include <QDebug>

namespace Graph {

namespace Format {

/*
Constructor for Axis object
    :param min: absolute minimum value
    :param max: absolute maximum value, expected to be larger then min
*/
Axis::Axis(int min, int max, QString label) :
    min(min),
    max(max),
    label(label)
{}

/*
Constructor for AxisRange object
    :param begin: the axis visible begin value, expected to be equal or bigger then the equivalent Axis.min value, and smaller then Axis.max value
    :param end: the axis visible end value, expected to be equal or smaller then the equivalent Axis.max value, and bigger then Axis.min value
*/
AxisRange::AxisRange(int begin, int end) :
    begin(begin),
    end(end),
    default_begin(begin),
    default_end(end)
{}

/*
Constructor for Tick object, will not add a label for this Tick
    :param location: the tick mark location
*/
Tick::Tick(int location) :
    location(location),
    label()
{}

/*
Constructor for Tick object, will add a label for this Tick
    :param location: tick mark location
    :param label: tick label
*/
Tick::Tick(int location, QString label) :
    location(location),
    label(label)
{}

/*
Find the indexes which fit in between the begin and end value (if any)
    :param begin: lowest boundary
    :param end: highest boundary
*/
template<std::size_t TICK_COUNT>
void Ticks<TICK_COUNT>::findIndexes(int begin, int end){
    if(TICK_COUNT <= 0){
        return;
    }
    
    // Check is begin and end are reversed (for Y-axis)
    if(begin > end){
        // Reversed order, so swap begin and end values
        std::swap(begin, end);
    }

    // Check if the value are within bounds of the array
    if(end < this->ticks[0].location || begin > this->ticks[this->ticks.size() -1].location){
        this->valid = false;
        return;
    }

    // Valid region so get indexes
    std::size_t index_begin = 0;
    for(std::size_t i=0; i<this->ticks.size(); ++i){
        if(this->ticks[i].location >= begin){
            index_begin = i;
            break;
        }
    }

    // std::size_t can be unsigned, so cannot be smaller then 0, so use index + 1 for iteration
    std::size_t index_end = 0;
    for(std::size_t i=this->ticks.size(); i>0; --i){
        if(this->ticks[i-1].location <= end){
            index_end = i;
            break;
        }
    }

    this->valid = true;
    this->index_begin = index_begin;
    this->index_end = index_end;

}

/*
Constructor: attributes are taken care of in header file, just to set correct state ticks
*/
Settings::Settings(){
    this->x_ticks.findIndexes(this->x_range.begin, this->x_range.end);
    this->y_ticks.findIndexes(this->y_range.begin, this->y_range.end);
}

} // Format namespace

} // Graph namespace
