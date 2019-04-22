/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "laser_lineedit.h"
#include "fluor_lineedit.h"

#include <QStyle>
#include <QChar>

#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QAbstractItemModel>
#include <QStandardItemModel>

#include <QMouseEvent>
#include <QKeyEvent>

#include <QDebug>

#include <QApplication>
#include <QClipboard>
#include <QDesktopWidget>
#include <QScreen>
#include <QScrollBar>

namespace Laser {

/*
Constructor: Builds the popup for the Laser::LineEdit
    :param parent: parent widget
*/
Popup::Popup(QWidget* widget) :
    QListView(widget),
    max_visible_items(50),
    max_size()
{
    // Set standard settings:
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setWindowModality(Qt::NonModal);
    this->setWindowFlag(Qt::Tool);
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::NoDropShadowWindowHint);
    this->setWindowFlag(Qt::WindowDoesNotAcceptFocus);
    
    // Set delegate for proper stylesheet usage.
    QStyledItemDelegate* delegate_popup = new QStyledItemDelegate{this};
    this->setItemDelegate(delegate_popup);

    // Set sample data
    std::vector<int> data_example = {350, 405, 562, 640, 900};
    this->buildModel(data_example);

    // Set eventfilter on the viewport to capture mouvemove events
    this->viewport()->installEventFilter(this);
}

/*
Updates the popup as if a Key_Up is pressed (popup will never receive these events so is propagated from parent)
    :returns: whether the event is (fully) handled
*/
bool Popup::updateKeyUp(){
    QModelIndex index_current = this->currentIndex();
    if(!index_current.isValid()){
        int row_count = this->model()->rowCount();
        QModelIndex index_last = this->model()->index(row_count -1, 0);

        this->setCurrentIndex(index_last);
    }else if(index_current.row() == 0){
        this->setCurrentIndex(QModelIndex());
    }else{
        QModelIndex index_up = this->model()->index(index_current.row() -1, 0);
        this->setCurrentIndex(index_up);
    }
    // Emit highlighted signal
    emit this->highlighted(this->currentIndex());
    return true;
}

/*
Updates the popup as if a Key_Down is pressed (popup will never receive these events so is propagated from parent)
    :returns: whether the event is (fully) handled
*/
bool Popup::updateKeyDown(){
    QModelIndex index_current = this->currentIndex();
    if(!index_current.isValid()){
        QModelIndex index_first = this->model()->index(0, 0);
        this->setCurrentIndex(index_first);
    }else if(index_current.row() == this->model()->rowCount() -1){
        this->setCurrentIndex(QModelIndex());
    }else{
        QModelIndex index_down = this->model()->index(index_current.row() +1, 0);
        this->setCurrentIndex(index_down);
    }

    // Emit signal for feedback to user
    emit this->highlighted(this->currentIndex());
    return true;
}

/*
Builds and sets an 'empty' QListview model
*/
void Popup::buildModel(){
    // Get old model, and build new model
    QAbstractItemModel* model_old = this->model();
    QStandardItemModel* model_new = new QStandardItemModel(this);

    // Set 'empty' model
    QStandardItem* item_remove = new QStandardItem("Remove Laser");
    item_remove->setCheckable(false);
    item_remove->setData(-1);
    model_new->appendRow(item_remove);

    this->setModel(model_new);

    // Delete old model
    delete model_old;
}

/*
Builds and sets the QListView model
    :param wavelengths: list of laser wavelengths
*/
void Popup::buildModel(const std::vector<int>& wavelengths){
    // Get original model as the original has to be deleted manually
    QAbstractItemModel* model_old = this->model();
    QStandardItemModel* model_new = new QStandardItemModel(this);

    QStandardItem* item_remove = new QStandardItem("Remove Laser");
    item_remove->setCheckable(false);
    item_remove->setData(-1);
    model_new->appendRow(item_remove);

    for(int wavelength : wavelengths){
        QString item_text = "%1nm - test";
        item_text = item_text.arg(wavelength);
        QStandardItem* item_widget = new QStandardItem(item_text);
        item_widget->setCheckable(false);
        item_widget->setData(wavelength, Qt::UserRole +1);
        model_new->appendRow(item_widget);
    }

    this->setModel(model_new);
    delete model_old;
}

