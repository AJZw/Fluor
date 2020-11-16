/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "state_program.h"
#include "general_widgets.h"

#include <QApplication>
#include <QWindow>
#include <QScreen>
#include <QDesktopWidget>
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
        qWarning() << "State::State: invalid Factory::Fluorophores";
    }else{
        this->data_fluorophores.load(this->factory);
    }

    // Load instrument data
    if(!this->factory.isValid(Data::Factory::Instruments)){
        qWarning() << "State::State: invalid Factory::Instruments";
    }else{
        this->data_instruments.load(this->factory);
    }
    this->retreiveInstrument();

    // Set global font size (for proper dpi scaling) (relevant if the primary screen is scaled)
    qreal dpi_scale = qApp->screens()[qApp->desktop()->primaryScreen()]->logicalDotsPerInch() / 96.0;
    QFont font = qApp->font();
    font.setPointSizeF(font.pointSizeF() * dpi_scale);
    qApp->setFont(font);

    // Connect the interface to the other components
    // DPI/Screen scaling
    QObject::connect(&this->gui, &Main::Controller::screenChanged, this, &State::Program::reloadStyle);
    QObject::connect(&this->gui, &Main::Controller::screenDPIChanged, this, &State::Program::reloadStyle);

    // Program exiting
    QObject::connect(&this->gui, &Main::Controller::closed, this, &State::Program::closedWindow);

    // Data loading
    QObject::connect(this, &State::Program::sendFluorophores, &this->gui, &Main::Controller::receiveFluorophores);
    QObject::connect(this, &State::Program::sendInstrument, &this->gui, &Main::Controller::receiveInstrument);
    QObject::connect(this, &State::Program::sendInstruments, &this->gui, &Main::Controller::receiveInstruments);
    QObject::connect(this, &State::Program::sendStyles, &this->gui, &Main::Controller::receiveStyles);

    // MenuBar
    QObject::connect(this, &State::Program::sendMenuBarState, &this->gui, &Main::Controller::receiveMenuBarStateUpdate);
    QObject::connect(&this->gui, &Main::Controller::sendMenuBarStateChange, this, &State::Program::receiveMenuBarState);

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

    // Retreive the GUI State data
    this->retreiveGUIState();

    // (Try to) load the default style
    this->loadStyle(this->state_gui.style);

    // Start GUI
    this->gui.show();

    // Retreive the GUI position data
    this->retreiveGUIPosition();

    // Synchronize the program state to the GUI
    this->syncFluorophores();
    this->syncInstruments();
    this->syncStyles();
    this->syncInstrument();
    this->syncStyle();
    this->syncOptions();
    this->syncCache();
    this->syncToolbar();
    this->syncGraphs();
}

/*
Retreives the GUI state from the stored settings data. Load this data before setting a stylesheet.
*/
void Program::retreiveGUIState() {
    if(!this->factory.isValid(Data::Factory::Settings)){
        qWarning() << "Program::retreiveGUIState: invalid Data::Factory::Settings - cannot load GUI state";
        // Load default style
        this->loadStyle(this->state_gui.style);
        return;
    }
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
Retreives the screen, size, and position of the GUI. Load this data after showing the GUI.
*/
void Program::retreiveGUIPosition() {
    if(!this->factory.isValid(Data::Factory::Settings)){
        qWarning() << "Program::retreiveGUIPosition: invalid Data::Factory::Settings - cannot load position";
        return;
    }
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);
    
    // Get dimensions of the gui window and screen
    bool screen_reset = false;
    int index = data->value("USER/screen_i", 0).toInt();
    int screen_x = data->value("USER/screen_x", 0).toInt();
    int screen_y = data->value("USER/screen_y", 0).toInt();
    int screen_width = data->value("USER/screen_width", 0).toInt();
    int screen_height = data->value("USER/screen_height", 0).toInt();
    
    int window_x = data->value("USER/window_x", QApplication::style()->pixelMetric(QStyle::PM_DefaultFrameWidth)).toInt();
    int window_y = data->value("USER/window_y", QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight)).toInt();
    int window_width = data->value("USER/window_width", data->value("DEFAULT/window_width", 0)).toInt();
    int window_height = data->value("USER/window_height", data->value("DEFAULT/window_height", 0)).toInt();
    bool maximized = data->value("USER/window_maximized", false).toBool();

    // Make sure the stored window exists (this to prevent accidental placement of the window outside the visible area)
    QList<QScreen*> screens = static_cast<QApplication*>(QApplication::instance())->screens();
    QScreen* screen = nullptr;
    if(index < 0 || index >= screens.length()){
        screen_reset = true;
    }else{
        screen = screens[index];
        if(screen->availableGeometry().x() != screen_x){
            screen_reset = true;
        }else if(screen->availableGeometry().y() != screen_y){
            screen_reset = true;
        }else if(screen->availableGeometry().width() != screen_width){
            screen_reset = true;
        }else if(screen->availableGeometry().height() != screen_height){
            screen_reset = true;
        }
    }

    if(screen_reset){
        index = static_cast<QApplication*>(QApplication::instance())->desktop()->primaryScreen();
        screen = screens[index];
        window_x = QApplication::style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
        window_y = QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight);
        window_width = data->value("DEFAULT/window_width", 0).toInt();
        window_height = data->value("DEFAULT/window_height", 0).toInt();
        maximized = false;
    }

    this->gui.windowHandle()->setScreen(screen);
    // Move between two screens of different DPI causes geometry move/resize. Therefor move first, then set proper geometry.
    // This will cause a geometry error. But setGeometry after move will atleast work as expected.
    this->gui.move(
        window_x + QApplication::style()->pixelMetric(QStyle::PM_DefaultFrameWidth),
        window_y + QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight)
    );
    this->gui.setGeometry(window_x, window_y, window_width, window_height);

    if(maximized){
        this->gui.setWindowState(this->gui.windowState() ^ Qt::WindowState::WindowMaximized);
    }
}

