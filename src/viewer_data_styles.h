/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_data_style.h is part of Spectral Viewer
**        
** Spectral Viewer is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** Spectral Viewer is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License along
** with Spectral Viewer. If not, see <https://www.gnu.org/licenses/>.  
***************************************************************************/

/**** DOC ******************************************************************
** The stylesheet builder
** 
** :class: StyleBuilder
** Requests the style QSettings from the DataFactory and uses that to build a stylesheet
** 
***************************************************************************/

#ifndef VIEWER_DATA_STYLE_H
#define VIEWER_DATA_STYLE_H

#include "viewer_data_factory.h"
#include <QString>

class StyleBuilder {
    public:
        StyleBuilder();
        QString getStyleSheet() const;
        std::vector<QString> getStyleID(DataFactory& data) const;
        void loadStyle(DataFactory& data, const QString& style_id);

    private:
        QString stylesheet;

        QString icons;

        QString background;
        QString pushbutton_background;
        QString pushbutton_text;
        QString pushbutton_text_weight;
        QString pushbutton_border;
        QString pushbutton_border_width;
        QString pushbutton_hover_background;
        QString pushbutton_hover_text;
        QString pushbutton_hover_border;
        QString pushbutton_press_background;
        QString pushbutton_press_text;
        QString pushbutton_press_border;
        QString pushbutton_inactive_background;
        QString pushbutton_inactive_text;
        QString pushbutton_inactive_border;
        QString pushbutton_inactive_hover_background;
        QString pushbutton_inactive_hover_text;
        QString pushbutton_inactive_hover_border;
        QString pushbutton_inactive_press_background;
        QString pushbutton_inactive_press_text;
        QString pushbutton_inactive_press_border;

        QString lineedit_background;
        QString lineedit_text;
        QString lineedit_text_weight;
        QString lineedit_selection;
        QString lineedit_selection_text;
        QString lineedit_border;
        QString lineedit_border_width;
        QString lineedit_hover;
        QString lineedit_hover_text;
        QString lineedit_hover_border;
        QString lineedit_popup;
        QString lineedit_popup_text;
        QString lineedit_popup_border;
        QString lineedit_popup_hover;
        QString lineedit_popup_hover_text;
        QString lineedit_popup_hover_border;

        QString listview_background;
        QString listview_text;
        QString listview_text_weight;
        QString listview_border;
        QString listview_border_width;
        QString listview_item;
        QString listview_item_text;
        QString listview_item_selection;
        QString listview_item_selection_text;
        QString listview_item_hover;
        QString listview_item_hover_text;
        QString listview_item_disable;
        QString listview_item_disable_text;

        QString tabwidget_background;
        QString tabwidget_unselected;
        QString tabwidget_unselected_hover;

        QString tabwidget_text;
        QString tabwidget_text_weight;
        QString tabwidget_border;
        QString tabwidget_border_width;

        QString scrollbar_border_width;
        QString scrollbar_border;
        QString scrollbar_handle;
        QString scrollbar_background;

        QString lasermenu_popup;

        QString fluormenu_remove;
        QString fluormenu_remove_hover;
        QString fluormenu_remove_press;
        QString fluormenu_popup;
        QString fluormenu_background;

        QString graph_background;
        QString graph_text;
        QString graph_text_weight;
        QString graph_plot;
        QString graph_plot_focus;
        QString graph_axis;
        QString graph_grid;
        QString graph_font;
        QString graph_detector;

        void buildStylesheet();
        QString buildPushButton() const;
        QString buildLineEdit() const;
        QString buildListView() const;
        QString buildTabWidget() const;
        QString buildScrollBar() const;
        QString buildCentralWindow() const;
        QString buildToolBar() const;
        QString buildLaserMenu() const;
        QString buildFluorMenu() const;
        QString buildGraph() const;
};

#endif // VIEWER_DATA_STYLE_H