/*
Modified QCompleter::showPopup() to correct the popup size to not be bigger then a specified QWidget
*/
void Popup::showPopup(){
    QWidget* parent_widget = static_cast<QWidget*>(this->parent());

    // Set height of the QListView
    // +14 depends on padding stylesheet parameter
    // Calculate height-necessary-to-fit-columns
    int height = this->sizeHintForRow(0) * std::min(this->max_visible_items, this->model()->rowCount()) + 14;

    QScrollBar* horizontal_scrollbar = this->horizontalScrollBar();
    if(horizontal_scrollbar){
        if(horizontal_scrollbar->isVisible()){
            height += horizontal_scrollbar->sizeHint().height();
        }
    }

    // Get parent widget height and width
    int parent_height = parent_widget->height();
    QPoint pos = parent_widget->mapToGlobal(QPoint(0, parent_height - 1));
    int width = parent_widget->width();

    // Adjust x and width to fit in popup_max_size QRect
    if(width > this->max_size.width()){
        width = this->max_size.width();
    }
    if( (pos.x() + width) > (this->max_size.x() + this->max_size.width()) ){
        pos.setX(this->max_size.x() + this->max_size.width() - width);
    }
    if(pos.x() < this->max_size.x()){
        pos.setX(this->max_size.x());
    }

    // Adjust y and height to fit most efficient in popup_max_size QRect
    int top = pos.y() - parent_height - this->max_size.top() + 2;
    int bottom = this->max_size.y() + this->max_size.height() - pos.y();
    height = std::max(height, this->minimumHeight());
    if(height > bottom){
        height = std::min(std::max(top, bottom), height);
        if(top > bottom){
            pos.setY(pos.y() - height - parent_height + 2);
        }
    }

    this->setGeometry(pos.x(), pos.y(), width, height);

    if(!this->isVisible()){
        this->show();
    }
}

/*
Eventfilter for the viewport. Intercepts mousemove events to correctly fire highlight events
    :param obj: object that fires the event
    :param event: event
    :returns: whether the event is (fully) handled
*/
bool Popup::eventFilter(QObject* obj, QEvent* event){
    switch(event->type()){
    case QEvent::MouseMove:{
        // Fires when mouse button is clicked and held
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);

        // Check if left mouse button is pressed
        if( (event_mouse->buttons()&Qt::LeftButton) == Qt::LeftButton){
            QPoint point_local = this->mapFromGlobal(event_mouse->globalPos());
            
            // I get coordinate from widget, but have to correct to viewport by removing the marings
            point_local.setY(point_local.y() - this->contentsMargins().top());
            point_local.setX(point_local.x() - this->contentsMargins().left());

            QModelIndex index_current = this->indexAt(point_local);
            
            emit this->highlighted(index_current);
        }
        break;
    }
    case QEvent::MouseButtonDblClick:{
        // Fires when mouse button is clicked and held
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);

        // Check if left mouse button is pressed
        if( (event_mouse->buttons()&Qt::LeftButton) == Qt::LeftButton){
            QPoint point_local = this->mapFromGlobal(event_mouse->globalPos());
            
            // I get coordinate from widget, but have to correct to viewport by removing the marings
            point_local.setY(point_local.y() - this->contentsMargins().top());
            point_local.setX(point_local.x() - this->contentsMargins().left());

            QModelIndex index_current = this->indexAt(point_local);

            emit this->dblClicked(index_current);
        }
        break;
    }
    default:
        break;    
    }
    return QListView::eventFilter(obj, event);
}

/*
Slot: Receives the activated QModelIndex, receives and emits the proper output
    :param index: the activated index
*/
void Popup::buildOutput(const QModelIndex& index){
    qDebug() << "Laser::Popup:buildOutput";
    if(index.isValid()){
        int index_data = index.data(Qt::UserRole +1).toInt();
        emit this->output(index_data);
    }
}

/*
Slot: Updates the popup max size parameter
    :param widget: (optional) the widget to base the popup size upon, or if not specified bases it upon the screen size
*/
void Popup::updateRect(const QWidget* widget){
    QWidget* parent_widget = static_cast<QWidget*>(this->parent());

    QRect size;
    if(widget){
        size = widget->geometry();
        size.moveTopLeft(widget->mapToGlobal(QPoint(0,0)));

        QMargins size_margins = widget->contentsMargins();
        size = size.marginsRemoved(size_margins);

    }else{
        QApplication* instance = static_cast<QApplication*>(QApplication::instance());
        int screen_id = instance->desktop()->screenNumber(parent_widget);
        size = instance->screens()[screen_id]->geometry();
    }

    this->max_size = size;
}

/*
Slot: Reloads and repopulates the model
    :param cytometer: the cytometer data to load into the model

void Popup::reloadModel(const DataCytometer* cytometer){
    
}
*/

