/**** General **************************************************************
** Version:    v0.9.2
** Date:       2019-04-07
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** laser_lineedit.h is part of Fluor
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
** The laser menu lineedit
**
** :class: Laser::Popup
** The nonmodal laser popup
**
** :class: Laser::LineEdit
** The laser lineedit
**
***************************************************************************/

#ifndef LASER_LINEEDIT_H
#define LASER_LINEEDIT_H

#include <QLineEdit>
#include <QListView>
#include <QEvent>
#include "data_instruments.h"

namespace Laser {

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

        bool updateKeyUp();
        bool updateKeyDown();

    private:
        int margin_scrollbar;
        const int max_visible_items;
        QRect max_size;

        void buildModel();
        void buildModel(const std::vector<int>& wavelengths);

        bool eventFilter(QObject* obj, QEvent* event);

    signals:
        void dblClicked(const QModelIndex& index);
        void output(int wavelength);
        void highlighted(const QModelIndex& index);

    private slots:
        void hidingScrollBar();
        void showingScrollBar();

    public slots:
        void showPopup();
        void buildOutput(const QModelIndex& index);
        void updateRect(const QWidget* widget);
        void reloadModel(const Data::Instrument& instrument);

};

class LineEdit : public QLineEdit {
    Q_OBJECT

    public:
        explicit LineEdit(QWidget* parent=nullptr);
        LineEdit(const LineEdit &obj) = delete;
        LineEdit& operator=(const LineEdit &obj) = delete;
        LineEdit(LineEdit&&) = delete;
        LineEdit& operator=(LineEdit&&) = delete;
        ~LineEdit() = default;

        void toggleStylePopup();
        void toggleStylePopup(bool popup);

    private:
        Laser::Popup* widget_popup;
        const QString text_before;
        const QString text_after;
        const int text_write_start;
        const int text_write_length;                // MUST be > 0, if not Undefined behavior will happen
        const int text_write_end;

        void setPopup(Laser::Popup* popup);
        Laser::Popup* popup();
        void showPopup();
        void hidePopup();
        void clearFocus();

        void buildOutput();

        bool eventFilter(QObject *obj, QEvent *event) override;

    signals:
        void output(int value);                     // emits output int
        void finished();                            // emits upon finishing an output
    
    public slots:
        void buildText(QString value);
        void buildText(int value);
        void showButton();
        void hideButton();
        void unfocus(QEvent* event);
        void reloadSize(const QWidget* widget=nullptr);
        void reloadModel(const Data::Instrument& instrument);

    private slots:
        void reset();
        void updatePopupActivated(const QModelIndex& index);
        void updatePopupDblClicked(const QModelIndex& index);
};

} // Laser namespace

#endif // LASER_LINEEDIT_H