/*
Retreive the instrument properties from data files and load into the state
*/
void Program::retreiveInstrument() {
    if(!this->factory.isValid(Data::Factory::Settings)){
        qWarning() << "Program::retreiveInstrument: invalid Data::Factory::Settings - cannot load instrument";
        // Load backup - empty Instrument
        this->loadInstrument(QString());
        return;
    }
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    QString instrument_id = data->value("USER/instrument", QString()).toString();

    this->loadInstrument(instrument_id);
}

/*
Stores the gui state to the settings.ini file.
*/
void Program::storeStateGUI() {
    if(!this->factory.isValid(Data::Factory::Settings)){
        qWarning() << "Program::storeStateGUI: invalid Data::Factory::Settings - cannot save settings";
        return;
    }
    std::unique_ptr<QSettings> data = this->factory.get(Data::Factory::Settings);

    data->beginGroup("USER");

    // Store screen properties to make sure the window size settings work for the current screen
    QScreen* screen = this->gui.windowHandle()->screen();
    // Find screen index to properly manage multi-screen systems
    QList<QScreen*> screens = static_cast<QApplication*>(QApplication::instance())->screens();
    int index;
    for(index=0; index < screens.length(); ++index){
        if(screens[index] == screen){
            break;
        }
    }

    data->setValue("screen_i", index);
    data->setValue("screen_x", screen->availableGeometry().x());
    data->setValue("screen_y", screen->availableGeometry().y());
    data->setValue("screen_width", screen->availableGeometry().width());
    data->setValue("screen_height", screen->availableGeometry().height());
    data->setValue("window_x", this->gui.geometry().x());
    data->setValue("window_y", this->gui.geometry().y());
    data->setValue("window_width", this->gui.geometry().width());
    data->setValue("window_height", this->gui.geometry().height());
    data->setValue("window_maximized", this->gui.isMaximized());

    data->setValue("style", this->state_gui.style);
    data->setValue("instrument", this->instrument.id());

    switch(this->state_gui.sort_fluorophores){
        case SortMode::Additive:{
            data->setValue("sort_mode", "Additive");
            break;
        }
        case SortMode::AdditiveReversed:{
            data->setValue("sort_mode", "AdditiveReversed");
            break;
        }
        case SortMode::Alphabetical:{
            data->setValue("sort_mode", "Alphabetical");
            break;
        }
        case SortMode::AlphabeticalReversed:{
            data->setValue("sort_mode", "AlphabeticalReversed");
            break;
        }
        case SortMode::Excitation:{
            data->setValue("sort_mode", "Excitation");
            break;
        }
        case SortMode::ExcitationReversed:{
            data->setValue("sort_mode", "ExcitationReversed");
            break;
        }
        case SortMode::Emission:{
            data->setValue("sort_mode", "Emission");
            break;
        }
        case SortMode::EmissionReversed:{
            data->setValue("sort_mode", "EmissionReversed");
            break;
        }
        default:
            break;
    }

    data->endGroup();
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
    emit this->sendFluorophores(this->data_fluorophores);
}

/*
Synchronizes the instrument to the GUI (needed by the laser lineedit & instrument menu)
*/
void Program::syncInstrument() {
    emit this->sendInstrument(this->instrument);
    emit this->sendMenuBarState(Main::MenuBarAction::InstrumentID, this->instrument.id());
}