/*
Constructor: Constructs a Fluorophore LineEdit object.
    :param parent: pointer to parent widget
*/
LineEdit::LineEdit(QWidget* parent) : 
    QLineEdit(parent),
    widget_popup(nullptr),
    text_before("Wavelength: "),
    text_after("nm"),
    text_write_start(this->text_before.length()),
    text_write_length(3),
    text_write_end(this->text_write_start + this->text_write_length)
{
    // Setup QLineEdit
    this->setProperty("popup", false);  // needs a property for proper stylesheet handling
    this->setContextMenuPolicy(Qt::NoContextMenu);
    this->setAlignment(Qt::AlignCenter);
    this->buildText(-1);
    this->setCursorPosition(this->text_write_end);

    // Builds and sets widget
    Laser::Popup* popup_widget = new Laser::Popup(this);
    this->setPopup(popup_widget);

    this->installEventFilter(this);

}

/*
Sets popup. Takes ownership of the popup (dont share popup with other widgets)
    :param popup: popup widget
*/
void LineEdit::setPopup(Laser::Popup* popup){
    // Remove previous popup
    if(this->widget_popup){
        QObject::disconnect(this->popup(), &Laser::Popup::pressed, this, &Laser::LineEdit::updatePopupActivated);
        QObject::disconnect(this->popup(), &Laser::Popup::highlighted, this, &Laser::LineEdit::updatePopupActivated);
        QObject::disconnect(this->popup(), &Laser::Popup::dblClicked, this, &Laser::LineEdit::updatePopupDblClicked);
        //QObject::disconnect(this, &LineEdit::highlightPopup, static_cast<Fluor::Completer*>(this->completer()), &Fluor::Completer::updateHighlight);
        delete this->widget_popup;
    }
    
    this->widget_popup = popup;
    QObject::connect(this->popup(), &Laser::Popup::pressed, this, &Laser::LineEdit::updatePopupActivated);
    QObject::connect(this->popup(), &Laser::Popup::highlighted, this, &Laser::LineEdit::updatePopupActivated);
    QObject::connect(this->popup(), &Laser::Popup::dblClicked, this, &Laser::LineEdit::updatePopupDblClicked);
    //QObject::connect(this, &LineEdit::highlightPopup, static_cast<Fluor::Completer*>(this->completer()), &Fluor::Completer::updateHighlight);
}

/*
Gets popup for LineEdit.
    :returns: popup widget
*/
Laser::Popup* LineEdit::popup(){
    if(this->widget_popup){
        return this->widget_popup;
    }

    // No popup has been set, so build one:
    Laser::Popup* popup_new = new Laser::Popup(this);
    this->setPopup(popup_new);  

    return this->widget_popup;
}

/*
Shows the popup (if not visible)
*/
void LineEdit::showPopup(){
    if(!this->popup()->isVisible()){
        this->popup()->showPopup();
    }
}

/*
Hides the popup (if visible)
*/
void LineEdit::hidePopup(){
    if(this->popup()->isVisible()){
        this->popup()->setCurrentIndex(QModelIndex());
        this->popup()->hide();
    }
}

/*
Removes focus from the widget and its popup. Builds and emits output events.
*/
void LineEdit::clearFocus(){
    this->buildOutput();
    this->toggleStylePopup(false);
    this->hidePopup();

    QLineEdit::clearFocus();
}

/*
Construct output integer and emits->output() 
*/
void LineEdit::buildOutput(){
    // Check if popup has remove selected as that one is exempt from text modification
    QModelIndex popup_index_current = this->popup()->currentIndex();
    int output = 0;
    if(popup_index_current.data(Qt::UserRole + 1).toInt() == -1){
        output = -1;
    }else{
        QString text = this->text();
        
        // Validate output
        QString output_text("");
        for(int i = this->text_write_start; i < this->text_write_end; ++i){
            QChar letter(text[i]);

            // Only accept numbers
            if(letter.isNumber()){
                output_text.append(letter);
            }
        }

        output = output_text.toInt();
    }

    emit this->output(output);
    qDebug() << "Laser::LineEdit: emits output: " << output;
    emit this->finished();
}

/*
Toggles the popup style property and forces style repolishing
*/
void LineEdit::toggleStylePopup() {
    this->setProperty("popup", !this->property("popup").toBool());

    // Repolish button stylesheet
    this->style()->unpolish(this);
    this->style()->polish(this);
}

