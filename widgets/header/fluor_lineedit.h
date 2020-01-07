/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-02-03
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** fluor_lineedit.h is part of Fluor
**        
** Fluor is free software: you can redistribute it and/or
** modify it under the terms of the Lesser GNU General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
** 
** Fluor is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Lesser
** GNU General Public License for more details.
** 
** You should have received a copy of the Lesser GNU General Public License
** along with Fluor. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

/**** DOC ******************************************************************
** The fluorophore menu lineedit
**
** :class: Fluor::LineEdit
** The fluorophore menu lineedit. Completes the input text, and allows for 
** multiple comma separated inputs
**
** :class: Fluor::Popup
** The popup of the completer, presents a list of possible completion solutions
**
** :class: Fluor::Completer
** Completes the inputs of the LineEdit by comparison to fluorophore data
**
***************************************************************************/

#ifndef FLUOR_LINEEDIT_H
#define FLUOR_LINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringList>
#include <QEvent>
#include <QListView>
#include <vector>
#include "data_fluorophores.h"
#include "cache.h"

namespace Fluor {

class LineEdit : public QLineEdit {
    Q_OBJECT

    public:
        explicit LineEdit(QWidget* parent=nullptr);
        LineEdit(const LineEdit &obj) = delete;
        LineEdit& operator=(const LineEdit &obj) = delete;
        LineEdit(LineEdit&&) = delete;
        LineEdit& operator=(LineEdit&&) = delete;
        ~LineEdit() = default;

        void setCompleter(QCompleter* completer);
        QCompleter* completer();
        void clearFocus();

        void toggleStylePopup();
        void toggleStylePopup(bool popup);

    private:
        QCompleter* _completer;
        std::unordered_map<QString, QString> lookup_id;
        std::unordered_map<QString, QStringList> lookup_names;
        std::vector<QString> incache_names;                         // Stores fluorophore names already in cache (need to be disabled in the completer)

        const bool inline_selection;
        int cursor_pos;                 // Cursor position
        QStringList entries_before;     // Text entries before active entry
        QStringList entries_after;      // Text entries after active entry
        QString prefix_text;            // Active entry, pre-cursor text
        int prefix_length;              // Active entry, pre-cursor text length
        QString postfix_text;           // Active entry, post-cursor text
        int postfix_length;             // Active entry, post-cursor text length

        void complete();
        void buildCompletion();
        void buildSelection();
        void buildText(QString completion=QString{""});
        QString getCompletion();
        void buildOutput();

        void updateTextParameters(const QString& text, const int cursor);
        void updateTab();
        void updateBackspace();
        void updateDelete();
        void updateLeft();
        void updateRight();
        void updateHome();
        void updateEnd();
        bool eventFilter(QObject *obj, QEvent *event) override;

    signals:
        void highlightPopup(const QString entry);               // emits upon selecting a completion
        void output(std::vector<Data::FluorophoreID>& output);     // emits output set
        void finished();                                        // emits upon finishing an output
    
    public slots:
        void showButton();
        void hideButton();
        void unfocus(QEvent* event);

        void reloadSize(const QWidget* widget=nullptr);
        void reloadData(const Data::FluorophoreReader& data);
        void sync(const std::vector<Cache::CacheID>& input);

        void updatePopupHighlighted(const QString& text);
        void updatePopupActivated(const QString& text);
        void updatePopupDblClicked(const QString& text);

    private slots:
        void reset();
        void updateTextEdited(const QString& text); // slot that wraps textEdited()
};

class Popup : public QListView {
    Q_OBJECT
    Q_PROPERTY(QString viewport_margins_scroll_bar READ viewportMarginsScrollBar WRITE setViewportMarginsScrollBar)

    public:
        explicit Popup(QWidget* parent=nullptr);
        Popup(const Popup &obj) = delete;
        Popup& operator=(const Popup &obj) = delete;
        Popup(Popup&&) = delete;
        Popup& operator=(Popup&&) = delete;
        ~Popup() = default;

        QString viewportMarginsScrollBar() const;
        void setViewportMarginsScrollBar(QString layout_spacing_scroll_bar);

        void setModel(QAbstractItemModel* model);
        bool updateKeyUp();
        bool updateKeyDown();

    private:
        int margin_scrollbar;
        const int max_visible_items;
        QRect max_size;

        bool eventFilter(QObject* obj, QEvent* event);

    private slots:
        void hidingScrollBar();
        void showingScrollBar();
        void highlight(const QModelIndex& index);
        void activate(const QModelIndex& index);

    public slots:
        void showPopup();
        void updateRect(const QWidget* widget);

    signals:
        void highlighted(const QString& entree);
        void activated(const QString& entree);
        void dblClicked(const QString& entree);
};

class Completer : public QCompleter {
    Q_OBJECT

    public:
        explicit Completer(QWidget* parent=nullptr);
        Completer(const Completer &obj) = delete;
        Completer& operator=(const Completer &obj) = delete;
        Completer(Completer&&) = delete;
        Completer& operator=(Completer&&) = delete;
        ~Completer() = default;

        void buildModel(const std::vector<QString>& items);

        const QString& getCompletion() const;

        Fluor::Popup* popup();
        void setPopup(Fluor::Popup* popup);
        void showPopup();
        void hidePopup();

    private:
        Fluor::Popup* widget_popup;
        bool mouse_pressed;
        bool select_active_row;
        QRect popup_max_size;
        const std::vector<QString> default_items;
        QString completion;
    
    signals:
        void q_complete(QModelIndex index);        // to (dynamically) connect to private _q_complete slot
        void completed(QString completion);         // fired after completion has been determined
        void popupVisible(bool visible);            // fires upon popup visibility changes

    public slots:
        void complete(const QRect& rect=QRect());
        void updateCompleter(const QString prefix, const QStringList& disabled);
        void updatePopupRect(const QWidget* widget=nullptr);
};

} // Fluor namespace

#endif // FLUOR_LINEEDIT_H