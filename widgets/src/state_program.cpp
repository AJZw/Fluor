/**** General **************************************************************
** Version:    v0.9.11
** Date:       2020-10-27
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "state_program.h"

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>

namespace State {

/*
Constructor: 
    :param factory: the data file factory
*/
Program::Program(Data::Factory& factory) : 
    factory(factory),
    data_fluorophores(),
    data_instruments(),
    style(),
    instrument(),
    cache(this->factory, this->data_fluorophores),
    state_gui(),
    gui()
{
    // Load fluorophore data
    if(!this->factory.isValid(Data::Factory::Fluorophores)){
        qFatal("State::State: invalid Factory::Fluorophores");
    }
    this->data_fluorophores.load(this->factory);

    // Load instrument data
    if(!this->factory.isValid(Data::Factory::Instruments)){
        qWarning() << "State::State: invalid Factory::Instruments. Uses default";
    }else{
        this->data_instruments.load(this->factory);
    }
    this->retreiveInstrument();

    // Retreive the settings data, needs to happen early to load the proper style information
    this->retreiveStateGUI();

    // If the style can be loaded, (try to) load the default style
    if(this->factory.isValid(Data::Factory::Styles)){
        this->style.loadStyle(this->factory, this->state_gui.style);
    }
    this->gui.setStyleSheet(this->style.getStyleSheet());

    // Connect the interface to the other components
    // DPI/Screen scaling
    QObject::connect(&this->gui, &Main::Controller::screenChanged, this, &State::Program::reloadStyle);
    QObject::connect(&this->gui, &Main::Controller::screenDPIChanged, this, &State::Program::reloadStyle);

    // Data loading
    QObject::connect(this, &State::Program::sendData, &this->gui, &Main::Controller::receiveData);
    QObject::connect(this, &State::Program::sendInstrument, &this->gui, &Main::Controller::receiveInstrument);

    // Cache
    QObject::connect(&this->gui, &Main::Controller::sendCacheAdd, this, &State::Program::receiveCacheAdd);
    QObject::connect(&this->gui, &Main::Controller::sendCacheRemove, this, &State::Program::receiveCacheRemove);
    QObject::connect(&this->gui, &Main::Controller::sendCacheRequestUpdate, this, &State::Program::receiveCacheRequestUpdate);    

    QObject::connect(this, &State::Program::sendCacheState, &this->gui, &Main::Controller::receiveCacheState);
    QObject::connect(this, &State::Program::sendCacheUpdate, &this->gui, &Main::Controller::receiveCacheUpdate);

    // Toolbar
    QObject::connect(&this->gui, &Main::Controller::sendToolbarStateChange, this, &State::Program::receiveToolbarState);
    QObject::connect(this, &State::Program::sendToolbarState, &this->gui, &Main::Controller::receiveToolbarStateUpdate);

    // Graphs
    QObject::connect(this, &State::Program::sendGraphState, &this->gui, &Main::Controller::receiveGraphState);
    QObject::connect(&this->gui, &Main::Controller::sendGraphSelect, this, &State::Program::receiveGraphSelect);

    // Laser selection
    QObject::connect(&this->gui, &Main::Controller::sendLasers, this, &State::Program::receiveLasers);

    // Start GUI
    this->gui.show();

    // Synchronize the program state to the GUI
    this->syncFluorophores();
    this->syncInstrument();
    this->syncCache();
    this->syncToolbar();
    this->syncGraphs();
}

/*
Retreives the GUI state from the stored settings data
*/
void Program::retreiveStateGUI() {
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    // Get style
    QString style_id = data->value("DEFAULT/style", this->state_gui.style).toString();
    style_id = data->value("USER/style", style_id).toString();
    this->state_gui.style = style_id;

    // Get sorting mode
    QString sort_mode = data->value("DEFAULT/sort_mode", QString()).toString();
    sort_mode = data->value("USER/sort_mode", sort_mode).toString();
    if(sort_mode == "Additive"){
        this->state_gui.sort_fluorophores = SortMode::Additive;
    }else if(sort_mode == "AdditiveReversed"){
        this->state_gui.sort_fluorophores = SortMode::AdditiveReversed;
    }else if(sort_mode == "Alphabetical"){
        this->state_gui.sort_fluorophores = SortMode::Alphabetical;
    }else if(sort_mode == "AlphabeticalReversed"){
        this->state_gui.sort_fluorophores = SortMode::AlphabeticalReversed;
    }else if(sort_mode == "Excitation"){
        this->state_gui.sort_fluorophores = SortMode::Excitation;
    }else if(sort_mode == "ExcitationReversed"){
        this->state_gui.sort_fluorophores = SortMode::ExcitationReversed;
    }else if(sort_mode == "Emission"){
        this->state_gui.sort_fluorophores = SortMode::Emission;
    }else if(sort_mode == "EmissionReversed"){
        this->state_gui.sort_fluorophores = SortMode::EmissionReversed;
    }
    // If QString isNull() it keeps the hardcoded default

}

