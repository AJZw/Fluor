/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-10-28
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** main_menu_bar.h is part of Fluor
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
** The program wide dropdown menu's
**
** :class: 
**
** 
***************************************************************************/

#ifndef MAIN_MENU_BAR_H
#define MAIN_MENU_BAR_H

#include "data_instruments.h"
#include "data_styles.h"
#include "global.h"
#include <QMenuBar>
#include <QMenu>
#include <QActionGroup>

namespace Main {

class FileMenu : public QMenu {
    Q_OBJECT

    public:
        explicit FileMenu(QWidget *parent = nullptr);
        FileMenu(const FileMenu &obj) = delete;
        FileMenu& operator=(const FileMenu &obj) = delete;
        FileMenu(FileMenu&&) = delete;
        FileMenu& operator=(FileMenu&&) = delete;
        virtual ~FileMenu() = default;

    private slots:
        void triggered_saveas(bool checked);
        void triggered_open(bool checked);
        void triggered_print(bool checked);
        void triggered_exit(bool checked);

    signals:
        void sendAction(Main::MenuBarAction action, const QVariant& id=QVariant());
};

class InstrumentMenu : public QMenu {
    Q_OBJECT

    public:
        explicit InstrumentMenu(QWidget *parent = nullptr);
        InstrumentMenu(const InstrumentMenu &obj) = delete;
        InstrumentMenu& operator=(const InstrumentMenu &obj) = delete;
        InstrumentMenu(InstrumentMenu&&) = delete;
        InstrumentMenu& operator=(InstrumentMenu&&) = delete;
        virtual ~InstrumentMenu() = default;
    
    private:
        void selectInstrument(QString id);
        void buildMenu(const Data::InstrumentReader& instruments);

    private slots:
        void triggered_instrument(QAction* action);

    public slots:
        void receiveInstrument(QString id);
        void receiveInstruments(const Data::InstrumentReader& instruments);

    signals:
        void sendAction(Main::MenuBarAction action, const QVariant& id=QVariant());
};

class OptionsMenu : public QMenu {
    Q_OBJECT

    public:
        explicit OptionsMenu(QWidget *parent = nullptr);
        OptionsMenu(const OptionsMenu &obj) = delete;
        OptionsMenu& operator=(const OptionsMenu &obj) = delete;
        OptionsMenu(OptionsMenu&&) = delete;
        OptionsMenu& operator=(OptionsMenu&&) = delete;
        virtual ~OptionsMenu() = default;

    private:
        QMenu* menu_sorting;
        QMenu* menu_styles;

        void selectStyle(const QString& id);
        void buildStyles(const std::vector<Data::StyleID>& styles);
        void selectSortMode(State::SortMode mode);

    private slots:
        void triggered_styles(QAction* action);
        void triggered_sorting(QAction* action);

    public slots:
        void receiveStyle(const QString& id);
        void receiveStyles(const std::vector<Data::StyleID>& styles);
        void receiveSortMode(State::SortMode mode);

    signals:
        void sendAction(Main::MenuBarAction action, const QVariant& id=QVariant());

};

class HelpMenu : public QMenu {
    Q_OBJECT

    public:
        explicit HelpMenu(QWidget *parent = nullptr);
        HelpMenu(const HelpMenu &obj) = delete;
        HelpMenu& operator=(const HelpMenu &obj) = delete;
        HelpMenu(HelpMenu&&) = delete;
        HelpMenu& operator=(HelpMenu&&) = delete;
        virtual ~HelpMenu() = default;

    protected slots:
        void triggered_about(bool checked);

    signals:
        void sendAction(Main::MenuBarAction action, const QVariant& id=QVariant());
};

class MenuBar : public QMenuBar {
    Q_OBJECT

    public:
        explicit MenuBar(QWidget *parent = nullptr);
        MenuBar(const MenuBar &obj) = delete;
        MenuBar& operator=(const MenuBar &obj) = delete;
        MenuBar(MenuBar&&) = delete;
        MenuBar& operator=(MenuBar&&) = delete;
        virtual ~MenuBar() = default;

    private:
        FileMenu* file_menu;
        InstrumentMenu* instrument_menu;
        OptionsMenu* options_menu;
        HelpMenu* help_menu;

    public slots:
        void receiveMenuBarStateUpdate(Main::MenuBarAction action, const QVariant& id=QVariant());
        void receiveMenuBarStateChange(Main::MenuBarAction action, const QVariant& id=QVariant());

        void receiveInstruments(const Data::InstrumentReader& instruments);
        void receiveStyles(const std::vector<Data::StyleID>& styles);

    signals:
        void sendMenuBarStateChange(Main::MenuBarAction action, QVariant id=QVariant());

        void sendInstruments(const Data::InstrumentReader& instruments);
        void sendStyles(const std::vector<Data::StyleID>& styles);

        void sendInstrument(const QString& id);
        void sendStyle(const QString& id);
        void sendSortMode(State::SortMode mode);
};

} // Main namespace

#endif // MAIN_MENU_BAR_H
