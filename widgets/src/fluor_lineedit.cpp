/**** General **************************************************************
** Version:    v0.9.1
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_lineedit.h"

#include <QStyle>
#include <QAbstractItemView>
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QModelIndex>

#include <QMouseEvent>
#include <QKeyEvent>

#include <QDebug>

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QList>
#include <QLayout>
#include <QMargins>
#include <QScrollBar>

#include <QWindow>

namespace Fluor {

/*
Constructor: Constructs a Fluorophore LineEdit object.
    :param parent: pointer to parent widget
*/
LineEdit::LineEdit(QWidget* parent) : 
    QLineEdit(parent),
    _completer{nullptr},
    inline_selection{true},
    cursor_pos{0},
    entries_before{},
    entries_after{},
    prefix_text{""},
    prefix_length{0},
    postfix_text{""},
    postfix_length{0}
{
    // Reserve space for before_entries and after_entries
    this->entries_before.reserve(10);
    this->entries_after.reserve(10);

    // Setup QLineEdit
    this->setText("");
    this->setProperty("popup", false);  // needs a property for proper stylesheet handling
    this->setContextMenuPolicy(Qt::NoContextMenu);

    Fluor::Completer* completer_text = new Fluor::Completer(this);
    this->setCompleter(completer_text);

    this->installEventFilter(this);

    // Text editing connections
    QObject::connect(this, &Fluor::LineEdit::textEdited, this, &Fluor::LineEdit::updateTextEdited);

}

/*
Sets completer. Overwrite QCompleter::setCompleter as that one builds connections we need to overwrite
*/
void LineEdit::setCompleter(QCompleter* completer){
    // Remove previous completer
    if(this->_completer){
        QObject::disconnect(this->completer(), QOverload<const QString &>::of(&QCompleter::highlighted), this, &Fluor::LineEdit::updatePopupHighlighted);
        QObject::disconnect(this->completer(), QOverload<const QString &>::of(&QCompleter::activated), this, &Fluor::LineEdit::updatePopupActivated);
        //QObject::disconnect(this, &LineEdit::highlightPopup, static_cast<Fluor::Completer*>(this->completer()), &Fluor::Completer::updateHighlight);
        delete this->_completer;
    }
    
    this->_completer = completer;
    QObject::connect(this->completer(), QOverload<const QString &>::of(&QCompleter::highlighted), this, &Fluor::LineEdit::updatePopupHighlighted);
    QObject::connect(this->completer(), QOverload<const QString &>::of(&QCompleter::activated), this, &Fluor::LineEdit::updatePopupActivated);
    //QObject::connect(this, &LineEdit::highlightPopup, static_cast<Fluor::Completer*>(this->completer()), &Fluor::Completer::updateHighlight);
}