/*
Retreive the instrument properties from data files and load into the state
*/
void Program::retreiveInstrument() {
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    QString instrument_id = data->value("USER/instrument", QString()).toString();

    this->loadInstrument(instrument_id);
}

/*
Synchronizes the GUI button with the GUI state (needed by the toolbar controller)
*/
void Program::syncToolbar() {
    emit this->sendToolbarState(Bar::ButtonType::Laser, this->state_gui.active_laser, this->state_gui.enabled_laser);
    emit this->sendToolbarState(Bar::ButtonType::Excitation, this->state_gui.active_excitation, true);
    emit this->sendToolbarState(Bar::ButtonType::Emission, this->state_gui.active_emission, true);
    emit this->sendToolbarState(Bar::ButtonType::Filter, this->state_gui.active_filter, this->state_gui.enabled_filter);

    emit this->sendToolbarState(Bar::ButtonType::GraphAdd, true, this->state_gui.active_graph_add);
    emit this->sendToolbarState(Bar::ButtonType::GraphRemove, true, this->state_gui.active_graph_remove);

    emit this->sendToolbarState(Bar::ButtonType::LaserLines, this->state_gui.active_laserlines, this->state_gui.enabled_laserlines);
}

/*
Synchronizes the graphs state to the GUI (needed by the graph controller)
*/
void Program::syncGraphs() {
    emit this->sendGraphState(this->state_gui.graphs());
}

/*
Synchronizes the cache state to the GUI state (needed by the fluor scrollarea)
*/
void Program::syncCache() {
    this->cache.setSettings({this->state_gui.active_excitation, this->state_gui.active_emission, this->state_gui.sort_fluorophores});

    // Need to synchronize as the sort order can be changed
    emit this->sendCacheState(this->cache.state());
}

/*
Synchronizes the fluorophore to the GUI (needed by the fluor lineedit)
*/
void Program::syncFluorophores() {
    emit this->sendData(this->data_fluorophores);
}

/*
Synchronizes the instrument to the GUI (needed by the laser lineedit)
*/
void Program::syncInstrument() {
    emit this->sendInstrument(this->instrument);
}

/*
Loads the instrument of the specified id into the program
    :param instrument_id: the instrument to load, if instrument is not found or string is empty unloads the instrument
*/
void Program::loadInstrument(const QString& instrument_id){
    if(instrument_id.isEmpty()){
        this->instrument = Data::Instrument();
    }else{
        this->instrument = this->data_instruments.getInstrument(instrument_id);
    }

    // Synchronize the toolbar buttons to the state of the instrument
    if(this->instrument.isEmpty()){
        this->state_gui.enabled_filter = false;
        this->state_gui.active_filter = true;

        this->state_gui.enabled_laserlines = false;
        this->state_gui.active_laserlines = true;
    }else{
        this->state_gui.enabled_filter = true;
        this->state_gui.active_filter = true;

        this->state_gui.enabled_laserlines = true;
        this->state_gui.active_laserlines = true;
    }

    // Reset the amount of graphs to default
    this->state_gui.clearGraphs();
    this->state_gui.addGraph();
    this->refreshToolbar();
}

/*
Recalculates add/remove buttons of the the toolbar
*/
void Program::refreshToolbar(){
    if(this->instrument.isEmpty()) { 
        if(this->state_gui.graphCount() >= this->state_gui.graph_count_max){
            this->state_gui.active_graph_add = false;
        }else{
            this->state_gui.active_graph_add = true;
        }    
    }else{
        if(this->state_gui.graphCount() >= this->instrument.optics().size()){
            this->state_gui.active_graph_add = false;
        }else{
            this->state_gui.active_graph_add = true;
        }
    }

    if(this->state_gui.graphCount() > 1){
        this->state_gui.active_graph_remove = true;
    }else{
        this->state_gui.active_graph_remove = false;
    }
}