/*
Synchronizes the list of instruments to the GUI (needed for the instrument menu)
*/
void Program::syncInstruments() {
    emit this->sendInstruments(this->data_instruments);
}

/*
Synchronizes the style to the GUI (needed for options->styles menu)
*/
void Program::syncStyle() {
    emit this->sendMenuBarState(Main::MenuBarAction::StyleID, this->style.id());
}

/*
Synchronizes the list of styles to the GUI (needed for the options->styles menu)
*/
void Program::syncStyles() {
    if(this->factory.isValid(Data::Factory::Styles)){
        emit this->sendStyles(this->style.getStyleIDs(this->factory));
    }else{
        qWarning() << "Program::loadStyle: invalid Data::Factory::Styles - cannot load styles";
    }
}

/*
Synchronizes the options to the GUI (needed for options menu)
*/
void Program::syncOptions() {
    emit this->sendMenuBarState(Main::MenuBarAction::SortOrder, static_cast<int>(this->state_gui.sort_fluorophores));
}

/*
Loads the style of the specified id into the program (if possible)
*/
void Program::loadStyle(const QString& style_id){
    this->state_gui.style = style_id;

    if(this->factory.isValid(Data::Factory::Styles)){
        this->style.loadStyle(this->factory, this->state_gui.style);
    }else{
        qWarning() << "Program::loadStyle: invalid Data::Factory::Styles - cannot load style" << style_id;
    }

    this->gui.setStyleSheet(this->style.getStyleSheet());
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

        if(!this->state_gui.enabled_laserlines){
            this->state_gui.enabled_laserlines = true;
            this->state_gui.active_laserlines = true;
        }
    }

    // Reset the amount of graphs to 1 or if laserlines button is inactive fill graphs
    this->state_gui.clearGraphs();
    if(!this->state_gui.active_laserlines){
        this->state_gui.fillGraphs(this->instrument);
    }else{
        this->state_gui.addGraph();
    }
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
Slot: receives MenuBar state changes
    :param action: the menubar action that emit the state
    :param id: (optional) the selected identifier
*/
void Program::receiveMenuBarState(Main::MenuBarAction action, const QVariant& id){
    switch(action){
        case Main::MenuBarAction::Exit:{
            qDebug() << "Program::receiveMenuBar: exit";
            this->storeStateGUI();
            static_cast<QApplication*>(QApplication::instance())->quit();
            break;
        }
        case Main::MenuBarAction::InstrumentID:{
            qDebug() << "Program::receiveMenuBar: select instrument:" << id.toString();
            // Check if instrument is already loaded
            QString instrument_id = id.toString();
            if(this->instrument.id() == instrument_id){
                return;
            }
            // Synchronized the instrument
            this->loadInstrument(instrument_id);
            this->syncInstrument();
            // Update toolbar to adhere to the new laserlines 
            this->syncToolbar();
            // Make sure the necessary graphs exist
            this->syncGraphs();
            // Make sure to update the fluorophores in all (potentially new) graphs
            this->sendCacheState(this->cache.state());
            break;
        }
        case Main::MenuBarAction::SortOrder:{
            qDebug() << "Program::receiveMenuBar: set sort order" << id.toInt();
            // Check if sorting mode is already applied
            State::SortMode mode = static_cast<State::SortMode>(id.toInt());
            if(this->state_gui.sort_fluorophores == mode){
                return;
            }
            this->state_gui.sort_fluorophores = mode;
            this->syncCache();
            break;
        }
        case Main::MenuBarAction::StyleID:{
            qDebug() << "Program::receiveMenuBar: select style:" << id.toString();
            // Check if style is already loaded
            QString style_id = id.toString();
            if(this->state_gui.style == style_id){
                return;
            }
            this->loadStyle(style_id);
            this->syncStyle();
            break;
        }
        case Main::MenuBarAction::About:{
            General::AboutWindow* about = new General::AboutWindow();
            about->setStyleSheet(this->style.getStyleSheet());
            QObject::connect(about, &General::AboutWindow::screenChanged, this, &State::Program::reloadStyle);
            QObject::connect(about, &General::AboutWindow::screenDPIChanged, this, &State::Program::reloadStyle);
            about->show();
            break;
        }
        case Main::MenuBarAction::SaveAs:
        case Main::MenuBarAction::Open:
        case Main::MenuBarAction::Print:
        default:
            break;
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
    }
}

/*
Slot: receives the QMainWindow::close event of window and makes sure the data is stored for further use
*/
void Program::closedWindow(const QWidget* source){
    Q_UNUSED(source);
    this->storeStateGUI();
    static_cast<QApplication*>(QApplication::instance())->exit();
}

} // State namespace