/*
Gets completer for LineEdit. See LineEdit::setCompleter for reasoning.
If no completer has been set, will set one before returning it
*/
QCompleter* LineEdit::completer() {
    if(this->_completer){
        return this->_completer;
    }

    // No completer has been set, so build one:
    Fluor::Completer* completer_text = new Fluor::Completer(this);
    this->setCompleter(completer_text);  

    return this->_completer;
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
Convenience function that calculates completion, builds text and sets selection
*/
void LineEdit::complete(){
    this->buildCompletion();
    QString completion = this->getCompletion();
    this->buildText(std::move(completion));
    this->buildSelection();
}

/*
Functions that talks to the this->completer() and effectively updates the popup view and completion state
*/
void LineEdit::buildCompletion() {
    QStringList entries = {this->entries_before + this->entries_after};
    Fluor::Completer* fluor_completer = static_cast<Fluor::Completer*>(this->completer());
    
    // Update completer (& popup) and get completion
    fluor_completer->updateCompleter(this->prefix_text, entries);
}

/*
Sets the (inline) selection
*/
void LineEdit::buildSelection(){
    // Inline Selection
    if(this->inline_selection && this->postfix_length != 0){
        if(this->prefix_length != 0){
            int selection_size = this->postfix_length;
            int selection_end = this->cursor_pos + this->postfix_length;
            this->setSelection(selection_end, -selection_size);
        }else if(this->postfix_length != 0){
            int selection_size = this->postfix_length;
            int selection_end = this->cursor_pos + this->postfix_length;
            this->setSelection(selection_end, -selection_size);
        }
    }else{
        this->setCursorPosition(this->cursor_pos);
    }
}

/*
Calculates and sets the selection of the text and updates text parameters
    :param completion: the completed prefix
*/
void LineEdit::buildText(const QString completion){
    // Build before and after text / adds separator if necessary
    QString before = this->entries_before.join(", ");
    if(before.length() > 0){
        before += ", ";
    }
    QString after = this->entries_after.join(", ");
    if(after.length() > 0){
        after = ", " + after;
    }

    QString text{""};

    // Check for completion emptiness -> highlight completion for example can have an empty prefix but valid completion
    if(this->prefix_text == "" && completion.isEmpty()){
        text = before + after;
        this->setText(text);
    }else if(completion == ""){
        text = before + this->prefix_text + after;
        this->setText(text);
    }else{
        text = before + completion + after;
        this->setText(text);
        // trick to have completion in its entirety in the LineEdit
        int cursor = before.length() + completion.length();
        this->setCursorPosition(cursor);
        this->repaint();
    }

    this->updateTextParameters(this->text(), before.length() + this->prefix_length);
}

/*
Function that request and returns the most likely completion from this->completer()
    :returns: completion
*/
QString LineEdit::getCompletion() {
    Fluor::Completer* fluor_completer = static_cast<Fluor::Completer*>(this->completer());
    QString completion = fluor_completer->getCompletion();

    return std::move(completion);
}

/*
Updates text parameters for the text_complete, event_tab, event_return functions
    :param text: the text to parse
    :param cursor: the cursor location
*/
void LineEdit::updateTextParameters(const QString& text, int cursor) {
    // Clear entries
    this->postfix_text = {""};
    this->postfix_length = 0;
    this->entries_before.clear();
    this->entries_after.clear();

    // Set cursor position (cursor only gets updated upon paint events -> happens as last step in updating text)
    this->cursor_pos = cursor;

    // Parsing states
    bool is_active{false};
    bool is_before_cursor{true};

    // Indexes of entries
    int tag_start{-1};
    int tag_end{-1};

    // Iterate through text
    for(int i = 0; i < text.length(); ++i){
        QChar letter{text[i]};

        if(cursor == i){
            is_active = {true};

            // parse prefix text
            if(tag_start == -1){
                this->prefix_text = {""};
                this->prefix_length = {0};
            }else{
                this->prefix_text = {text.mid(tag_start, i-tag_start)}; // must be i otherwise forgets the ignore-characters
                this->prefix_length = this->prefix_text.length();
            }

            // Reset entry parameters
            tag_start = {i};
            tag_end = {i};

            is_before_cursor = {false};
        }

        if(letter.isSpace()){
            // ignore character
            continue;
        }else if(letter == QChar{','}){
            // entry separator
            if(tag_start != -1){
                // -> parse entry
                if(is_before_cursor){
                    this->entries_before.append(text.mid(tag_start, tag_end-tag_start+1));
                }else if(is_active){
                    //postfix text - need to check for false-positive 
                    QString postfix_text = {text.mid(tag_start, tag_end-tag_start+1)};
                    if(postfix_text == ","){
                        this->postfix_text = "";
                    }else{
                        this->postfix_text = std::move(postfix_text);
                    }
                    this->postfix_length = this->postfix_text.length();
                    is_active = {false};
                }else{
                    this->entries_after.append(text.mid(tag_start, tag_end-tag_start+1));
                }

                // Reset entry parameters
                tag_start = {-1};
                tag_end = {-1};
            }else{
                // No entry to be parsed, so ignore (also tag_start/end should still be -1, so no need to reasign)
                continue;
            }
        }else{
            // valid character
            if(tag_start == -1){
                tag_start = {i};
            }
            tag_end = {i};
        }
    }

    // handle last entry
    if(is_before_cursor){
        // Cursor in last place
        if(tag_start == -1){
            this->prefix_text = {""};
            this->prefix_length = {0};
        }else{
            this->prefix_text = {text.mid(tag_start, text.length()-tag_start)};
            this->prefix_length = this->prefix_text.length();
        }
    }else if(is_active){
        // postfix text
        if(tag_start != -1){
            this->postfix_text = {text.mid(tag_start, tag_end-tag_start+1)};
            this->postfix_length = this->postfix_text.length();
        }
    }else{
        // last entry, add if any
        if(tag_start != -1){
            this->entries_after.append(text.mid(tag_start, tag_end-tag_start+1));
        }
    }
}

/*
Updates selection upon popup highlighting
*/
void LineEdit::updatePopupHighlighted(const QString &text){
    // If the entree is disabled, the highlighted signal returns the completion text
    if(this->prefix_text != text){
        this->buildText(text);
        this->buildSelection();
    }
}

/*
Updates selection upon popup activation
*/
void LineEdit::updatePopupActivated(const QString &text){
    this->buildText(text);
    this->setCursorPosition(this->cursor_pos + this->postfix_length);
}

/*
Sets cursor to the end of the current active text
*/
void LineEdit::updateTab(){
    // Tab, in case of no entries_after adds a ", " and sets cursor to the end
    // Tab, in case of entries_after goes to the start of the first after entry 
    if(this->entries_after.length() == 0){
        QString text = this->text();
        text += ", ";
        this->updateTextParameters(text, text.length());
        this->buildCompletion();
        this->buildText(QString{""});
        this->buildSelection();
    }else{
        // cursorposition (entries_before + prefix) + postfix + ", "
        int cursor = this->cursorPosition() + this->postfix_length + 2;
        this->updateTextParameters(this->text(), cursor);
        this->buildSelection();
    }
}

/*
Independently of selection, removes the letter in front of the cursor and runs complete()
*/
void LineEdit::updateBackspace() {
    if(this->cursorPosition() > 0){
        QString text{""};
        if(this->prefix_text == ""){
            text = this->text().remove(this->cursorPosition()-2, 2);
            this->updateTextParameters(text, this->cursorPosition() -2);
        }else{
            text = this->text().remove(this->cursorPosition()-1, 1);
            this->updateTextParameters(text, this->cursorPosition() -1);
        }       
    }
    this->buildCompletion();
    // Makes sure a new entry, entered by typing, doesnt start fill the text with a completion
    if(this->prefix_length != 0){
        QString completion = this->getCompletion();
        this->buildText(std::move(completion));
    }else{
        this->buildText(QString{""});
    }
    this->buildSelection();
}

/*
Independently of selection, removes a letter after the cursor and runs complete()
*/
void LineEdit::updateDelete(){
    if(this->cursorPosition() < this->text().length()){
        QString text = this->text().remove(this->cursorPosition(), 1);
        this->updateTextParameters(text, this->cursorPosition());
    }
    
    this->buildCompletion();
    // Makes sure a new entry, entered by typing, doesnt start fill the text with a completion
    if(this->prefix_length != 0){
        QString completion = this->getCompletion();
        this->buildText(std::move(completion));
    }else{
        this->buildText(QString{""});
    }
    this->buildSelection();
}

/*
Moves cursor and activates selection() after Key_Left press
*/
void LineEdit::updateLeft(){
    int cursor_pos = this->cursorPosition();
    if(cursor_pos > 0){
        if(this->prefix_length == 0){
            this->updateTextParameters(this->text(), cursor_pos - 2);
        }else{
            this->updateTextParameters(this->text(), cursor_pos - 1);
        }
        // Make sure trailing ", " are removed
        if(this->postfix_length == 0 && this->entries_after.length() == 0){
            this->buildCompletion();
            QString completion = this->getCompletion();
            this->buildText(std::move(completion));
            this->buildSelection();
        }else{
            this->buildCompletion();
            this->buildSelection();
        }
    }
}

/*
Moves cursor and activates selection() after Key_Right press
*/
void LineEdit::updateRight(){
    int cursor_pos = this->cursorPosition();
    if(cursor_pos <= this->text().length()){
        if(this->postfix_text == "" && this->entries_after.length() != 0){
            this->updateTextParameters(this->text(), cursor_pos + 2);
        }else{
            this->updateTextParameters(this->text(), cursor_pos + 1);
        }
        this->buildCompletion();
        this->buildSelection();
    }
}

/*
Moves cursor to the begin and activates selection();
*/
void LineEdit::updateHome(){
    this->updateTextParameters(this->text(), 0);
    this->buildCompletion();
    this->buildSelection();
}

/*
Moves cursor to the end;
*/
void LineEdit::updateEnd(){
    this->updateTextParameters(this->text(), this->text().length());
    this->buildCompletion();
    this->buildSelection();
}

/*
eventFilter for FocusIn, FocusOut events and Keypress events to properly update the text
    :param obj: source object
    :param event: the event
*/
bool LineEdit::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::FocusIn && obj == this){
        this->toggleStylePopup(true);
    }else if(event->type() == QEvent::FocusOut && obj == this){
        if(!this->completer()->popup()->hasFocus()){
            this->toggleStylePopup(false);
            this->buildOutput();
        }
    }else if(event->type() == QEvent::KeyPress){
        QKeyEvent* event_key = static_cast<QKeyEvent*>(event);
        if(event_key->key() == Qt::Key_Backspace){
            // If using setSelection backspace doesnt work properly -> removes entire selection
            this->updateBackspace();
            return true;
        }else if(event_key->key() == Qt::Key_Delete){
            this->updateDelete();
            return true;
        }else if(event_key->key() == Qt::Key_Tab){
            this->updateTab();
            return true;
        }else if(event_key->key() == Qt::Key_Enter || event_key->key() == Qt::Key_Return){
            this->clearFocus();
            return true;
        }else if(event_key->key() == Qt::Key_Escape){
            this->setText("");
            this->clearFocus();
            return true;
        }else if(event_key->key() == Qt::Key_Left){
            this->updateLeft();
            return true;
        }else if(event_key->key() == Qt::Key_Right){
            this->updateRight();
            return true;
        }else if(event_key->key() == Qt::Key_PageDown || event_key->key() == Qt::Key_PageUp){
            // Ignore pagedown and pageup keys
            return true;
        }else if(event_key->key() == Qt::Key_Home){
            this->updateHome();
            return true;
        }else if(event_key->key() == Qt::Key_End){
            this->updateEnd();
            return true;
        }
    }else if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        int mouse_pos = this->cursorPositionAt(QPoint(event_mouse->x(), event_mouse->y()));

        this->updateTextParameters(this->text(), mouse_pos);
        // Correct for the ", " space
        if(this->prefix_length == 0 && this->postfix_length != 0){
            if(this->postfix_text[0] == " " && this->cursor_pos > 0){
                this->updateTextParameters(this->text(), this->cursor_pos - 1);
            }
        }
        this->buildCompletion();
        this->buildSelection();
        return true;
    }else if(event->type() == QEvent::MouseMove){
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        if(event_mouse->buttons() == Qt::LeftButton){
            int mouse_pos = this->cursorPositionAt(QPoint(event_mouse->x(), event_mouse->y()));

            this->updateTextParameters(this->text(), mouse_pos);
            // Correct for the ", " space
            if(this->prefix_length == 0 && this->postfix_length != 0){
                if(this->postfix_text[0] == " " && this->cursor_pos > 0){
                    this->updateTextParameters(this->text(), this->cursor_pos - 1);
                }
            }
            this->buildCompletion();
            this->buildSelection();
            return true;
        }
    }else if(event->type() == QEvent::MouseButtonDblClick){
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        if(event_mouse->button() == Qt::LeftButton){
            int mouse_pos = this->cursorPositionAt(QPoint(event_mouse->x(), event_mouse->y()));
            this->updateTextParameters(this->text(), mouse_pos);
            // Move cursor to first entree and update afterwards
            this->updateTextParameters(this->text(), this->cursor_pos - this->prefix_length);
            this->buildCompletion();
            this->buildSelection();
        }
        return true;
    }else if(event->type() == QEvent::MouseButtonRelease){
        return true;
    }

    return QLineEdit::eventFilter(obj, event);
}