/*
Slot: receives toolbar state changes
    :param type: the toolbar buttons that emits the status change
    :param active: whether the button is in active state
    :param enable: whether the button is enabled (if disabled active is saved but irrelevant)
*/
void Program::receiveToolbarState(Bar::ButtonType type, bool active, bool enable) {
    switch(type){
    case Bar::ButtonType::Laser:
        this->state_gui.active_laser = active;
        this->state_gui.enabled_laser = enable;
        this->state_gui.setGraphVisibleLaser(active);
        this->syncGraphs();
        break;

    case Bar::ButtonType::Excitation:
        this->state_gui.active_excitation = active;
        this->cache.setSettingsExcitation(active);
        emit this->sendCacheUpdate();
        break;

    case Bar::ButtonType::Emission:
        this->state_gui.active_emission = active;
        this->cache.setSettingsEmission(active);
        emit this->sendCacheUpdate();
        break;

    case Bar::ButtonType::Filter:
        this->state_gui.active_filter = active;
        this->state_gui.enabled_filter = enable;
        this->state_gui.setGraphVisibleFilter(active);
        this->syncGraphs();
        break;

    case Bar::ButtonType::GraphAdd:
        this->state_gui.addGraph();
        this->refreshToolbar();
        this->syncToolbar();
        this->syncGraphs();
        // make sure to sync the cache to draw the spectra in the new graph
        this->syncCache();
        break;

    case Bar::ButtonType::GraphRemove:
        this->state_gui.removeGraph();
        this->refreshToolbar();
        this->syncToolbar();
        this->syncGraphs();
        break;

    case Bar::ButtonType::LaserLines:
        this->state_gui.active_laserlines = active;
        this->state_gui.enabled_laserlines = enable;
        if(!active){
            // Plot all laserlines
            this->state_gui.fillGraphs(this->instrument);
        }else{
            // Remove all laserlines
            this->state_gui.clearGraphs();
            this->state_gui.addGraph();
        }
        this->syncGraphs();
        this->syncCache();
        this->refreshToolbar();
        this->syncToolbar();

        break;
    default:
        qDebug() << "State::State::receiveToolbarState: GraphAdd/GraphRemove/Lasers not yet implemented";
    }
}

/*
Slot: receives a Laser signal and forwards it to the state_gui
    :param wavelength: the laser wavelength (zero value means no selection, negative value means removal of laser)
*/
void Program::receiveLasers(std::vector<Data::LaserID>& lasers){
    // Let the GUI add the lasers
    this->state_gui.addLasers(lasers, this->instrument);
    
    emit this->sendGraphState(this->state_gui.graphs());
}

/*
Slot: receives a Cache add signal, connects the signal to the cache and handles synchronisation with GUI
*/
void Program::receiveCacheAdd(std::vector<Data::FluorophoreID>& fluorophores){
    this->cache.add(fluorophores);

    // Synchronize
    emit this->sendCacheState(this->cache.state());
}

/*
Slot: receives a Cache remove signal, connects the signal to the cache and handles synchronisation with GUI
*/
void Program::receiveCacheRemove(std::vector<Data::FluorophoreID>& fluorophores){
    this->cache.remove(fluorophores);

    // Synchronize
    emit this->sendCacheState(this->cache.state());
}

/*
Slot: receives a Cache request sync signal, forces GUI synchronisation
*/
void Program::receiveCacheRequestSync(){
    emit this->sendCacheState(this->cache.state());
}

/*
Slot: receives a Cache request update signal, forces GUI update
*/
void Program::receiveCacheRequestUpdate(){
    emit this->sendCacheUpdate();
}

/*
Slot: receive Graph select signal and forwards to the state_gui
    :param index: the graph index that sends the signal
    :param state: the select state
*/
void Program::receiveGraphSelect(std::size_t index, bool state){
    this->state_gui.setGraphSelect(index, state);

    emit this->sendGraphState(this->state_gui.graphs());
}

/*
Slot: receives dpi/screen changes and reloads the style
    :param source: widget that needs to reload the style
*/
void Program::reloadStyle(QWidget* source){
    if(source){
        QFontMetrics metrics = QFontMetrics(source->font(), source);
        this->style.buildStyleSheet(metrics);

        source->setStyleSheet(this->style.getStyleSheet());
    }else{
        QFontMetrics metrics = QFontMetrics(qApp->font());
        this->style.buildStyleSheet(metrics);

        source->setStyleSheet(this->style.getStyleSheet());
    }
}

} // State namespace
