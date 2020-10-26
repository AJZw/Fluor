/**** General **************************************************************
** Version:    v0.9.11
** Date:       2020-10-27
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "fluor_lineedit.h"
#include "general_widgets.h"

#include <QStyle>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
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

namespace Fluor {

/*
Constructor: Constructs a Fluorophore LineEdit object.
    :param parent: pointer to parent widget
*/
LineEdit::LineEdit(QWidget* parent) : 
    QLineEdit(parent),
    _completer(nullptr),
    lookup_id(),
    lookup_names(),
    incache_names(),
    inline_selection(true),
    cursor_pos(0),
    entries_before(),
    entries_after(),
    prefix_text(""),
    prefix_length(0),
    postfix_text(""),
    postfix_length(0)
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
Clear focus of lineedit and hides popup
*/
void LineEdit::clearFocus(){
    static_cast<Fluor::Completer*>(this->completer())->hidePopup();
    this->toggleStylePopup(false);
    this->buildOutput();

    QLineEdit::clearFocus();
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
    int cache_size = static_cast<int>(this->incache_names.size());

    QStringList entries;
    entries.reserve(this->entries_before.size() + this->entries_after.size() + cache_size + 5);

    // Add the naming alternatives
    for(const QString& entry : this->entries_before){
        entries.append(this->lookup_names[entry]);
    }
    for(const QString& entry : this->entries_after){
        entries.append(this->lookup_names[entry]);
    }
    for(const QString& entry : this->incache_names){
        entries.append(this->lookup_names[entry]);
    }

    // Update completer (& popup) and get completion
    Fluor::Completer* fluor_completer = static_cast<Fluor::Completer*>(this->completer());
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
Slot: Updates text parameters for the text_complete, event_tab, event_return functions
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
Slot: Updates text and selection upon popup highlighting
    :param text: text to add
*/
void LineEdit::updatePopupHighlighted(const QString& text){
    // If the entree is disabled, the highlighted signal returns the completion text
    if(text.isEmpty()){
        if(this->prefix_text == ""){
            // No entree selected, and no prefix -> become empty
            this->buildText(QString(""));
        }
        //else{
        //    // No entree, but prefix, so get first completion
        //    this->buildCompletion();
        //    QString completion = this->getCompletion();
        //    this->buildText(std::move(completion));
        //}
    }else if(this->prefix_text != text){
        this->buildText(text);
        this->buildSelection();
    }
}

/*
Updates text and selection upon popup activation
    :param text: text to add
*/
void LineEdit::updatePopupActivated(const QString& text){
    this->buildText(text);
    this->setCursorPosition(this->cursor_pos + this->postfix_length);
}

/*
Updates text and selection upon popup double click
    :param text: text to add
*/
void LineEdit::updatePopupDblClicked(const QString& text){
    if(!text.isEmpty()){
        if(this->entries_after.length() == 0){
            QString text = this->text();
            text += ", ";
            this->updateTextParameters(text, text.length());
            this->buildCompletion();
            this->buildText(QString{""});
            this->buildSelection();
        }else{
            int cursor = this->cursorPosition() + 2;
            this->updateTextParameters(this->text(), cursor);
            this->buildCompletion();
            //this->buildText(QString{""});
            this->buildSelection();
        }
    }
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
        this->buildCompletion();
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
    switch(event->type()){
    case QEvent::FocusIn:
        if(obj == this){
            this->toggleStylePopup(true);
        }
        return false;
    case QEvent::FocusOut:
        return true;
    case QEvent::KeyPress: {
        QKeyEvent* event_key = static_cast<QKeyEvent*>(event);
        switch(event_key->key()){
        case Qt::Key_Backspace:
            this->updateBackspace();
            return true;
        case Qt::Key_Delete:
            this->updateDelete();
            return true;
        case Qt::Key_Tab:
            this->updateTab();
            return true;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            this->clearFocus();
            return true;
        case Qt::Key_Escape:
            this->setText("");
            this->clearFocus();
            return true;
        case Qt::Key_Left:
            this->updateLeft();
            return true;
        case Qt::Key_Right:
            this->updateRight();
            return true;
        case Qt::Key_Home:
            this->updateHome();
            return true;
        case Qt::Key_End:
            this->updateEnd();
            return true;
        // key to propagate to popup
        case Qt::Key_PageUp:
        case Qt::Key_Up:{
            return static_cast<Fluor::Completer*>(this->completer())->popup()->updateKeyUp();
        }
        case Qt::Key_PageDown:
        case Qt::Key_Down:{
            return static_cast<Fluor::Completer*>(this->completer())->popup()->updateKeyDown();
        }
        default:
            break;
        }
        break;
    }
    case QEvent::MouseButtonPress: {
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
    }
    case QEvent::MouseMove: {
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
        }
        return true;
    }
    case QEvent::MouseButtonDblClick: {
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
    }
    case QEvent::MouseButtonRelease:
        return true;
    default:
        break;
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
    // Make sure it is the correct mouse event
    if(event->type() != QEvent::MouseButtonRelease){
        return;
    }

    // Can now cast statically
    QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

    // Check if it is a click on this widget -> if so, ignore
    if(this->rect().contains(this->mapFromGlobal(mouse_event->globalPos()))){
        return;
    }

    // Clicks on the popup do not propagate to the Main::Controller
    // But clicks moved into the popup do propagate, so check
    Fluor::Popup* popup = static_cast<Fluor::Completer*>(this->completer())->popup();
    if(popup->rect().contains(popup->mapFromGlobal(mouse_event->globalPos()))){
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
void LineEdit::reloadSize(const QWidget* widget){
    if(widget){
        static_cast<Fluor::Completer*>(this->completer())->updatePopupRect(widget);
    }

    // This slot is fired when the main widget changes size/location -> finish editing.
    if(this->isVisible()){
        this->clearFocus();
    }
}

/*
Slot: reloads the this->completer() model
*/
void LineEdit::reloadData(const Data::FluorophoreReader& data){
    // Make hard copies, that way data can be invalidated without issues here / race conditions
    this->lookup_id = data.getFluorID();
    this->lookup_names = data.getFluorNames();

    static_cast<Fluor::Completer*>(this->completer())->buildModel(data.getFluorName());
}

/*
Slot: synchronizes the fluorophores names loaded into the cache.items into the lineedit
*/
void LineEdit::sync(const std::vector<Cache::ID>& input){
    std::vector<QString> new_vector;
    new_vector.reserve(input.size());

    for(const Cache::ID& id : input){
        new_vector.push_back(id.name);
    }

    std::swap(this->incache_names, new_vector);
}

/*
Parses the current text and returns a list of unique entrees.
*/
void LineEdit::buildOutput() {
    // update text parameters
    // cursor position is 0, so postfix_text is also a full entree
    this->updateTextParameters(this->text(), 0);

    QStringList input{this->postfix_text};
    input.append(this->entries_after);

    // Reset button
    this->reset();

    // Sanetize output and transform into set of fluorophoreIDs
    std::set<Data::FluorophoreID> input_unique;

    // Counter, independent from loop because duplicate entrees / hash misses should not increase the counter
    unsigned int counter(0);

    for(const QString& entree : input){
        // Lookup in ID unordered_map
        std::unordered_map<QString, QString>::const_iterator entree_id = this->lookup_id.find(entree);

        if(entree_id == this->lookup_id.end()){
            // hashmiss so ignore
            continue;
        }

        std::pair<std::set<Data::FluorophoreID>::iterator, bool> emplace_succes = input_unique.emplace(entree_id->second, entree, counter);

        if(emplace_succes.second){
            ++counter;
        }
    }

    // Transform into a vector for further usage
    std::vector<Data::FluorophoreID> output;
    std::copy(input_unique.begin(), input_unique.end(), std::back_inserter(output));

    // Before emitting list, check if it contains anything, if not do not emit
    if(output.size() != 0){
        { // Scoping to fire the debug message before the signal gets emitted
        qDebug() << "Fluor::LineEdit: emits output:" << output;
        }
        emit this->output(output);
    }

    emit this->finished();
}

// ################################################################## //

/*
Constructor: Builds the popup for the Laser::LineEdit
    :param parent: parent widget
*/
Popup::Popup(QWidget* parent) :
    QListView(parent),
    margin_scrollbar(0),
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
    auto old_delegate = this->itemDelegate();
    QStyledItemDelegate* delegate_popup = new QStyledItemDelegate{this};
    this->setItemDelegate(delegate_popup);
    delete old_delegate;

    // Replace scrollbar to be able to hook into hide/show events
    General::ScrollBar* vertical_scrollbar = new General::ScrollBar(this);
    this->setVerticalScrollBar(vertical_scrollbar);

    QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::showing, this, &Fluor::Popup::showingScrollBar);
    QObject::connect(static_cast<General::ScrollBar*>(this->verticalScrollBar()), &General::ScrollBar::hiding, this, &Fluor::Popup::hidingScrollBar);

    // Connect to ModelIndex signals and signal out QString ouputs
    QObject::connect(this, &Fluor::Popup::pressed, this, &Fluor::Popup::activate);

    // Make sure viewport() always inherits QAbstractItemView (or change code in eventFilter)
    this->viewport()->installEventFilter(this);
    this->setViewportMargins(0, 0, this->margin_scrollbar, 0);
}

/*
Getter for layout spacing property, as it has to return a QString it returns the value in pixels
*/
QString Popup::viewportMarginsScrollBar() const {
    return QString::number(this->margin_scrollbar, 'f', 0);
}

/*
Receives layout scaling properties from the stylesheet
*/
void Popup::setViewportMarginsScrollBar(QString layout_spacing_scroll_bar){
    this->margin_scrollbar = layout_spacing_scroll_bar.toInt();
    if(this->verticalScrollBar()->isVisible()){
        this->setViewportMargins(0, 0, layout_spacing_scroll_bar.toInt(), 0);
    }else{
        this->setViewportMargins(0, 0, 0, 0);
    }
}

/*
Modified setModel to automatically connect the model output with resizing. Does not take ownership of model.
*/
void Popup::setModel(QAbstractItemModel* model){
    // Disconnect previous model
    if(this->model()){
        QObject::disconnect(this->model(), &QAbstractItemModel::modelReset, this, &Fluor::Popup::showPopup);
    }
    
    QListView::setModel(model);
    QObject::connect(this->model(), &QAbstractItemModel::modelReset, this, &Fluor::Popup::showPopup);
}

/*
Updates the popup as if a Key_Up is pressed (popup will never receive these events so is propagated from parent)
    :returns: whether the event is (fully) handled
*/
bool Popup::updateKeyUp(){
    QModelIndex index_current = this->currentIndex();
    
    int row_max = index_current.row();
    if(!index_current.isValid()){
        row_max = this->model()->rowCount();
    }

    // Because rows can be disabled find bottom most enabled row
    int row_valid = -1;
    for(int i = row_max -1; i >= 0; --i){
        QModelIndex index_valid = this->model()->index(i, 0);
        if( (index_valid.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled ){
            row_valid = i;
            break;
        }
    }

    // No valid rows -> dont select anything
    if(row_valid < 0){
        this->setCurrentIndex(QModelIndex());

        // Make sure the top most entree is visible -> as feedback that the top has been reached
        this->scrollToTop();
    }else{
        QModelIndex index_last = this->model()->index(row_valid, 0);
        this->setCurrentIndex(index_last);
    }

    // Emit highlighted signal 
    this->highlight(this->currentIndex());

    return true;
}

/*
Updates the popup as if a Key_Down is pressed (popup will never receive these events so is propagated from parent)
    :returns: whether the event is (fully) handled
*/
bool Popup::updateKeyDown(){
    QModelIndex index_current = this->currentIndex();
    
    int row_max = this->model()->rowCount();
    int row_current = 0;
    if(index_current.isValid()){
        row_current = index_current.row() + 1; 
    }

    // Because rows can be disabled find bottom most enabled row
    int row_valid = -1;
    for(int i = row_current; i < row_max; ++i){
        QModelIndex index_valid = this->model()->index(i, 0);
        if( (index_valid.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled ){
            row_valid = i;
            break;
        }
    }

    // No valid rows -> dont select anything
    if(row_valid < 0){
        this->setCurrentIndex(QModelIndex());
        // Scroll to bottom upon no valid lower entree -> as feedback that end has been reached
        this->scrollToTop();
    }else{
        QModelIndex index_last = this->model()->index(row_valid, 0);
        this->setCurrentIndex(index_last);
    }

    // Emit highlighted signal 
    this->highlight(this->currentIndex());
    return true;
}

/*
Eventfilter for the viewport. Intercepts mousemove events to correctly fire highlight events
    :param obj: object that fires the event
    :param event: event
    :returns: whether the event is (fully) handled
*/
bool Popup::eventFilter(QObject* obj, QEvent* event){
    switch(event->type()){
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:{
        QMouseEvent* event_mouse = static_cast<QMouseEvent*>(event);
        if(event_mouse->button() == Qt::LeftButton){
            break;
        }else{
            return true;
        }
        break;
    }
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

            if(index_current.isValid() && (index_current.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled){
                emit this->highlighted(index_current.data(Qt::DisplayRole).toString());
            }else{
                emit this->highlighted(QString());
                // No need to update the itemview
                // This prevents disabled items of 'taking' activity from the last selected valid item
                return true;
            }
        }else{
            return true;
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
            
            if(index_current.isValid() && (index_current.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled){
                emit this->dblClicked(index_current.data(Qt::DisplayRole).toString());
            }else{
                emit this->dblClicked(QString());
            }
        }else{
            return true;
        }
        // Must return true; if not double click will also emit activated event
        // This will cause side-effects with the dblClick() signal and capture of LineEdit
        return true;
    }
    default:
        break;    
    }
    return QWidget::eventFilter(obj, event);
}

/*
Slot: Receives hiding signal from the vertical scrollbar and removes scrollbar margin
*/
void Popup::hidingScrollBar(){
    this->setViewportMargins(0, 0, 0, 0);
}

/*
Slot: Receives showing signal from the vertical scrollbar and adds scrollbar margin
*/
void Popup::showingScrollBar(){
    this->setViewportMargins(0, 0, this->margin_scrollbar, 0);
}

/*
Slot: retreives data of clicked ModelIndex and emits activated
    :param index: model index of clicked entree
*/
void Popup::activate(const QModelIndex& index){
    if(index.isValid() && (index.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled){

        QString index_data = index.data(Qt::DisplayRole).toString();
        
        emit this->activated(index_data);
    }
}

/*
Slot: retreives data of highlighted ModelIndex and emits highlighted
    :param index: model index of clicked entree
*/
void Popup::highlight(const QModelIndex& index){
    // Check if entree is valid and is enabled
    if(index.isValid() && (index.flags()&(Qt::ItemIsEnabled)) == Qt::ItemIsEnabled){
        QString index_data = index.data(Qt::DisplayRole).toString();
        
        emit this->highlighted(index_data);
    }else{
        emit this->highlighted(QString());
    }
}

/*
Slot: Modified QCompleter::showPopup() to correct the popup size to not be bigger then a specified QWidget
    :param parent: parent widget - completer takes ownership so physical parent has to be parsed
*/
void Popup::showPopup(){
    QWidget* parent = static_cast<QWidget*>(this->parent());
    // Set height of the QListView
    // +14 depends on padding stylesheet parameter
    // Calculate height-necessary-to-fit-columns
    int height = this->sizeHintForRow(0) * std::min(this->max_visible_items, this->model()->rowCount()) + 14;
    // sizeHintForRow is not updated until the listview has been painted with the new (DPI adjusted) font

    QScrollBar* horizontal_scrollbar = this->horizontalScrollBar();
    if(horizontal_scrollbar){
        if(horizontal_scrollbar->isVisible()){
            height += horizontal_scrollbar->sizeHint().height();
        }
    }

    // Get parent widget height and width
    int parent_height = parent->height();
    QPoint pos = parent->mapToGlobal(QPoint(0, parent_height - 1));
    int width = parent->width();

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
        // Upon dpi changes, not only the row height/width changes. This change is only updated upon painting of the listview
        // Secondly, setGeometry fails somewhere downstream in Qt (i think). Running show/hide/show makes it work...
        // NOT nice! but it works...
        this->show();
        this->hide();
        this->show();
    }

}

/*
Slot: Updates the popup max size parameter, also hides the popup
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

// ################################################################## //

/*
Constructor: Constructs a Completer for Fluor::LineEdit object.
    :param parent: pointer to parent widget
*/
Completer::Completer(QWidget* parent) :
    QCompleter(parent),
    widget_popup(nullptr),
    mouse_pressed(false),
    select_active_row(true),
    popup_max_size(),
    default_items({QString{"No Data Loaded"}}),
    //default_items({QString{"PE"}, QString{"APC"}, QString{"FITC"}, QString{"BUV395"}, QString{"BUV560"}, QString{"BUV737"}, QString{"BV410"}, QString{"BV650"}, QString{"BV735"}, QString{"BV785"}}),
    completion("")
{
    this->setWidget(parent);
    this->setCaseSensitivity(Qt::CaseInsensitive);
    this->setCompletionMode(QCompleter::PopupCompletion); // Normal popup is blocked and replaced
    this->setMaxVisibleItems(50);
    this->setWrapAround(true);

    // Build & set model (also setups the popup)
    this->buildModel(this->default_items);

    // Need to use old-style connect to connect during runtime to the Q_PRIVATE_SLOT(_q_complete)
    QObject::connect(this, SIGNAL(q_complete(QModelIndex)), static_cast<QCompleter*>(this), SLOT(_q_complete(QModelIndex)));
}

/*
Builds and set the model based upon the input vector
    :param items: the items names to build the model from
*/
void Completer::buildModel(const std::vector<QString>& items){
    // Build new model (and make the QCompleter it's parent. Necessary for lifetime management)
    QStandardItemModel* model_standarditem = new QStandardItemModel{this};
    for(std::size_t i=0; i<items.size(); ++i){
        QStandardItem *item = new QStandardItem(items[i]);
        model_standarditem->appendRow(item);
    }
    this->setModel(model_standarditem);
    this->popup()->hide();
}

/*
Returns the current completion. Use updateCompleter() to update the completion
*/
const QString& Completer::getCompletion() const{
    return(this->completion);
}

/*
Sets popup. Takes ownership of the popup (dont share popup with other widgets)
    :param popup: popup widget
*/
void Completer::setPopup(Fluor::Popup* popup){
    // Remove previous popup
    if(this->widget_popup){
        QObject::disconnect(this->popup(), &Fluor::Popup::activated, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupActivated);
        QObject::disconnect(this->popup(), &Fluor::Popup::highlighted, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupHighlighted);
        QObject::disconnect(this->popup(), &Fluor::Popup::dblClicked, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupDblClicked);
        delete this->widget_popup;
    }
    
    this->widget_popup = popup;
    if(this->completionModel()){
        popup->setModel(this->completionModel());
    }else{
        qWarning() << "Fluor::Completer::setPopup(): No model to give to popup.";
    }

    QObject::connect(this->popup(), &Fluor::Popup::activated, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupActivated);
    QObject::connect(this->popup(), &Fluor::Popup::highlighted, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupHighlighted);
    QObject::connect(this->popup(), &Fluor::Popup::dblClicked, static_cast<Fluor::LineEdit*>(this->parent()), &Fluor::LineEdit::updatePopupDblClicked);
}

/*
Gets popup for Completer. If no completer has been set, will construct one before returning it
    :returns: popup widget
*/
Fluor::Popup* Completer::popup(){
    if(this->widget_popup){
        return this->widget_popup;
    }

    // No popup has been set, so build one:
    Fluor::Popup* popup_new = new Fluor::Popup(static_cast<QWidget*>(this->parent()));
    this->setPopup(popup_new);

    return this->widget_popup;
}

/*
Shows the popup (if not visible)
*/
void Completer::showPopup(){
    if(!this->popup()->isVisible()){
        this->popup()->showPopup();
    }
}

/*
Hides the popup (if visible)
*/
void Completer::hidePopup(){
    if(this->popup()->isVisible()){
        this->popup()->hide();
    }
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
            emit this->q_complete(index);
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
    static_cast<Fluor::Popup*>(this->popup())->updateRect(widget);
}

} // Fluor namespace