/*
Slot: shows the button and sets-up the starting state
*/
void LineEdit::showButton(){
    // Set the focus
    this->setFocus();

    // Show the button
    this->show();
    
    // Build first completion
    this->buildCompletion();
}

/*
Slot: hides and resets button
*/
void LineEdit::hideButton(){
    this->reset();
    this->hide();
}

/*
Slot: unfocuses the widget
*/
void LineEdit::unfocus(QEvent* event){
    Q_UNUSED(event);
    if(this->hasFocus()){
        this->clearFocus();
    }
}

/*
Slot: that resets the LineEdit back to the starting state
*/
void LineEdit::reset() {
    this->deselect();
    this->setText("");
    this->updateTextParameters(QString{""}, 0);
}

/*
Slot: updates the LineEdit state upon text editing
    :param text: new Text
*/
void LineEdit::updateTextEdited(const QString& text) {
    this->updateTextParameters(text, this->cursorPosition());
    this->buildCompletion();
    
    // Makes sure a new entry, entered by typing, doesnt start fill the text with a completion
    if(this->prefix_length != 0){
        QString completion = this->getCompletion();
        this->buildText(std::move(completion));
    }else{
        this->buildText(QString{""});
    }
    this->buildSelection();
}

/*
Slot: reloads the this->completer() maximum popup size
    :param widget: widget to base the size upon
*/
void LineEdit::reloadCompleterPopupSize(const QWidget* widget){
    if(widget){
        static_cast<Fluor::Completer*>(this->completer())->updatePopupRect(widget);
    }
}

