/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_controller.h"
#include "general_widgets.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include "fluor_lineedit.h"
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QScrollBar>

#include <QObjectList>

#include <QDebug>


namespace Fluor {

/*
Initializer: Builds and connects the Fluorophore menu
    :parent: parent widget
*/
Controller::Controller(QWidget* parent) :
    QWidget(parent)
{
    this->setContentsMargins(0, 0, 0, 0);

    // Set layout
    QVBoxLayout* controller_layout = new QVBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    controller_layout->setSpacing(6);

    this->setLayout(controller_layout);

    // Add buttons
    Fluor::PushButton* widget_button = new Fluor::PushButton(this);
    Fluor::LineEdit* widget_lineedit = new Fluor::LineEdit(this);
    Fluor::ScrollController* widget_scrollarea = new Fluor::ScrollController(this);
    widget_lineedit->hide();

    controller_layout->addWidget(widget_button, 0);
    controller_layout->addWidget(widget_lineedit, 0);
    controller_layout->addWidget(widget_scrollarea, 1);

    // Connect internal signals and slots
    // FluorPushButton and FluorLineEdit connections
    QObject::connect(widget_button, &Fluor::PushButton::clicked, this, &Fluor::Controller::clickedPushButton);
    QObject::connect(widget_lineedit, &Fluor::LineEdit::finished, this, &Fluor::Controller::finishedLineEdit);
    QObject::connect(this, &Fluor::Controller::showPushButton, widget_button, &Fluor::PushButton::showButton);
    QObject::connect(this, &Fluor::Controller::hidePushButton, widget_button, &Fluor::PushButton::hideButton);
    QObject::connect(this, &Fluor::Controller::showLineEdit, widget_lineedit, &Fluor::LineEdit::showButton);
    QObject::connect(this, &Fluor::Controller::hideLineEdit, widget_lineedit, &Fluor::LineEdit::hideButton);

    // Forwards the events to/from LineEdit
    QObject::connect(this, &Fluor::Controller::sendGlobalEvent, widget_lineedit, &Fluor::LineEdit::unfocus);
    QObject::connect(this, &Fluor::Controller::sendData, widget_lineedit, &Fluor::LineEdit::reloadData);
    QObject::connect(this, &Fluor::Controller::sendGlobalSize, widget_lineedit, &Fluor::LineEdit::reloadSize);
    QObject::connect(widget_lineedit, &Fluor::LineEdit::output, this, &Fluor::Controller::receiveCacheAdd);
    
    // Forward events to/from ScrollController
    QObject::connect(this, &Fluor::Controller::sendCacheSync, widget_scrollarea, &Fluor::ScrollController::syncButtons);
    QObject::connect(this, &Fluor::Controller::sendCacheUpdate, widget_scrollarea, &Fluor::ScrollController::updateButtons);
    QObject::connect(this, &Fluor::Controller::sendCacheSync, widget_lineedit, &Fluor::LineEdit::sync);
    QObject::connect(widget_scrollarea, &Fluor::ScrollController::sendCacheRequestUpdate, this, &Fluor::Controller::receiveCacheRequestUpdate);
    QObject::connect(widget_scrollarea, &Fluor::ScrollController::sendRemove, this, &Fluor::Controller::receiveCacheRemove);
}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void Controller::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString Controller::layoutSpacing() const {
    return QString::number(this->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void Controller::setLayoutSpacing(QString layout_spacing){
    this->layout()->setSpacing(layout_spacing.toInt());
}

/*
Slot: unfocus the widget
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/*
Slot: reload the data of the widget
*/
void Controller::receiveData(const Data::FluorophoreReader& data){
    emit this->sendData(data);
}

/*
Slot: reload the max size of a (ListView) widget
*/
void Controller::receiveGlobalSize(const QWidget* widget){
    emit this->sendGlobalSize(widget);
}

/*
Slot: forwards the request to the cache to update
*/
void Controller::receiveCacheRequestUpdate(){
    emit this->sendCacheRequestUpdate();
}

/*
Slot: receives and sends output from the internal lineedit and forwards towards cache
*/
void Controller::receiveCacheAdd(std::vector<Data::FluorophoreID>& output){
    emit this->sendCacheAdd(output);
}

/*
Slot: receives and sends output from the scroll/button controllers and forwards towards cache
*/
void Controller::receiveCacheRemove(std::vector<Data::FluorophoreID>& output){
    emit this->sendCacheRemove(output);
}

/*
Slot: receives and sends cache's synchronisation state to the scrollcontroller
*/
void Controller::receiveCacheSync(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheSync(cache_state);
}

/*
Slot: receives and sends cache's update state to the scrollcontroller
*/
void Controller::receiveCacheUpdate(const std::vector<Cache::CacheID>& cache_state){
    emit this->sendCacheUpdate(cache_state);
}

/*
Slot: slot for FluorPushButton::clicked, hides PushButton and show LineEdit
*/
void Controller::clickedPushButton(bool checked) {
    Q_UNUSED(checked);
    emit this->hidePushButton();
    emit this->showLineEdit();
}

/*
Slot: slot for FluorLineEdit::finished, hides LineEdit and shows Pushbutton
*/
void Controller::finishedLineEdit() {
    emit this->hideLineEdit();
    emit this->showPushButton();
}

// ####################################################################################### //

/*
Contructor: builds the fluorophore menu scrollarea
*/
ScrollController::ScrollController(QWidget* parent) :
    QScrollArea(parent),
    button_widgets(),
    margin_scrollbar(0)
{
    // Set Scrollarea properties
    this->setFocusPolicy(Qt::NoFocus);
    this->setWidgetResizable(true);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setFrameStyle(QFrame::NoFrame);

    // Setup widget and layout inside the scrollarea
    QWidget* widget_internal = new QWidget(this);

    QVBoxLayout* layout_internal = new QVBoxLayout(this);
    layout_internal->setContentsMargins(0, 0, 0, 0);
    layout_internal->setSpacing(6);
    layout_internal->insertStretch(INT_MAX, 99);

    widget_internal->setLayout(layout_internal);
    this->setWidget(widget_internal);

    General::ScrollBar* vertical_scrollbar = new General::ScrollBar(this);
    this->setVerticalScrollBar(vertical_scrollbar);

    QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::showing, this, &Fluor::ScrollController::showingScrollBar);
    QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::hiding, this, &Fluor::ScrollController::hidingScrollBar);

    // Reserve vector space
    this->button_widgets.reserve(20);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString ScrollController::layoutMarginsScrollBar() const {
    return QString::number(this->margin_scrollbar, 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void ScrollController::setLayoutMarginsScrollBar(QString layout_spacing_scroll_bar){
    this->margin_scrollbar = layout_spacing_scroll_bar.toInt();
    if(this->verticalScrollBar()->isVisible()){
        this->widget()->layout()->setContentsMargins(0, 0, this->margin_scrollbar, 0);
    }else{
        this->widget()->layout()->setContentsMargins(0, 0, 0, 0);
    }
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString ScrollController::layoutSpacing() const {
    return QString::number(this->widget()->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void ScrollController::setLayoutSpacing(QString layout_spacing){
    this->widget()->layout()->setSpacing(layout_spacing.toInt());
}

/*
Receives hiding signal from the vertical scrollbar and removes scrollbar margin
*/
void ScrollController::hidingScrollBar(){
    this->widget()->layout()->setContentsMargins(0, 0, 0, 0);
}

/*
Receives showing signal from the vertical scrollbar and adds scrollbar margin
*/
void ScrollController::showingScrollBar(){
    this->widget()->layout()->setContentsMargins(0, 0, this->margin_scrollbar, 0);
}

/*
Slot: Synchronizes the internal ButtonsController widgets to the cache_state. Adds/removes the necessary
ButtonsController and resets the name, id, cache pointers.
    :param cache_state: the to-be-synced ids
*/
void ScrollController::syncButtons(const std::vector<Cache::CacheID>& cache_state){
    // qDebug() << "ScrollController::syncButtons:" << this->button_widgets.size() << ":" << inputs.size();
    
    // First add/remove to requested buttonControllers count
    if(this->button_widgets.size() < cache_state.size()){
        for(std::size_t i=this->button_widgets.size(); i<cache_state.size(); ++i){
            // Build new widget
            ButtonsController* widget = new ButtonsController(this->widget());
            static_cast<QVBoxLayout*>(this->widget()->layout())->insertWidget(static_cast<int>(i), widget);

            QObject::connect(widget, &Fluor::ButtonsController::requestUpdate, this, &Fluor::ScrollController::receiveCacheRequestUpdate);
            QObject::connect(widget, &Fluor::ButtonsController::sendRemove, this, &Fluor::ScrollController::receiveRemove);
            QObject::connect(widget, &Fluor::ButtonsController::sendSelected, this, &Fluor::ScrollController::receiveSelected);

            // Add to lookup vector
            this->button_widgets.push_back(widget);
        }
    }else if(this->button_widgets.size() > cache_state.size()){
        for(std::size_t i=this->button_widgets.size(); i>cache_state.size(); --i){
            // Get last widget and remove it
            ButtonsController* widget = this->button_widgets[i-1];

            QObject::disconnect(widget, &Fluor::ButtonsController::requestUpdate, this, &Fluor::ScrollController::receiveCacheRequestUpdate);
            QObject::disconnect(widget, &Fluor::ButtonsController::sendRemove, this, &Fluor::ScrollController::receiveRemove);
            QObject::disconnect(widget, &Fluor::ButtonsController::sendSelected, this, &Fluor::ScrollController::receiveSelected);

            delete widget;

            this->button_widgets.pop_back();
        }
    }

    // Now sync the ButtonsControllers
    for(std::size_t i=0; i<this->button_widgets.size(); ++i){
        this->button_widgets[i]->syncButtons(cache_state[i]);
    }
}

/*
Slot: Updates the internal ButtonsController widgets to the cache_state. Assumes synchronized state. Doesnt add or remove widgets
    :param cache_state: the state to update to
*/
void ScrollController::updateButtons(const std::vector<Cache::CacheID>& cache_state){
    for(std::size_t i=0; i<this->button_widgets.size(); ++i){
        this->button_widgets[i]->updateButtons(cache_state[i]);
    }
}

/*
Slot: forwards the request to the cache to update
*/
void ScrollController::receiveCacheRequestUpdate(){
    emit this->sendCacheRequestUpdate();
}

/*
Slot: receives and forwards remove fluorophore request
*/
void ScrollController::receiveRemove(std::vector<Data::FluorophoreID>& fluorophores){
    emit this->sendRemove(fluorophores);
}

/*
Slot: receives selected signal. Scrolls the scrollarea to show the widget
    :param widget: the widget to ensure visibility
*/
void ScrollController::receiveSelected(QWidget* widget){
    if(!this->underMouse()){
        this->ensureWidgetVisible(widget, 0, 2);
    }
}

// ####################################################################################### //

/*
Constructor: Set of emission/excitation visibility toggle buttons, and a removal button for one fluorophore ID
*/
ButtonsController::ButtonsController(QWidget* parent) :
    QWidget(parent),
    widget_emission(nullptr),
    widget_excitation(nullptr),
    widget_remove(nullptr),
    id(""),
    name(""),
    data(nullptr)
{
    this->setContentsMargins(0, 0, 0, 0);

    // Set layout
    QHBoxLayout* controller_layout = new QHBoxLayout(this);
    controller_layout->setContentsMargins(0, 0, 0, 0);
    controller_layout->setSpacing(3);
    this->setLayout(controller_layout);

    // Add buttons
    this->widget_emission = new Fluor::EmissionButton(this);
    this->widget_excitation = new Fluor::ExcitationButton(this);
    this->widget_remove = new Fluor::RemoveButton(this);

    controller_layout->addWidget(widget_emission, 1);
    controller_layout->addWidget(widget_excitation, 0);
    controller_layout->addWidget(widget_remove, 0);

    QObject::connect(this->widget_emission, &Fluor::EmissionButton::clicked, this, &Fluor::ButtonsController::receiveEmissionClick);
    QObject::connect(this->widget_excitation, &Fluor::ExcitationButton::clicked, this, &Fluor::ButtonsController::receiveExcitationClick);
    QObject::connect(this->widget_remove, &Fluor::RemoveButton::clicked, this, &Fluor::ButtonsController::receiveRemoveClick);

    QObject::connect(this->widget_emission, &Fluor::EmissionButton::hoverEntered, this, &Fluor::ButtonsController::hoverEntered);
    QObject::connect(this->widget_emission, &Fluor::EmissionButton::hoverLeaved, this, &Fluor::ButtonsController::hoverLeaved);
    QObject::connect(this->widget_emission, &Fluor::EmissionButton::selected, this, &Fluor::ButtonsController::receiveSelected);
}

/*
Reimplementation of paintEvent. This allows proper stylesheet management of QWidget inheriting widget
    :param event: paintevent
*/
void ButtonsController::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QStyleOption style_option;
    style_option.initFrom(this);
    QPainter painter(this);
    this->style()->drawPrimitive(QStyle::PE_Widget, &style_option, &painter, this);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString ButtonsController::layoutSpacing() const {
    return QString::number(this->layout()->spacing(), 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void ButtonsController::setLayoutSpacing(QString layout_spacing){
    this->layout()->setSpacing(layout_spacing.toInt());
}

/*
Sync the buttonsController and internal widgets with a CacheID
    :param cache_state: cacheid (assumed to have a valid data pointer)
*/
void ButtonsController::syncButtons(const Cache::CacheID& cache_state){
    this->id = cache_state.id;
    this->name = cache_state.name;
    this->data = cache_state.data;

    this->widget_emission->setText(this->name);

    // Finally update the button active state
    this->updateButtons(cache_state);
}

/*
Update the buttonsController and internal widgets with the CacheID
    :param cache_state: the state of the cache
*/
void ButtonsController::updateButtons(const Cache::CacheID& cache_state){
    //qDebug() << cache_state.data->visibleExcitation() << cache_state.data->visibleEmission();
    this->widget_excitation->setActive(cache_state.data->visibleExcitation());
    this->widget_emission->setActive(cache_state.data->visibleEmission());
    this->widget_emission->setSelect(cache_state.data->selectEmission());
}

/*
Slot: receives the click from the emission button. Forwards the change to the relevant cache/spectrum object
*/
void ButtonsController::receiveEmissionClick(bool active){
    // Just constructed, not synced to ButtonsController
    // This shouldnt happen, but to prevent segfaults
    if(!this->data){
        return;
    }

    this->data->setVisibleEmission(active);
    emit this->requestUpdate();
}

/*
Slot: receives the click from the excitation button. Forwards the change to the relevant cache/spectrum object
*/
void ButtonsController::receiveExcitationClick(bool active){
    // Just constructed, not synced to ButtonsController
    // This shouldnt happen, but to prevent segfaults
    if(!this->data){
        return;
    }

    this->data->setVisibleExcitation(active);
    emit this->requestUpdate();
}   


/*
Slot: receives the click from the remove button. Emits removal request
*/
void ButtonsController::receiveRemoveClick(){
    // Unknown spectrum id's wont cause errors in the cache, so no need to check
    // Build ID, the counter is irrelevant for removal requests
    Data::FluorophoreID id = {this->id, this->name, 0};
    std::vector<Data::FluorophoreID> fluorophores;
    fluorophores.push_back(id);
    
    emit this->sendRemove(fluorophores);
}

/*
Slot: receives the button hover entered signal. Updates the CacheSpectrum select attribute and requests update
*/
void ButtonsController::hoverEntered(){
    this->data->setSelectEmission(true);
    this->data->setSelectExcitation(true);

    emit this->requestUpdate();
}

/*
Slot: receives the button hover leaved signal. Updates the CacheSpectrum select attribute and requests update
*/
void ButtonsController::hoverLeaved(){
    this->data->setSelectEmission(false);
    this->data->setSelectExcitation(false);

    emit this->requestUpdate();
}

/*
Slot: receives selected event. Means that the internal widget has been selected, needs to be forwarded to scrollarea for scroll update
*/
void ButtonsController::receiveSelected(){
    emit this->sendSelected(this);
}


} // Fluor namespace
