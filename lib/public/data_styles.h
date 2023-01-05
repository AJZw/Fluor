/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** LGPLv3 License *******************************************************
** data_style.h is part of Fluor
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
** The stylesheet builder
**
** :class: StyleBuilder
** Requests the style QSettings from the DataFactory and uses that to build a stylesheet
**
***************************************************************************/

#ifndef DATA_STYLE_H
#define DATA_STYLE_H

#include <QFontMetrics>
#include <QString>
#include <QtCore>

#include "data_factory.h"
#include "data_global.h"

namespace Data {

struct DATALIB_EXPORT StyleID {
  StyleID(QString id, QString name) : id(id), name(name){};
  StyleID(const StyleID&) = default;
  StyleID& operator=(const StyleID&) = default;
  StyleID(StyleID&&) = default;
  StyleID& operator=(StyleID&&) = default;
  ~StyleID() = default;

  friend QDebug operator<<(QDebug stream, const StyleID& object) { return stream << "{" << object.id << ":" << object.name << "}"; };

  const QString id;
  QString name;
};

class DATALIB_EXPORT StyleBuilder {
 public:
  StyleBuilder();
  StyleBuilder(const StyleBuilder&) = default;
  StyleBuilder& operator=(const StyleBuilder&) = default;
  StyleBuilder(StyleBuilder&&) = default;
  StyleBuilder& operator=(StyleBuilder&&) = default;
  ~StyleBuilder() = default;

  QString id() const;
  QString getStyleSheet() const;
  std::vector<StyleID> getStyleIDs(const Data::Factory& data) const;

  void loadStyle(const Data::Factory& data, const QString& style_id);
  void buildStyleSheet();
  void buildStyleSheet(const QFontMetrics& metrics);

 private:
  QString style_id;

  QString stylesheet;

  QString icons;

  QString layout_margins;
  QString layout_spacing;
  QString layout_sub_spacing;

  QString background;

  QString label_text;
  QString label_text_weight;
  QString label_disabled_text;

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
  QString pushbutton_disabled_background;
  QString pushbutton_disabled_text;
  QString pushbutton_disabled_border;

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

  QString graph_scene;
  QString graph_background;
  QString graph_background_hover;
  QString graph_background_press;
  QString graph_axis;
  QString graph_axis_hover;
  QString graph_axis_press;
  QString graph_label;
  QString graph_label_weight;
  QString graph_grid;
  QString graph_grid_label;
  QString graph_grid_label_weight;
  QString graph_filter;
  QString graph_filter_width;
  QString graph_absorption_width;
  QString graph_absorption_style;
  QString graph_excitation_width;
  QString graph_excitation_style;
  QString graph_emission_width;
  QString graph_emission_style;

  QString buildLayout(const QFontMetrics& metrics) const;
  QString buildLabel(const QFontMetrics& metrics) const;
  QString buildPushButton() const;
  QString buildLineEdit() const;
  QString buildListView() const;
  QString buildScrollBar() const;
  QString buildCentralWindow() const;
  QString buildToolBar(const QFontMetrics& metrics) const;
  QString buildLaserMenu(const QFontMetrics& metrics) const;
  QString buildFluorMenu(const QFontMetrics& metrics) const;
  QString buildGraph(const QFontMetrics& metrics) const;
  QString buildAboutWindow(const QFontMetrics& metrics) const;

  static QString toPixels(const QFontMetrics& metrics, const QString& text, QString fallback = "0");
};

}  // namespace Data

#endif  // DATA_STYLE_H