/*
Slot: reloads the this->completer() model
*/
void LineEdit::reloadCompleterModel(const DataFluorophores* data){
    // Make sure to delete the old model, as it is not deleted upon resetting
    if(data){
        qDebug() << "reload completer model";
    }
}

/*
Parses the current text and returns a list of unique entrees.
*/
void LineEdit::buildOutput() {
    // update text parameters
    // cursor position is 0, so postfix_text is also a full entree
    this->updateTextParameters(this->text(), 0);

    QStringList outputs{this->postfix_text};
    outputs.append(this->entries_after);

    // Reset button
    this->reset();

    // Remove duplicates
    outputs.removeDuplicates();

    qDebug() << "Fluor::LineEdit: emits output: " << outputs;

    // Emit output signal
    emit this->output(outputs);

    emit this->finished();
}

/*
Constructor: Constructs a Completer for Fluor::LineEdit object.
    :param parent: pointer to parent widget
*/
Completer::Completer(QWidget* parent) :
    QCompleter(parent),
    mouse_pressed(false),
    select_active_row(true),
    popup_max_size(),
    //model_list({QString{"No Data Loaded"}}),
    model_list({QString{"PE"}, QString{"APC"}, QString{"FITC"}, QString{"BV410"}, QString{"BV650"}, QString{"BV735"}, QString{"BV785"}}),
    completion("")
{
    this->setWidget(parent);
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompletionMode(QCompleter::PopupCompletion);
    this->setMaxVisibleItems(50);
    this->setWrapAround(true);

    // Build & set model
    QStandardItemModel* model_standarditem = new QStandardItemModel{this};
    for(int i=0; i<model_list.length(); ++i){
        QStandardItem *item = new QStandardItem(QString(model_list[i]));
        model_standarditem->appendRow(item);
    }
    this->setModel(model_standarditem);

    // Set popup item deligate for proper style handling; normally inherits QItemDelegate
    this->popup()->setObjectName("fluor_popup");

    QStyledItemDelegate* delegate_popup = new QStyledItemDelegate{this};
    this->popup()->setItemDelegate(delegate_popup);
    this->popup()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->popup()->setWindowFlag(Qt::NoDropShadowWindowHint);

    // Sets base size of popup (not sure if relevant in this case -> likely a draw() call had to happen before)
    this->updatePopupRect();

    // Event managing
    this->installEventFilter(this);

    // Need to use old-style connect to connect during runtime to the Q_PRIVATE_SLOT(_q_complete)
    QObject::connect(this, SIGNAL(_q_complete(QModelIndex)), static_cast<QCompleter*>(this), SLOT(_q_complete(QModelIndex)));
}

