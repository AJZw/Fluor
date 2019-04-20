/**** General **************************************************************
** Version:    v0.9.1
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
** :class: LineEdit
** The fluorophore menu lineedit
**
** :class: Completer
** The fluorophore completer
**
***************************************************************************/

#ifndef FLUOR_LINEEDIT_H
#define FLUOR_LINEEDIT_H

#include <QLineEdit>
#include <QCompleter>
#include <QStringList>
#include <QEvent>
#include <QListView>
#include "data_fluorophores.h"

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

        void toggleStylePopup();
        void toggleStylePopup(bool popup);

    private:
        QCompleter* _completer;

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
        void updatePopupHighlighted(const QString &text);
        void updatePopupActivated(const QString &text);
        void updateTab();
        void updateBackspace();
        void updateDelete();
        void updateLeft();
        void updateRight();
        void updateHome();
        void updateEnd();
        bool eventFilter(QObject *obj, QEvent *event) override;

    signals:
        void highlightPopup(const QString entry);   // emits upon selecting a completion
        void output(QStringList outputs);           // emits output QStringList
        void finished();                            // emits upon finishing an output
        //void requestDataFluorophores();
        //void fluorophores(const QStringList fluorophores);
    
    public slots:
        void showButton();
        void hideButton();
        void unfocus(QEvent* event);
        void reloadCompleterPopupSize(const QWidget* widget=nullptr);
        void reloadCompleterModel(const DataFluorophores* data=nullptr);

    private slots:
        void reset();
        void updateTextEdited(const QString& text); // slot that wraps textEdited()
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

        const QString& getCompletion() const;

    private:
        bool mouse_pressed;
        bool select_active_row;
        QRect popup_max_size;
        QStringList model_list;
        QString completion;

        void showPopup();
        bool eventFilter(QObject *obj, QEvent *event) override;
    
    signals:
        void _q_complete(QModelIndex index);        // to (dynamically) connect to private _q_complete slot
        void completed(QString completion);         // fired after completion has been determined
        void popupVisible(bool visible);              // fires upon popup visibility changes

    public slots:
        void complete(const QRect& rect=QRect());
        void updateCompleter(const QString prefix, const QStringList& disabled);
        void updatePopupRect(const QWidget* widget=nullptr);
        void updateHighlight(const QString& entry);
};

} // Fluor namespace

#endif // FLUOR_LINEEDIT_H