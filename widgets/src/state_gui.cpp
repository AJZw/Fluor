/**** General **************************************************************
** Version:    v0.9.7
** Date:       2020-01-17
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "state_gui.h"

namespace State {

/* ############################################################################## */

/*
Constructor: construct default graph state, internal vector has size 0
*/
GraphState::GraphState() :
    graph_lasers(),
    graph_laserline(nullptr),
    selected(false),
    visible_lasers(false),
    visible_filters(false)
{}

/*
Constructor: construct graph state, internal vector has size 0, with the specified visibility states
*/
GraphState::GraphState(bool visible_lasers, bool visible_filters) :
    graph_lasers(),
    graph_laserline(nullptr),
    selected(false),
    visible_lasers(visible_lasers),
    visible_filters(visible_filters)
{}

/*
Constructor: constructs graph state, internal vector has size of the given size
    :param size: the reserved size of the laser vector
*/
GraphState::GraphState(std::size_t size) :
    graph_lasers(),
    graph_laserline(nullptr),
    selected(false),
    visible_lasers(false),
    visible_filters(false)
{
    this->graph_lasers.reserve(size);
}

/*
Returns whether the graph is selected
*/
bool GraphState::isSelected() const {
    return this->selected;
}

/*
Sets the selection state
    :param select: the new selection state
*/
void GraphState::setSelected(bool select){
    this->selected = select;
}

/*
Returns the visibility/drawing state of the lasers
*/
bool GraphState::visibleLasers() const{
    return this->visible_lasers;
}

/*
Sets the visibility/drawing state of the lasers
    :param visible: the state to change to
*/
void GraphState::setVisibleLasers(bool visible){
    this->visible_lasers = visible;
}

/*
Returns the visibility/drawing state of the filters
*/
bool GraphState::visibleFilters() const {
    return this->visible_filters;
}

/*
Sets the visibility/drawing state of the lasers
    :param visible: the state to change to
*/
void GraphState::setVisibleFilters(bool visible){
    this->visible_filters = visible;
}

/*
Returns the laser vector.
*/
const std::vector<Data::Laser>& GraphState::lasers() const {
    return this->graph_lasers;
}

/*
Returns a non-const laser vector, for modification
*/
std::vector<Data::Laser>& GraphState::setLasers(){
    return this->graph_lasers;
}

/*
Returns the laserline the lasers belong to (if applicable, if not returns a nullptr).
*/
const Data::LaserLine* GraphState::laserLine() const {
    return this->graph_laserline;
}

/*
Sets the LaserLine pointer (is allowed to be nullptr)
*/
void GraphState::setLaserLine(const Data::LaserLine* laserline){
    this->graph_laserline = laserline;
}

/* ############################################################################## */

/*
Constructor: construct basic GUI State object
*/
GUI::GUI() {
    this->graphs_state.reserve(this->graph_count_max);
}

/*
Returns the state of the graphs.
(Allows for manipulation to allow graphs themself to manipulate selection states)
*/
std::vector<GraphState>& GUI::graphs(){
    return this->graphs_state;
}

/*
Returns the number of graphs 
*/
std::size_t GUI::graphCount() const {
    return this->graphs_state.size();
}

/*
Adds a graph to the graph state. (If the max size allows for it)
*/
void GUI::addGraph(){
    if(this->graphs_state.size() + 1 >= this->graph_count_max){
        return;
    }
    this->graphs_state.push_back(State::GraphState(this->active_laser, this->active_filter));
}

/*
Remove a graph from the graph state.
If a graph is selected, remove that graph, otherwise the last graph. (If min size allows for it)
*/
void GUI::removeGraph(){
    // Checks if a graph can be removed
    if(this->graphs_state.size() <= 1){
        return;
    }

    // Finds the selected graph
    std::size_t i = this->findSelectedGraph();

    // If no selected graph, remove last one
    if(i >= this->graphs_state.size()){
        i = this->graphs_state.size() - 1;
    }
    auto i_diff = static_cast<std::iterator_traits<State::GraphState*>::difference_type>(i);

    // Remove graph
    this->graphs_state.erase(
        std::next(this->graphs_state.begin(), i_diff),
        std::next(this->graphs_state.begin(), i_diff + 1)
    );
}