/*
Returns the current completion. Use updateCompleter() to update the completion
*/
const QString& Completer::getCompletion() const{
    return(this->completion);
}

/*
Modified QCompleter::showPopup() to correct the popup size to not be bigger then a specified QWidget
*/
void Completer::showPopup(){
    QWidget* parent_widget = static_cast<QWidget*>(this->parent());

    // Set height of the QListView
    // +14 depends on padding stylesheet parameter
    int columns{1};
    if(this->completionMode() == QCompleter::UnfilteredPopupCompletion){
        columns = this->maxVisibleItems();
    }else{
        if(this->completionPrefix() == QString{""}){
            columns = this->maxVisibleItems();
        }else{
            columns = this->completionCount();
            if(columns < 1){
                columns = 1;
            }
        }
    }

    // Calculate height-necessary-to-fit-columns
    int height = this->popup()->sizeHintForRow(0) * std::min(columns, this->model()->rowCount()) + 14;

    QScrollBar* horizontal_scrollbar = this->popup()->horizontalScrollBar();
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
    if(width > this->popup_max_size.width()){
        width = this->popup_max_size.width();
    }
    if( (pos.x() + width) > (this->popup_max_size.x() + this->popup_max_size.width()) ){
        pos.setX(this->popup_max_size.x() + this->popup_max_size.width() - width);
    }
    if(pos.x() < this->popup_max_size.x()){
        pos.setX(this->popup_max_size.x());
    }

    // Adjust y and height to fit most efficient in popup_max_size QRect
    int top = pos.y() - parent_height - this->popup_max_size.top() + 2;
    int bottom = this->popup_max_size.y() + this->popup_max_size.height() - pos.y();
    height = std::max(height, this->popup()->minimumHeight());
    if(height > bottom){
        height = std::min(std::max(top, bottom), height);
        if(top > bottom){
            pos.setY(pos.y() - height - parent_height + 2);
        }
    }

    this->popup()->setGeometry(pos.x(), pos.y(), width, height);

    if(!this->popup()->isVisible()){
        this->popup()->show();
    }
}

