/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_controller.h"

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
    QObject::connect(this, &Fluor::Controller::sendSync, widget_scrollarea, &Fluor::ScrollController::syncButtons);
    QObject::connect(this, &Fluor::Controller::sendSync, widget_lineedit, &Fluor::LineEdit::sync);
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
Slot: unfocus the widget
*/
void Controller::receiveGlobalEvent(QEvent* event){
    emit this->sendGlobalEvent(event);
}

/*
Slot: reload the data of the widget
*/
void Controller::receiveData(const Data::Fluorophores& data){
    emit this->sendData(data);
}

/*
Slot: reload the max size of a (ListView) widget
*/
void Controller::receiveGlobalSize(const QWidget* widget){
    emit this->sendGlobalSize(widget);
}

/*
Slot: receives and sends output from the internal lineedit and forwards towards cache
*/
void Controller::receiveCacheAdd(std::set<Data::FluorophoreID>& output){
    emit this->sendCacheAdd(output);
}

/*
Slot: receives and sends output from the scroll/button controllers and forwards towards cache
*/
void Controller::receiveCacheRemove(std::set<Data::FluorophoreID>& output){
    emit this->sendCacheRemove(output);
}

/*
Slot: receives and sends synchronisation input to the scrollcontroller
*/
void Controller::receiveSync(const std::vector<Cache::CacheID>& input){
    emit this->sendSync(input);
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
    button_widgets()
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

    // Need to re-polish the scrollbar for proper stylesheet handling, not sure why
    this->verticalScrollBar()->style()->unpolish(this->verticalScrollBar());
    this->verticalScrollBar()->style()->polish(this->verticalScrollBar());

    // Reserve vector space
    this->button_widgets.reserve(20);
}

/*
Synchronizes the internal ButtonsController widgets to the input. Adds/removes the necessary
ButtonsController and resets the name, id, cache pointers.
    :param inputs: the to-be-synced ids
*/
void ScrollController::syncButtons(const std::vector<Cache::CacheID>& inputs){
    // qDebug() << "ScrollController::syncButtons:" << this->button_widgets.size() << ":" << inputs.size();
    
    // First add/remove to requested buttonControllers count
    if(this->button_widgets.size() < inputs.size()){
        for(std::size_t i=this->button_widgets.size(); i<inputs.size(); ++i){
            // Build new widget
            ButtonsController* widget = new ButtonsController(this->widget());
            static_cast<QVBoxLayout*>(this->widget()->layout())->insertWidget(static_cast<int>(i), widget);

            QObject::connect(widget, &Fluor::ButtonsController::sendRemove, this, &Fluor::ScrollController::receiveRemove);

            // Add to lookup vector
            this->button_widgets.push_back(widget);
        }
    }else if(this->button_widgets.size() > inputs.size()){
        for(std::size_t i=this->button_widgets.size(); i>inputs.size(); --i){
            // Get last widget and remove it
            ButtonsController* widget = this->button_widgets[i-1];

            QObject::disconnect(widget, &Fluor::ButtonsController::sendRemove, this, &Fluor::ScrollController::receiveRemove);

            delete widget;

            this->button_widgets.pop_back();
        }
    }

    // Now sync the ButtonsControllers
    for(std::size_t i=0; i<this->button_widgets.size(); ++i){
        this->button_widgets[i]->syncButtons(inputs[i]);
    }
}

/*
Slot: receives and forwards remove fluorophore request
*/
void ScrollController::receiveRemove(std::set<Data::FluorophoreID>& fluorophores){
    emit this->sendRemove(fluorophores);
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
Sync the buttonsController and internal widgets with a CacheID
    :param input: cacheid (assumed to have a valid data pointer)
*/
void ButtonsController::syncButtons(const Cache::CacheID& input){
    this->id = input.id;
    this->name = input.name;
    this->data = input.data;

    this->widget_emission->setText(this->name);
}

/*
Slot: receives the click from the emission button. Forwards the change to the relevant cache/spectrum object
*/
void ButtonsController::receiveEmissionClick(bool active){
    // Just constructor, not syncID'd ButtonsController has a nullptr as data
    // This shouldnt happen, but to prevent segfaults
    if(!this->data){
        return;
    }

    this->data->setVisibleEmission(!active);
}

/*
Slot: receives the click from the excitation button. Forwards the change to the relevant cache/spectrum object
*/
void ButtonsController::receiveExcitationClick(bool active){
    // Just constructor, not syncID'd ButtonsController has a nullptr as data
    // This shouldnt happen, but to prevent segfaults
    if(!this->data){
        return;
    }

    this->data->setVisibleExcitation(!active);
}


/*
Slot: receives the click from the remove button. Emits removal request
*/
void ButtonsController::receiveRemoveClick(){
    // Unknown spectrum id's wont cause errors in the cache, so no need to check

    // Build ID, the counter is irrelevant for removal requests
    Data::FluorophoreID id = {this->id, this->name, 0};
    std::set<Data::FluorophoreID> fluorophores;
    fluorophores.insert(id);
    
    emit this->sendRemove(fluorophores);
}


} // Fluor namespace