/*
Toggles the popup style property to the defined state and forces repolishing
    :param popup: whether or not the popup is visible
*/
void LineEdit::toggleStylePopup(bool popup) {
    if(this->property("popup") != popup){
        this->setProperty("popup", popup);

        // Repolish button stylesheet
        this->style()->unpolish(this);
        this->style()->polish(this);
    }
}

/*
eventFilter for FocusIn, FocusOut events and Keypress events to properly update the text
    :param obj: source object
    :param event: the event
*/
bool LineEdit::eventFilter(QObject *obj, QEvent *event){
    switch(event->type()){
    case QEvent::FocusIn:
        this->toggleStylePopup(true);
        return false;
    case QEvent::FocusOut:
        return true;
    
    case QEvent::KeyPress: {
        // Make sure the popup is shown
        this->showPopup();

        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        switch(key_event->key()){
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:{
            if(this->selectionStart() == -1){
                int cursor_pos = this->cursorPosition();
                // Make sure the cursor is in a valid location
                if(cursor_pos >= this->text_write_start && cursor_pos < this->text_write_end){
                    int cursor_loc = cursor_pos - this->text_write_start;
                    QString text_current = this->text().mid(this->text_write_start, this->text_write_length);

                    // Determine whether to replace or insert the text
                    QString output("");
                    if(text_current[this->text_write_length -1].isNumber()){
                        output = text_current.replace(cursor_loc, 1, key_event->text());
                    }else{
                        output = text_current.insert(cursor_loc, key_event->text());
                    }

                    this->buildText(std::move(output));
                    this->setCursorPosition(cursor_pos +1);
                    this->popup()->setCurrentIndex(QModelIndex());
                }
            }else{
                int select_start = this->selectionStart();
                int select_end = this->selectionEnd();
                QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                
                // correct to local coordinates
                select_start -= this->text_write_start;
                select_end -= this->text_write_start;

                // limit to modifyable region
                if(select_start < 0){
                    select_start = 0;
                }else if(select_start > this->text_write_length){
                    select_start = this->text_write_length;
                }
                if(select_end > this->text_write_length){
                    select_end = this->text_write_length;
                }else if(select_end < 0){
                    select_end = 0;
                }
                int select_length = select_end - select_start;

                // Remove selection
                QString output("");
                output = text_current.replace(select_start, select_length, key_event->text());

                // Output results
                this->buildText(std::move(output));
                this->setCursorPosition(select_start + this->text_write_start + 1);
                this->popup()->setCurrentIndex(QModelIndex());
            }

            return true;
        }
        case Qt::Key_Backspace:{
            if(this->selectionStart() == -1){
                int cursor_pos = this->cursorPosition();

                // Make sure the cursor is in a valid location
                if(cursor_pos > this->text_write_start && cursor_pos <= this->text_write_end){
                    QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                    int cursor_loc = cursor_pos - this->text_write_start;

                    QString output("");
                    output = text_current.remove(cursor_loc-1, 1);

                    this->buildText(std::move(output));
                    this->setCursorPosition(cursor_pos -1);
                    this->popup()->setCurrentIndex(QModelIndex());
                }
            }else{
                int select_start = this->selectionStart();
                int select_end = this->selectionEnd();
                QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                
                // correct to local coordinates
                select_start -= this->text_write_start;
                select_end -= this->text_write_start;

                // limit to modifyable region
                if(select_start < 0){
                    select_start = 0;
                }else if(select_start > this->text_write_length){
                    select_start = this->text_write_length;
                }
                if(select_end > this->text_write_length){
                    select_end = this->text_write_length;
                }else if(select_end < 0){
                    select_end = 0;
                }
                int select_length = select_end - select_start;

                // Remove selection
                QString output("");
                output = text_current.remove(select_start, select_length);

                // Output results
                this->buildText(std::move(output));
                this->setCursorPosition(select_start + this->text_write_start);
                this->popup()->setCurrentIndex(QModelIndex());
            }

            return true;
        }
        case Qt::Key_Delete:{
            if(this->selectionStart() == -1){
                int cursor_pos = this->cursorPosition();

                // Make sure the cursor is in a valid location
                if(cursor_pos >= this->text_write_start && cursor_pos < this->text_write_end){
                    // Check if area to right can be deleted -> doesnt really matter cause buildText will correct anyway
                    bool valid_to_right = this->text()[cursor_pos].isNumber();
                    if(!valid_to_right){
                        return true;
                    }
                    QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                    int cursor_loc = cursor_pos - this->text_write_start;

                    QString output("");
                    output = text_current.remove(cursor_loc, 1);

                    this->buildText(std::move(output));
                    this->setCursorPosition(cursor_pos);
                    this->popup()->setCurrentIndex(QModelIndex());
                }
            }else{
                int select_start = this->selectionStart();
                int select_end = this->selectionEnd();
                QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                
                // correct to local coordinates
                select_start -= this->text_write_start;
                select_end -= this->text_write_start;

                // limit to modifyable region
                if(select_start < 0){
                    select_start = 0;
                }else if(select_start > this->text_write_length){
                    select_start = this->text_write_length;
                }
                if(select_end > this->text_write_length){
                    select_end = this->text_write_length;
                }else if(select_end < 0){
                    select_end = 0;
                }
                int select_length = select_end - select_start;

                // Remove selection
                QString output("");
                output = text_current.remove(select_start, select_length);

                // Output results
                this->buildText(std::move(output));
                this->setCursorPosition(select_start + this->text_write_start);
                this->popup()->setCurrentIndex(QModelIndex());
            }
            return true;
        }
        case Qt::Key_Left:{
            int cursor_pos = this->cursorPosition();
            
            // Make sure the cursor is in a valid location
            if(cursor_pos > this->text_write_start){
                if(key_event->modifiers() == Qt::ShiftModifier){
                    if(this->selectionStart() == -1){
                        this->setSelection(cursor_pos, -1);
                    }else if(this->selectionStart() == cursor_pos -1){
                        this->setCursorPosition(cursor_pos -1);
                    }else if(this->selectionStart() < cursor_pos -1){
                        this->setSelection(this->selectionStart(), this->selectionLength() -1);
                    }else{
                        this->setSelection(this->selectionEnd(), -this->selectionLength() -1);
                    }
                }else{
                    if(this->selectionStart() == -1){
                        this->setCursorPosition(cursor_pos -1);
                    }else{
                        this->setCursorPosition(this->selectionStart());
                    }
                }
            }else if(cursor_pos == this->text_write_start){
                if(this->selectionStart() != -1 && key_event->modifiers() != Qt::ShiftModifier){
                    this->setCursorPosition(this->text_write_start);
                }
            }
            return true;
        }
        case Qt::Key_Right:{
            int cursor_pos = this->cursorPosition();

            // Make sure the cursor is in a valid location
            if(cursor_pos < this->text_write_end){
                // Check if valid to-the-right symbol
                // Safe to use [] as above we check the location in string
                bool valid_to_right = this->text()[cursor_pos].isNumber();
                if(!valid_to_right){
                    if(this->selectionStart() != -1 && key_event->modifiers() != Qt::ShiftModifier){
                        this->setCursorPosition(cursor_pos);
                    }
                    return true;
                }

                if(key_event->modifiers() == Qt::ShiftModifier){
                    if(this->selectionStart() == -1){
                        this->setSelection(cursor_pos, 1);
                    }else if(this->selectionEnd() == cursor_pos +1){
                        this->setCursorPosition(cursor_pos + 1);
                    }else if(this->selectionEnd() > cursor_pos + 1){
                        this->setSelection(this->selectionEnd(), -this->selectionLength() +1);
                    }else{
                        this->setSelection(this->selectionStart(), this->selectionLength() +1);
                    }
                }else{
                    if(this->selectionStart() == -1){
                        this->setCursorPosition(cursor_pos +1);
                    }else{
                        this->setCursorPosition(this->selectionEnd());
                    }
                }
            }else if(cursor_pos == this->text_write_end){
                if(this->selectionStart() != -1 && key_event->modifiers() != Qt::ShiftModifier){
                    this->setCursorPosition(this->text_write_end);
                }
            }
            return true;
        }
        case Qt::Key_Home:
            if(key_event->modifiers() == Qt::ShiftModifier){
                if(this->selectionStart() != -1){
                    if(this->cursorPosition() == this->selectionStart()){
                        this->setSelection(this->selectionEnd(), this->text_write_start - this->selectionEnd());
                    }else{
                        this->setSelection(this->selectionStart(), this->text_write_start - this->selectionStart());
                    }
                }else{
                    this->setSelection(this->cursorPosition(), this->text_write_start - this->cursorPosition());
                }
            }else{
                this->setCursorPosition(this->text_write_start);
            }
            return true;
        case Qt::Key_End:{
            // First find last 'correct' entree
            QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
        
            int last_valid_entree = this->text_write_length;
            for(int i = 0; i < this->text_write_length; ++i){
                QChar letter(text_current[i]);
                // Only accept numbers
                if(!letter.isNumber()){
                    last_valid_entree = i;
                    break;
                }
            }

            last_valid_entree += this->text_write_start;
            if(key_event->modifiers() == Qt::ShiftModifier){
                if(this->selectionStart() != -1){
                    if(this->cursorPosition() == this->selectionStart()){
                        this->setSelection(this->selectionEnd(), last_valid_entree  - this->selectionEnd());
                    }else{
                        this->setSelection(this->selectionStart(), last_valid_entree  - this->selectionStart());
                    }
                }else{
                    this->setSelection(this->cursorPosition(), last_valid_entree - this->cursorPosition());
                }
            }else{
                this->setCursorPosition(last_valid_entree);
            }
            return true;
        }
        case Qt::Key_Enter:
        case Qt::Key_Return:
            this->buildOutput();
            return true;
        case Qt::Key_Escape:
            this->buildText(-1);
            this->setCursorPosition(this->text_write_start);
            this->buildOutput();
            return true;
        
        // key to handle for popup
        case Qt::Key_PageUp:
        case Qt::Key_Up:{
            return this->popup()->updateKeyUp();
        }
        case Qt::Key_PageDown:
        case Qt::Key_Down:{
            return this->popup()->updateKeyDown();
        }

        // Shortcuts
        case Qt::Key_A:{
            if(key_event->modifiers() == Qt::ControlModifier){
                // First find last 'correct' entree
                QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
            
                int last_valid_entree = this->text_write_length;
                for(int i = 0; i < this->text_write_length; ++i){
                    QChar letter(text_current[i]);
                    // Only accept numbers
                    if(!letter.isNumber()){
                        last_valid_entree = i;
                        break;
                    }
                }
                this->setSelection(this->text_write_start, last_valid_entree);
            }
            return true;
        }
        case Qt::Key_C:{
            if(key_event->modifiers() == Qt::ControlModifier){
                if(this->selectionStart() != -1){
                    // I do not check for selection bounds, cause if the user manages to copy more then the write area
                    // they wouldnt expect copy to return only a part of the selected area
                    QString output("");
                    output = this->text().mid(this->selectionStart(), this->selectionLength());
                    qDebug() << "Laser::LineEdit::eventFilter: copy to clipboard (" << output << ")";
                    QApplication::clipboard()->setText(output);
                }
            }
            return true;
        }
        case Qt::Key_X:{
            if(key_event->modifiers() == Qt::ControlModifier){
                if(this->selectionStart() != -1){
                    // I do not check for selection bounds, cause if the user manages to cut more then the write area
                    // they wouldnt expect copy to return only a part of the selected area
                    QString output_cut("");
                    output_cut = this->text().mid(this->selectionStart(), this->selectionLength());
                    qDebug() << "Laser::LineEdit::eventFilter: cut to clipboard (" << output_cut << ")";
                    QApplication::clipboard()->setText(output_cut);

                    // Now i do care about the editable area
                    int select_start = this->selectionStart();
                    int select_end = this->selectionEnd();
                    QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                    
                    // correct to local coordinates
                    select_start -= this->text_write_start;
                    select_end -= this->text_write_start;

                    // limit to modifyable region
                    if(select_start < 0){
                        select_start = 0;
                    }else if(select_start > this->text_write_length){
                        select_start = this->text_write_length;
                    }
                    if(select_end > this->text_write_length){
                        select_end = this->text_write_length;
                    }else if(select_end < 0){
                        select_end = 0;
                    }
                    int select_length = select_end - select_start;

                    // Remove selection
                    QString output("");
                    output = text_current.remove(select_start, select_length);

                    // Output results
                    this->buildText(std::move(output));
                    this->setCursorPosition(select_start + this->text_write_start);
                }
            }
            return true;
        }
        case Qt::Key_V:{
            if(key_event->modifiers() == Qt::ControlModifier){
                // First remove all non numbers from the to-be-copied string
                // To prevent waste of time, will break the moment this->text_write_length has been reached in valid characters
                QString text_paste("");
                text_paste = QApplication::clipboard()->text();

                if(text_paste.isEmpty() || text_paste.length() == 0){
                    return true;
                }

                QString text_numbers("");
                for(int i=0; i<text_paste.length(); ++i){
                    QChar letter(text_paste[i]);

                    // Only accept numbers
                    if(letter.isNumber()){
                        text_numbers += letter;
                        if(text_numbers.length() >= this->text_write_length){
                            break;
                        }
                    }
                }
                
                QString text_current = this->text().mid(this->text_write_start, this->text_write_length);
                QString output("");
                
                // Check for selection and if so remove selected area
                if(this->selectionStart() != -1){
                    int select_start = this->selectionStart();
                    int select_end = this->selectionEnd();
                    
                    // correct to local coordinates
                    select_start -= this->text_write_start;
                    select_end -= this->text_write_start;

                    // limit to modifyable region
                    if(select_start < 0){
                        select_start = 0;
                    }else if(select_start > this->text_write_length){
                        select_start = this->text_write_length;
                    }
                    if(select_end > this->text_write_length){
                        select_end = this->text_write_length;
                    }else if(select_end < 0){
                        select_end = 0;
                    }
                    int select_length = select_end - select_start;

                    // Remove selection
                    output = text_current.replace(select_start, select_length, text_numbers);
                    this->buildText(output);

                    int cursor_output = std::min(select_start + text_numbers.length(), this->text_write_length);
                    this->setCursorPosition(cursor_output + this->text_write_start);
                    this->popup()->setCurrentIndex(QModelIndex());
                }else{
                    int cursor_pos = this->cursorPosition();
                    // check if cursor_pos is valid, if not limit ot modifyable area
                    if(cursor_pos < this->text_write_start){
                        cursor_pos = 0;
                    }else if(cursor_pos > this->text_write_end){
                        cursor_pos = this->text_write_length;
                    }else{
                        cursor_pos -= this->text_write_start;
                    }
                    output = text_current.insert(cursor_pos, text_numbers);
                    this->buildText(output);
                    
                    int cursor_output = std::min(cursor_pos + text_numbers.length(), this->text_write_length);
                    this->setCursorPosition(cursor_output + this->text_write_start);
                    this->popup()->setCurrentIndex(QModelIndex());
                }
            }
            return true;
        }
        default:
            return true;
        }

    }
    case QEvent::MouseButtonPress: {
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        // Dont think this is relevant, but as safety to always be able to restart the popup
        this->showPopup();

        // Get rightmost valid location   
        int last_valid_entree = this->text_write_end;
        for(int i = this->text_write_start; i < this->text_write_end; ++i){
            QChar letter = this->text()[i];
            // Only accept numbers
            if(!letter.isNumber()){
                last_valid_entree = i;
                break;
            }
        }

        // Get cursor location
        int mouse_pos = this->cursorPositionAt(QPoint(event_mouse->x(), event_mouse->y()));
        mouse_pos = std::max(this->text_write_start, mouse_pos);
        mouse_pos = std::min(mouse_pos, last_valid_entree);

        // What if shift is pressed?
        if(event_mouse->modifiers() == Qt::ShiftModifier){
            // Is there already a selection?
            if(this->selectionStart() == -1){
                this->setSelection(this->cursorPosition(), mouse_pos - this->cursorPosition());
            }else if(this->selectionStart() == this->cursorPosition()){
                this->setSelection(this->selectionEnd(), mouse_pos - this->selectionEnd());
            }else{
                this->setSelection(this->selectionStart(), mouse_pos - this->selectionStart());
            }

        }else{
            this->setCursorPosition(mouse_pos);
        }
        return true;
    }
    case QEvent::MouseMove: {
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);

        if(event_mouse->buttons() == Qt::LeftButton){
            // Get rightmost valid location   
            int last_valid_entree = this->text_write_end;
            for(int i = this->text_write_start; i < this->text_write_end; ++i){
                QChar letter = this->text()[i];
                // Only accept numbers
                if(!letter.isNumber()){
                    last_valid_entree = i;
                    break;
                }
            }

            // Get cursor location
            int cursor_pos = this->cursorPosition();
            int mouse_pos = this->cursorPositionAt(QPoint(event_mouse->x(), event_mouse->y()));
            mouse_pos = std::max(this->text_write_start, mouse_pos);
            mouse_pos = std::min(mouse_pos, last_valid_entree);

            // Set selection
            if(this->selectionStart() == -1){
                this->setSelection(cursor_pos, mouse_pos - cursor_pos);
            }else{
                if(this->selectionEnd() == cursor_pos){
                    this->setSelection(this->selectionStart(), mouse_pos - this->selectionStart());
                }else if(this->selectionStart() == cursor_pos){
                    this->setSelection(this->selectionEnd(), mouse_pos - this->selectionEnd());
                }else{
                    this->setCursorPosition(mouse_pos);
                }
            }
        }
        return true;
    }
    case QEvent::MouseButtonDblClick: {
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        if(event_mouse->buttons() == Qt::LeftButton){
            // Get rightmost valid location   
            int last_valid_entree = this->text_write_end;
            for(int i = this->text_write_start; i < this->text_write_end; ++i){
                QChar letter = this->text()[i];
                // Only accept numbers
                if(!letter.isNumber()){
                    last_valid_entree = i;
                    break;
                }
            }

            this->setSelection(this->text_write_start, last_valid_entree - this->text_write_start);
        }
        return true;
    }
    case QEvent::MouseButtonRelease: {
        return true;
    }
    default:
        break;
    }

    return QLineEdit::eventFilter(obj, event);
}