/*
Resets the graph state to default by removing all graphs but one, and deselecting all
*/
void GUI::clearGraphs(){
    this->graphs_state.clear();
}

/*
Adds a laser to the selected. If no graphs are selected, adds a laser to the first graph
    :param wavelength: the wavelength to add
    :param instrument: the instrument the laser (might) belong to
*/
void GUI::addLaser(double wavelength, Data::Instrument& instrument){
    // Check if there is any graph
    if(this->graphs_state.empty()){
        return;
    }

    // Find selected graph or set first graph
    std::size_t index = this->findSelectedGraph();
    if(index >= this->graphs_state.size()){
        index = 0;
    }

    // If wavelength is negative clear all wavelength from the indexed graph
    if(wavelength < 0){
        this->graphs_state[index].setLasers().clear();
        this->graphs_state[index].setLaserLine(nullptr);
        return;
    }

    // Find the relevant LaserLine
    auto laserline = instrument.findLaser(wavelength);
    
    // Set the relevant graph to the correct laser/laserline state
    this->graphs_state[index].setLaserLine(laserline.first);

    if(laserline.second){
        this->graphs_state[index].setLasers().clear();
        this->graphs_state[index].setLasers().push_back(*laserline.second);
    }else{
        this->graphs_state[index].setLasers().clear();
        this->graphs_state[index].setLasers().push_back(Data::Laser(wavelength));
    }
}

/*
Fills the graphstate with all laserlines of the instrument
    :param instrument: the instrument to fill into the graphstate
*/
void GUI::fillGraphs(Data::Instrument& instrument){
    this->graphs_state.clear();

    for(std::size_t i=0; i<instrument.optics().size(); ++i){
        this->graphs_state.push_back(State::GraphState(this->active_laser, this->active_filter));
        this->graphs_state[i].setLasers() = instrument.optics()[i].lasers();
        this->graphs_state[i].setLaserLine(&instrument.optics()[i]);
    }
}

/*
Removes the lasers from the selected (otherwise first) graph
*/
void GUI::removeLaser(){
    // Check if there is any graph
    if(this->graphs_state.empty()){
        return;
    }

    // Find selected graph or set first graph
    std::size_t index = this->findSelectedGraph();
    if(index >= this->graphs_state.size()){
        index = 0;
    }

    this->graphs_state[index].setLasers().clear();
    this->graphs_state[index].setLaserLine(nullptr);
}

/*
Finds the (first) selected graph and returns the index. If no graphs are selected
returns a index past the last graph (effectively the size of the vector)
*/
std::size_t GUI::findSelectedGraph() const{
    std::size_t i;
    for(i=0; i<this->graphs_state.size(); ++i){
        if(this->graphs_state[i].isSelected() == true){
            break;
        }
    }
    return i;
}

/*
Sets the laser visibility of all graphs
    :param visible: the visibility state to change to
*/
void GUI::setGraphVisibleLaser(bool visible){
    for(auto& item : this->graphs_state){
        item.setVisibleLasers(visible);
    }
}

/*
Sets the laser visibility of all graphs
    :param visible: the visibility state to change to
*/
void GUI::setGraphVisibleFilter(bool visible){
    for(auto& item : this->graphs_state){
        item.setVisibleFilters(visible);
    }
}

/*
Sets a single graph to be selected
*/
void GUI::setGraphSelect(std::size_t index, bool state){
    for(auto& item : this->graphs_state){
        item.setSelected(false);
    }
    
    if(state){   
        this->graphs_state[index].setSelected(true);
    }
}

} // State namespace