/*
Evenfilter for KeyPress events
    :param obj: event source
    :param event: event
*/
bool Completer::eventFilter(QObject *obj, QEvent *event){
    if(event->type() == QEvent::Show && this->popup() == obj){
        emit this->popupVisible(true);
        return true;
    }else if(event->type() == QEvent::Hide && this->popup() == obj){
        emit this->popupVisible(false);
        return true;
    }

    if(event->type() == QEvent::MouseButtonPress && this->popup() == obj){
        if(!this->popup()->underMouse()){
            // Get pointer to parent object and check if widget!
            QWidget* widget_parent = dynamic_cast<QWidget*>(this->parent());
            QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);

            if(widget_parent){
                if(widget_parent->rect().contains(widget_parent->mapFromGlobal(event_mouse->globalPos()))){
                    this->mouse_pressed = true;
                    this->parent()->eventFilter(obj, event);
                }else{
                    this->popup()->hide();
                }
            }else{
                qWarning() << "Fluor::Completer::eventFilter - parent does not inherit from QWidget";
            }
            return true;
        }
        return false;
    }else if(event->type() == QEvent::MouseMove){
        if(!this->popup()->underMouse()){
            this->parent()->eventFilter(obj, event);
        }else{
            return false;
        }
    }else if(event->type() == QEvent::MouseButtonDblClick){
        if(!this->popup()->underMouse()){
            QWidget* widget_parent = dynamic_cast<QWidget*>(this->parent());
            QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
            
            if(widget_parent){
                if(widget_parent->rect().contains(widget_parent->mapFromGlobal(event_mouse->globalPos()))){
                    this->parent()->eventFilter(obj, event);
                }
            }else{
                qWarning() << "Fluor::Completer::eventFilter - parent does not inherit from QWidget";
            }

            return true;
        }else{
            return false;
        }
    }else if(event->type() == QEvent::MouseButtonRelease){
        if(this->mouse_pressed){
            this->mouse_pressed = false;
            this->parent()->eventFilter(obj, event);
            return true;
        }else{
            return false;
        }
    }

    // Some events need to be explicitly forwared to parent, because popup() doesnt have its own parent (atleast in pyqt)
    if(event->type() == QEvent::KeyPress){
        QKeyEvent* event_key = static_cast<QKeyEvent*>(event);
        
        if(event_key->key() == Qt::Key_Tab){
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Backspace){
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Delete){
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Enter || event_key->key() == Qt::Key_Return){
            this->popup()->close();
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Down || event_key->key() == Qt::Key_Up){
            // Need to return false, otherwise the fall-back eventhandling allows for selection of inactive QItems
            return false;
        }else if(event_key->key() == Qt::Key_Left || event_key->key() == Qt::Key_Right){
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Escape){
            this->popup()->close();
            this->parent()->eventFilter(obj, event);
            return true;
        }else if(event_key->key() == Qt::Key_Home || event_key->key() == Qt::Key_End){
            this->parent()->eventFilter(obj, event);
            return true;
        }
    }

    return QCompleter::eventFilter(obj, event);
}

