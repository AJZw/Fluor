/**** General **************************************************************
** Version:    v0.9.5
** Date:       2019-09-12
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "state.h"

#include <QApplication>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>

namespace State {

/*
Constructor: 
    :param factory: 
*/
State::State(Data::Factory& factory) : 
    factory(factory),
    data_fluorophores(),
    data_instruments(),
    style(),
    cache(this->factory, this->data_fluorophores),
    gui(),
    global(),
    instrument()
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

    // Retreive the settings data
    this->syncGlobal();

    // If the style can be loaded, (try to) load the default style
    if(this->factory.isValid(Data::Factory::Styles)){
        this->style.loadStyle(this->factory, this->global.style);
    }
    this->gui.setStyleSheet(this->style.getStyleSheet());

    // Connect the interface to the other components
    QObject::connect(&this->gui, &Main::Controller::screenChanged, this, &State::State::reloadStyle);
    QObject::connect(&this->gui, &Main::Controller::screenDPIChanged, this, &State::State::reloadStyle);

    QObject::connect(&this->gui, &Main::Controller::sendCacheAdd, &this->cache, &Cache::Cache::cacheAdd);
    QObject::connect(&this->gui, &Main::Controller::sendCacheRemove, &this->cache, &Cache::Cache::cacheRemove);
    QObject::connect(&this->gui, &Main::Controller::sendCacheRequestUpdate, &this->cache, &Cache::Cache::cacheRequestUpdate);
    
    QObject::connect(&this->cache, &Cache::Cache::cacheSync, &this->gui, &Main::Controller::receiveCacheSync);
    QObject::connect(&this->cache, &Cache::Cache::cacheUpdate, &this->gui, &Main::Controller::receiveCacheUpdate);

    QObject::connect(&this->gui, &Main::Controller::sendToolbarStateChange, this, &State::State::receiveToolbarState);
    QObject::connect(this, &State::State::sendToolbarState, &this->gui, &Main::Controller::receiveToolbarStateUpdate);

    QObject::connect(this, &State::State::sendCacheState, &this->cache, &Cache::Cache::cacheStateSet);
    QObject::connect(this, &State::State::sendCacheStateExcitation, &this->cache, &Cache::Cache::cacheStateSetExcitation);
    QObject::connect(this, &State::State::sendCacheStateEmission, &this->cache, &Cache::Cache::cacheStateSetEmission);
    QObject::connect(this, &State::State::sendCacheStateSorting, &this->cache, &Cache::Cache::cacheStateSetSorting);

    // Load default settings here
    this->syncInstrument();
    this->syncToolbar();
    this->syncCache();

    // Start GUI
    this->gui.show();

    // Loads the fluorophore data into the GUI
    emit this->gui.sendData(this->data_fluorophores);
    emit this->gui.sendInstrument(this->instrument);
}

/*
Synchronizes the global state to the stored settings dat
*/
void State::syncGlobal() {
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    // Get style
    QString style_id = data->value("DEFAULT/style", this->global.style).toString();
    style_id = data->value("USER/style", style_id).toString();
    this->global.style = style_id;

    // Get sorting mode
    QString sort_mode = data->value("DEFAULT/sort_mode", QString()).toString();
    sort_mode = data->value("USER/sort_mode", sort_mode).toString();
    if(sort_mode == "Additive"){
        this->global.sort_fluorophores = SortMode::Additive;
    }else if(sort_mode == "AdditiveReversed"){
        this->global.sort_fluorophores = SortMode::AdditiveReversed;
    }else if(sort_mode == "Alphabetical"){
        this->global.sort_fluorophores = SortMode::Alphabetical;
    }else if(sort_mode == "AlphabeticalReversed"){
        this->global.sort_fluorophores = SortMode::AlphabeticalReversed;
    }else if(sort_mode == "Excitation"){
        this->global.sort_fluorophores = SortMode::Excitation;
    }else if(sort_mode == "ExcitationReversed"){
        this->global.sort_fluorophores = SortMode::ExcitationReversed;
    }else if(sort_mode == "Emission"){
        this->global.sort_fluorophores = SortMode::Emission;
    }else if(sort_mode == "EmissionReversed"){
        this->global.sort_fluorophores = SortMode::EmissionReversed;
    }
    // If QString isNull() it keeps the hardcoded default
}

/*
Synchronizes the instrument properties
*/
void State::syncInstrument() {
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    QString instrument_id = data->value("USER/instrument", QString()).toString();

    if(!instrument_id.isEmpty()){
        this->instrument = this->data_instruments.getInstrument(this->factory, instrument_id);
    }

}

/*
Synchronizes ALL toolbar states
*/
void State::syncToolbar() {
    emit this->sendToolbarState(Bar::ButtonType::Laser, this->global.active_laser, this->global.enabled_laser);
    emit this->sendToolbarState(Bar::ButtonType::Excitation, this->global.active_excitation, true);
    emit this->sendToolbarState(Bar::ButtonType::Emission, this->global.active_emission, true);
    emit this->sendToolbarState(Bar::ButtonType::Detector, this->global.active_detector, this->global.enabled_detector);

    emit this->sendToolbarState(Bar::ButtonType::GraphAdd, this->global.active_graph_add, true);
    emit this->sendToolbarState(Bar::ButtonType::GraphRemove, this->global.active_graph_remove, true);

    emit this->sendToolbarState(Bar::ButtonType::Lasers, this->global.active_lasers, this->global.enabled_lasers);
}

/*
Synchronizes ALL cache states
*/
void State::syncCache() {
    emit this->sendCacheState({this->global.active_excitation, this->global.active_emission, this->global.sort_fluorophores});
}

/*
Slot: receives toolbar state changes
    :param type: the toolbar buttons that emits the status change
    :param active: whether the button is in active state
    :param enable: whether the button is enabled (if disabled active is saved but irrelevant)
*/
void State::receiveToolbarState(Bar::ButtonType type, bool active, bool enable) {
    switch(type){
    case Bar::ButtonType::Laser:
        this->global.active_laser = active;
        this->global.enabled_laser = enable;
        break;
    case Bar::ButtonType::Excitation:
        this->global.active_excitation = active;
        emit this->sendCacheStateExcitation(active);
        break;
    case Bar::ButtonType::Emission:
        this->global.active_emission = active;
        emit this->sendCacheStateEmission(active);
        break;
    case Bar::ButtonType::Detector:
        this->global.active_detector = active;
        this->global.enabled_detector = enable;
        break;
    case Bar::ButtonType::GraphAdd:
        this->global.active_graph_add = active;
        break;
    case Bar::ButtonType::GraphRemove:
        this->global.active_graph_remove = active;
        break;
    case Bar::ButtonType::Lasers:
        this->global.active_lasers = active;
        this->global.enabled_lasers = enable;
        break;
    default:
        qDebug() << "State::State::receiveToolbarState: GraphAdd/GraphRemove/Lasers not yet implemented";
    }
}

/*
Slot: receives dpi/screen changes and reloads the style
    :param source: widget that needs to reload the style
*/
void State::reloadStyle(QWidget* source){
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
