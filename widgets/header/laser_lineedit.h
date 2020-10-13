/**** General **************************************************************
** Version:    v0.9.10
** Date:       2020-10-13
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
** :int: SeparatorRole
** Qt::UserRole for separator indicators in the model items (in laser's QListViews)
**
** :int: LaserLineRole
** Qt::LaserLineRole for the laserline pointer value (cast to void*)
**
** :int: SeparatorRole
** Qt::UserRole for the wavelength value in the model items (in laser's QListViews)
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
#include "state_gui.h"

namespace Laser {

class AbstractPopup : public QListView {
    Q_OBJECT
    Q_PROPERTY(QString viewport_margins_scroll_bar READ viewportMarginsScrollBar WRITE setViewportMarginsScrollBar)

    public:
        explicit AbstractPopup(QWidget* parent=nullptr);
        AbstractPopup(const AbstractPopup &obj) = delete;
        AbstractPopup& operator=(const AbstractPopup &obj) = delete;
        AbstractPopup(AbstractPopup&&) = delete;
        AbstractPopup& operator=(AbstractPopup&&) = delete;
        virtual ~AbstractPopup() = default;

    public:
        QString viewportMarginsScrollBar() const;
        void setViewportMarginsScrollBar(QString layout_spacing_scroll_bar);

        virtual bool updateKeyUp();
        virtual bool updateKeyDown();

        virtual std::vector<Data::LaserID> active_items() const;

    private:
        int margin_scrollbar;
        QRect max_size;

    protected:
        bool eventFilter(QObject* obj, QEvent* event);
        virtual void buildModel() = 0;
        virtual void buildModel(const Data::Instrument& instrument) = 0;

    signals:
        void dblClicked(const QModelIndex& index);
        void highlighted(const QModelIndex& index);

    private slots:
        void hidingScrollBar();
        void showingScrollBar();

    public slots:
        void showPopup();
        virtual void wavelengthEdited();
        void updateRect(const QWidget* widget);
        void reloadModel(const Data::Instrument& instrument);
        virtual void updateState(std::vector<::State::GraphState>& state);
};

class LaserLinePopup : public AbstractPopup {
    Q_OBJECT

    public:
        explicit LaserLinePopup(QWidget* parent=nullptr);
        LaserLinePopup(const LaserLinePopup &obj) = delete;
        LaserLinePopup& operator=(const LaserLinePopup &obj) = delete;
        LaserLinePopup(LaserLinePopup&&) = delete;
        LaserLinePopup& operator=(LaserLinePopup&&) = delete;
        ~LaserLinePopup() = default;

    private:
        bool ignore_next_mouse_release = false;

    protected:
        bool eventFilter(QObject* obj, QEvent* event);
        void buildModel();
        void buildModel(const Data::Instrument& instrument);

    public:
        bool updateKeyUp() override;
        bool updateKeyDown() override;

        virtual std::vector<Data::LaserID> active_items() const override;

    public slots:
        virtual void wavelengthEdited();
        virtual void updateState(std::vector<::State::GraphState>& state);
};

class Popup : public AbstractPopup {
    Q_OBJECT

    public:
        explicit Popup(QWidget* parent=nullptr);
        Popup(const Popup &obj) = delete;
        Popup& operator=(const Popup &obj) = delete;
        Popup(Popup&&) = delete;
        Popup& operator=(Popup&&) = delete;
        ~Popup() = default;

    protected:
        void buildModel();
        void buildModel(const Data::Instrument& instrument);
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
        Laser::AbstractPopup* widget_popup;
        const QString text_before;
        const QString text_after;
        const int text_write_start;
        const int text_write_length;                // MUST be > 0, if not Undefined behavior will happen
        const int text_write_end;

        void setPopup(Laser::AbstractPopup* popup);
        Laser::AbstractPopup* popup();
        void showPopup();
        void hidePopup();
        void clearFocus();

        void buildOutput();

        bool eventFilter(QObject *obj, QEvent *event) override;

    signals:
        void wavelengthEdited();            // emits whenever the text is edited
        void output(std::vector<Data::LaserID>& lasers);  // emits output lasers
        void finished();                                  // emits upon finishing an output
    
    public slots:
        void buildText(QString value);
        void buildText(int value);
        void showButton();
        void hideButton();
        void unfocus(QEvent* event);
        void reloadSize(const QWidget* widget=nullptr);
        void reloadModel(const Data::Instrument& instrument);
        void receiveGraphState(std::vector<State::GraphState>& state);

    signals:
        void updatePopupGraphState(std::vector<State::GraphState>& state);

    private slots:
        void reset();
        void updatePopupActivated(const QModelIndex& index);
        void updatePopupDblClicked(const QModelIndex& index);
};

} // Laser namespace

#endif // LASER_LINEEDIT_H