/*
Reimplemented complete slot, runs modified showPopup(). Based on QCompleter.cpp
    :param rect: UNUSED - needed for proper reimplementation
*/
void Completer::complete(const QRect& rect){
    Q_UNUSED(rect);

    QModelIndex index = this->currentIndex();
    if(this->completionMode() == QCompleter::InlineCompletion){
        if(index.isValid()){
            emit this->_q_complete(index);
        }
        return;
    }

    if(this->completionMode() == QCompleter::PopupCompletion && !index.isValid()){
        if(this->popup()->isVisible()){
            this->popup()->hide();
        }
        return;
    }else if(this->completionMode() == QCompleter::UnfilteredPopupCompletion && this->model()->rowCount() == 0){
        if(this->popup()->isVisible()){
            this->popup()->hide();
        }
        return;
    }

    // To make sure that popup() has been build
    this->popup();

    if(this->completionMode() == QCompleter::UnfilteredPopupCompletion){
        this->setCurrentRow(index.row());
    }

    this->showPopup();
}

/*
Update the completer state, reenabled/disables the internal model, and emits the completed prefix
    :param prefix: the completion prefix
    :param disabled: the list of disabled model entrees
    :emits: completed signal
*/
void Completer::updateCompleter(const QString prefix, const QStringList& disabled){
    // Update model - might want to link the QTextEdit model with the QCompleter model
    const QStandardItemModel* completer_model = static_cast<QStandardItemModel*>(this->model());

    for(int i=0; i < completer_model->rowCount(); ++i){     
        if(disabled.contains(completer_model->item(i)->text())){
            completer_model->item(i)->setEnabled(false);
        }else{
            completer_model->item(i)->setEnabled(true);
        }
    }

    // Activate completer
    this->setCompletionPrefix(std::move(prefix));
    this->complete();

    // Get first non-disabled completion entree
    QString completion{""};
    int completions_total = this->completionCount();
    if(completions_total != 0){
        for(int i=0; i<completions_total; ++i){
            this->setCurrentRow(i);
            if(!disabled.contains(this->currentCompletion())){
                completion = this->currentCompletion();
                break;
            }
        }
    }

    //sets and emit completion
    this->completion = completion;

    emit this->completed(std::move(completion));
}

/*
Updates the popup max size parameter
    :param widget: (optional) the widget to base the popup size upon, or if not specified bases it upon the screen size
*/
void Completer::updatePopupRect(const QWidget* widget){
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

    this->popup_max_size = size;
}

/*
Tries to find the entry and selects it. This does force a highlighted() signal to be fired. 
    :param entry: the entry to select
*/
void Completer::updateHighlight(const QString& entry){
    //select active popup QListView
    qDebug() << "updateHighlight";
    if(this->select_active_row && entry != ""){
        // Enters infinite loop upon first-letter-in-no-entry
        QModelIndex index = this->popup()->indexAt(QPoint(0, 0));
        if(!(index.isValid() && index.data().toString() == entry)){
            while(this->popup()->viewport()->rect().contains(QPoint(0, this->popup()->visualRect(index).y() + this->popup()->visualRect(index).height() + 1 ))){
                if(index.isValid() && index.data().toString() == entry){
                    break;
                }
                index = this->popup()->indexAt(QPoint(0, this->popup()->visualRect(index).y() + this->popup()->visualRect(index).height() + 1 ));
            }
        }
        if(index.isValid()){
            QItemSelectionModel* selection = this->popup()->selectionModel();
            selection->select(index, QItemSelectionModel::ClearAndSelect);
        }
    }
}

} // Fluor namespace