/*
Slot: builds the text of the widget by combining the text_before, text_after, and value
    :param value: string to add to text, empty string will cause the text to reset
*/
void LineEdit::buildText(QString value){
    QString text = this->text_before;
    text += value.leftJustified(this->text_write_length, '_', true);
    text += this->text_after;

    this->setText(std::move(text));
}

/*
Slot: builds the text of the widget by combining the text_before, text_after, and value
    :param value: value to add to text, negative value will cause the text to reset
*/
void LineEdit::buildText(int value){
    // Build begin of text
    QString text = this->text_before;
    QString text_value = QString().setNum(value);

    // Build variable part
    if(value < 0){
        text += QString(this->text_write_length, '_');
    }else{
        text += QString(text_value).leftJustified(this->text_write_length, '_', true);
    }

    // Build end of text
    text += this->text_after;

    this->setText(std::move(text));
}

/*
Slot: shows the button and sets-up the starting state
*/
void LineEdit::showButton(){
    // Set the focus
    this->setFocus();

    // Show the button
    this->show();
    this->setCursorPosition(this->text_write_start);
    
    // Build first completion
    this->showPopup();
}

/*
Slot: hides and resets button
*/
void LineEdit::hideButton(){
    this->reset();
    this->hidePopup();
    this->hide();
}

/*
Slot: unfocuses the widget, based upon a global QEvent::MouseButtonRelease. Different QEvents are ignored.
    :param event: the global QEvent::MouseButtonRelease
*/
void LineEdit::unfocus(QEvent* event){
    // Cast dynamically, because event is of undefined super class
    QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);

    // No mouse event
    if(!mouse_event){
        return;
    }
    
    // Make sure it is the correct mouse event
    if(mouse_event->type() != QEvent::MouseButtonRelease){
        return;
    }

    // Check if it is a click on this widget -> if so, ignore
    if(this->rect().contains(this->mapFromGlobal(mouse_event->globalPos()))){
        return;
    }

    // Clicks on the popup do not propagate to the Main::Controller
    // But clicks moved into the popup do propagate, so check
    if(this->popup()->rect().contains(this->mapFromGlobal(mouse_event->globalPos()))){
        return;
    }

    if(this->isVisible()){
        this->clearFocus();
    }
}

/*
Slot: that resets the LineEdit back to the starting state
*/
void LineEdit::reset() {
    this->deselect();
    this->buildText(-1);
}

/*
Slot: reloads the this->popup() maximum popup size & buildOutput
    :param widget: widget to base the size upon
*/
void LineEdit::reloadSize(const QWidget* widget){
    if(widget){
        this->popup()->updateRect(widget);
    }

    // Upon size changing should finish widget function
    if(this->isVisible()){
        this->clearFocus();
    }
}

/*
Slot: upon popup item activation adds data of activated entree to LineEdit text
    :param index: activated index
*/
void LineEdit::updatePopupActivated(const QModelIndex& index){
    if(index.isValid()){
        int index_data = index.data(Qt::UserRole +1).toInt();
        
        // Incase of negative values -> reset text, so make sure cursor is in correct location
        int index_data_str_length = 0;
        if(index_data >= 0){
            index_data_str_length = index.data(Qt::UserRole +1).toString().length();
        }

        this->buildText(index_data);
        this->setCursorPosition(this->text_write_start + index_data_str_length);
    }else{
        this->buildText(-1);
        this->setCursorPosition(this->text_write_start);
    }
}

/*
Slot: upon popup double clicking. Clear focus.
    :param index: activated index
*/
void LineEdit::updatePopupDblClicked(const QModelIndex& index){
    Q_UNUSED(index);
    this->clearFocus();
}


/*
Slot: reloads the this->popup() model

void LineEdit::reloadModel(const DataCytometers* data){
    if(data){
        qDebug() << "reload laser model";
    }
}
*/

} // Laser namespace
