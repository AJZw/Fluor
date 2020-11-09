/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_styles.h"

#include <QFont>
#include <QFontMetrics>
#include <QApplication>
#include <QDebug>
#include <QFont>

namespace Data {

/* 
(Constructor) Builds a CSS stylesheet (from QSettings)
*/
StyleBuilder::StyleBuilder() :
    style_id(QString()),

    stylesheet(""),
    
    icons("light"),

    layout_margins("6px"),
    layout_spacing("6px"),
    layout_sub_spacing("3px"),

    background("#FFFFFF"),

    label_background("#FFFFFF"),
    label_text("#000000"),
    label_text_weight("bold"),
    label_disabled_background(this->label_background),
    label_disabled_text(this->label_text),

    pushbutton_background("#FFFFFF"),
    pushbutton_text("#000000"),
    pushbutton_text_weight("bold"),
    pushbutton_border("#000000"),
    pushbutton_border_width("1px"),
    pushbutton_hover_background(this->pushbutton_background),
    pushbutton_hover_text(this->pushbutton_text),
    pushbutton_hover_border(this->pushbutton_border),
    pushbutton_press_background(this->pushbutton_background),
    pushbutton_press_text(this->pushbutton_hover_text),
    pushbutton_press_border(this->pushbutton_border),
    pushbutton_inactive_background("#CCCCCC"),
    pushbutton_inactive_text("#A0A0A0"),
    pushbutton_inactive_border(this->pushbutton_border),
    pushbutton_inactive_hover_background(this->pushbutton_inactive_background),
    pushbutton_inactive_hover_text(this->pushbutton_inactive_text),
    pushbutton_inactive_hover_border(this->pushbutton_inactive_border),
    pushbutton_inactive_press_background(this->pushbutton_inactive_hover_background),
    pushbutton_inactive_press_text(this->pushbutton_inactive_hover_text),
    pushbutton_inactive_press_border(this->pushbutton_inactive_hover_border),
    pushbutton_disabled_background(this->pushbutton_inactive_background),
    pushbutton_disabled_text(this->pushbutton_inactive_text),
    pushbutton_disabled_border(this->pushbutton_inactive_border),

    lineedit_background("#FFFFFF"),
    lineedit_text("#000000"),
    lineedit_text_weight("bold"),
    lineedit_selection("#808080"),
    lineedit_selection_text(this->lineedit_text),
    lineedit_border("#000000"),
    lineedit_border_width("1px"),
    lineedit_hover(this->lineedit_background),
    lineedit_hover_text(this->lineedit_text),
    lineedit_hover_border(this->lineedit_border),
    lineedit_popup(this->lineedit_background),
    lineedit_popup_text(this->lineedit_text),
    lineedit_popup_border(this->lineedit_border),
    lineedit_popup_hover(this->lineedit_popup),
    lineedit_popup_hover_text(this->lineedit_popup_text),
    lineedit_popup_hover_border(this->lineedit_popup_border),

    listview_background("#FFFFFF"),
    listview_text("#000000"),
    listview_text_weight("bold"),
    listview_border(this->lineedit_border),
    listview_border_width("1px"),
    listview_item(this->listview_background),
    listview_item_text(this->listview_text),
    listview_item_selection("#808080"),
    listview_item_selection_text(this->listview_text),
    listview_item_hover(this->listview_item_selection),
    listview_item_hover_text(this->listview_item_selection_text),
    listview_item_disable("#000000"),
    listview_item_disable_text("#A0A0A0"),

    tabwidget_background("#FFFFFF"),
    tabwidget_unselected("#CCCCCC"),
    tabwidget_unselected_hover(this->tabwidget_unselected),
    tabwidget_text("#000000"),
    tabwidget_text_weight("bold"),
    tabwidget_border("#000000"),
    tabwidget_border_width("1px"),

    scrollbar_border_width("1px"),
    scrollbar_border("#000000"),
    scrollbar_handle("#FFFFFF"),
    scrollbar_background("#000000"),

    lasermenu_popup(this->lineedit_background),

    fluormenu_remove("#FFFFFF"),
    fluormenu_remove_hover(this->fluormenu_remove),
    fluormenu_remove_press(this->fluormenu_remove_hover),
    fluormenu_popup(this->listview_background),
    fluormenu_background("#FFFFFF"),

    graph_scene("#FFFFFF"),
    graph_background(this->graph_scene),
    graph_background_hover(this->graph_scene),
    graph_background_press(this->graph_scene),
    graph_label("#000000"),
    graph_label_weight("bold"),
    graph_grid_label("#000000"),
    graph_grid_label_weight("bold"),
    graph_axis("#000000"),
    graph_axis_hover("#CCCCCC"),
    graph_axis_press("#CCCCCC"),
    graph_absorption_width("2px"),
    graph_absorption_style("dot"),
    graph_excitation_width("2px"),
    graph_excitation_style("dash"),
    graph_emission_width("2px"),
    graph_emission_style("solid")

{
    this->buildStyleSheet();
}

/*
Get the currently loaded style identifier. If no style is loaded, returns an empty QString
*/
QString StyleBuilder::id() const{
    return this->style_id;
}

/*
Getter: returns the QSS stylesheet
    :returns: stylesheet
*/
QString StyleBuilder::getStyleSheet() const {
    return(this->stylesheet);
}

/*
Getter: returns all style names from the settings file
    :param data: a data factory
    :returns: vector of style_id(s)
*/
std::vector<StyleID> StyleBuilder::getStyleIDs(const Data::Factory& data) const {
    std::unique_ptr<QSettings> styles = data.get(Data::Factory::Styles);

    std::vector<StyleID> style_ids{};
    
    QStringList style_groups = styles->childGroups();
    for(const auto& style_id : style_groups){
        styles->beginGroup(style_id);
        QString style_name = styles->value("name", style_id).toString();
        style_ids.push_back(StyleID(style_id, style_name));
        styles->endGroup();
    }
    
    return style_ids;
}

/*
Loads attributes from settings file
    :param data: a data factory
    :param style_id: the id of the style to load
*/
void StyleBuilder::loadStyle(const Data::Factory& data, const QString& style_id){
    std::unique_ptr<QSettings> style = data.get(Data::Factory::Styles);

    QStringList style_groups;
    style_groups = style->childGroups();
    if(!style_groups.contains(style_id)){
        qWarning() << "StyleBuilder::loadStyle: " << style_id << " group is not found within the styles data object";
        this->style_id = QString();
        return;
    }else{
        this->style_id = style_id;
        style->beginGroup(style_id);
    };

    this->icons = style->value("icons", "light").toString();

    this->layout_margins = style->value("layout_margins", "6px").toString();
    this->layout_spacing = style->value("layout_spacing", "6px").toString();
    this->layout_sub_spacing = style->value("layout_sub_spacing", "3px").toString();

    this->background = style->value("main_background", "#FFFFFF").toString();

    this->label_background = style->value("main_background", "#FFFFFF").toString();
    this->label_text = style->value("text_color", "#000000").toString();
    this->label_text_weight = style->value("text_weight", "bold").toString();
    this->label_disabled_background = style->value("main_background", this->label_background).toString();
    this->label_disabled_text = style->value("text_color_disabled", this->label_text).toString();

    this->pushbutton_background = style->value("widget", "#FFFFFF").toString();
    this->pushbutton_text = style->value("text_color", "#000000").toString();
    this->pushbutton_text_weight = style->value("text_weight", "bold").toString();
    this->pushbutton_border = style->value("border_color", "#000000").toString();
    this->pushbutton_border_width = style->value("border_width", "1px").toString();
    this->pushbutton_hover_background = style->value("widget_hover", this->pushbutton_background).toString();
    this->pushbutton_hover_text = style->value("text_color", this->pushbutton_text).toString();
    this->pushbutton_hover_border = style->value("border_color_hover", this->pushbutton_border).toString();
    this->pushbutton_press_background = style->value("widget_press", this->pushbutton_hover_background).toString();
    this->pushbutton_press_text = style->value("text_color", this->pushbutton_hover_text).toString();
    this->pushbutton_press_border = style->value("border_color_press", this->pushbutton_hover_border).toString();
    this->pushbutton_inactive_background = style->value("widget_inactive", "#CCCCCC").toString();
    this->pushbutton_inactive_text = style->value("text_color", "A0A0A0").toString();
    this->pushbutton_inactive_border = style->value("border_color", this->pushbutton_border).toString();
    this->pushbutton_inactive_hover_background = style->value("widget_inactive_hover", this->pushbutton_inactive_background).toString();
    this->pushbutton_inactive_hover_text = style->value("text_color", this->pushbutton_inactive_text).toString();
    this->pushbutton_inactive_hover_border = style->value("border_color_hover", this->pushbutton_inactive_border).toString();
    this->pushbutton_inactive_press_background = style->value("widget_inactive_press", this->pushbutton_inactive_hover_background).toString();
    this->pushbutton_inactive_press_text = style->value("text_color", this->pushbutton_inactive_hover_text).toString();
    this->pushbutton_inactive_press_border = style->value("border_color_press", this->pushbutton_inactive_hover_border).toString();
    this->pushbutton_disabled_background = style->value("widget_disabled", this->pushbutton_inactive_background).toString();
    this->pushbutton_disabled_text = style->value("text_color_disabled", this->pushbutton_inactive_text).toString();
    this->pushbutton_disabled_border = style->value("border_color_disabled", this->pushbutton_inactive_border).toString();

    this->lineedit_background = style->value("widget", "#FFFFFF").toString();
    this->lineedit_text = style->value("text_color", "#000000").toString();
    this->lineedit_text_weight = style->value("text_weight", "bold").toString();
    this->lineedit_selection = style->value("selection_color", "#808080").toString();
    this->lineedit_selection_text = style->value("selection_text_color", this->lineedit_text).toString();
    this->lineedit_border = style->value("border_color", "#000000").toString();
    this->lineedit_border_width = style->value("border_width", "1px").toString();
    this->lineedit_hover = style->value("widget_hover", this->lineedit_background).toString();
    this->lineedit_hover_text = style->value("text_color", this->lineedit_text).toString();
    this->lineedit_hover_border = style->value("border_color_hover", this->lineedit_border).toString();
    this->lineedit_popup = style->value("widget", this->lineedit_background).toString();
    this->lineedit_popup_text = style->value("text_color", this->lineedit_text).toString();
    this->lineedit_popup_border = style->value("border_color", this->lineedit_border).toString();
    this->lineedit_popup_hover = style->value("widget", this->lineedit_popup).toString();
    this->lineedit_popup_hover_text = style->value("text_color", this->lineedit_popup_text).toString();
    this->lineedit_popup_hover_border = style->value("border_color", this->lineedit_popup_border).toString();

    this->listview_background = style->value("popup", "#FFFFFF").toString();
    this->listview_text = style->value("text_color", "#000000").toString();
    this->listview_text_weight = style->value("text_weight", "bold").toString();
    this->listview_border = style->value("popup_border_color", this->lineedit_border).toString();
    this->listview_border_width = style->value("border_width", "1px").toString();
    this->listview_item = style->value("popup", this->listview_background).toString();
    this->listview_item_text = style->value("text_color", this->listview_text).toString();
    this->listview_item_selection = style->value("selection_color", "#808080").toString();
    this->listview_item_selection_text = style->value("selection_text_color", this->listview_text).toString();
    this->listview_item_hover = style->value("selection_color_hover", this->listview_item_selection).toString();
    this->listview_item_hover_text = style->value("selection_text_color", this->listview_item_selection_text).toString();
    this->listview_item_disable = style->value("popup_disabled", "#000000").toString();
    this->listview_item_disable_text = style->value("text_color_disabled", "#A0A0A0").toString();

    this->tabwidget_background = style->value("widget", "#FFFFFF").toString();
    this->tabwidget_unselected = style->value("widget_inactive", "#CCCCCC").toString();
    this->tabwidget_unselected_hover = style->value("widget_hover", this->tabwidget_unselected).toString();
    this->tabwidget_text = style->value("text_color", "#000000").toString();
    this->tabwidget_text_weight = style->value("text_weight", "bold").toString();
    this->tabwidget_border = style->value("border_color", "#000000").toString();
    this->tabwidget_border_width = style->value("border_width", "1px").toString();

    this->scrollbar_border_width = style->value("scrollbar_border_width", "1px").toString();
    this->scrollbar_border = style->value("border_color", "#000000").toString();
    this->scrollbar_handle = style->value("scrollbar_handle", "#FFFFFF").toString();
    this->scrollbar_background = style->value("scrollbar_background", "#000000").toString();

    this->lasermenu_popup = style->value("lasermenu_popup", this->lineedit_background).toString();

    this->fluormenu_remove = style->value("fluormenu_remove", "#FFFFFF").toString();
    this->fluormenu_remove_hover = style->value("fluormenu_remove_hover", this->fluormenu_remove).toString();
    this->fluormenu_remove_press = style->value("fluormenu_remove_press", this->fluormenu_remove_hover).toString();
    this->fluormenu_popup = style->value("fluormenu_popup", this->listview_background).toString();
    this->fluormenu_background = style->value("fluormenu_background", this->background).toString();

    this->graph_scene = style->value("graph_scene", this->background).toString();
    this->graph_background = style->value("graph_background", this->graph_scene).toString();
    this->graph_background_hover = style->value("graph_background_hover", this->graph_background).toString();
    this->graph_background_press = style->value("graph_background_press", this->graph_background_hover).toString();
    this->graph_label = style->value("graph_label", this->pushbutton_text).toString();
    this->graph_label_weight = style->value("graph_label_weight", this->pushbutton_text_weight).toString();
    this->graph_grid_label = style->value("graph_grid_label", this->pushbutton_text).toString();
    this->graph_grid_label_weight = style->value("graph_grid_label_weight", this->pushbutton_text_weight).toString();
    this->graph_axis = style->value("graph_axis", this->pushbutton_border).toString();
    this->graph_axis_hover = style->value("graph_axis_hover", this->pushbutton_hover_border).toString();
    this->graph_axis_press = style->value("graph_axis_press", this->pushbutton_press_border).toString();
    this->graph_absorption_width = style->value("graph_absorption_width", "2px").toString();
    this->graph_absorption_style = style->value("graph_absorption_style", "dot").toString();
    this->graph_excitation_width = style->value("graph_excitation_width", "2px").toString();
    this->graph_excitation_style = style->value("graph_excitation_style", "dash").toString();
    this->graph_emission_width = style->value("graph_emission_width", "2px").toString();
    this->graph_emission_style = style->value("graph_emission_style", "solid").toString();

    style->endGroup();

    this->buildStyleSheet();
}

/*
Combines all individual stylesheet sections into this->styleSheet(). Uses the default font (non DPI scaled)
*/
void StyleBuilder::buildStyleSheet(){
    QFontMetrics metrics = QFontMetrics(static_cast<QApplication*>(QCoreApplication::instance())->font());

    this->buildStyleSheet(metrics);
}

/*
Combines all individual stylesheet sections into the this->styleSheet(). Uses font metrics to adjust
text-relative inputs (when necessary) into pixels
    :param metrics: dpi scaled font metrics (build a QFontMetrics using the widgets QFont, and QPaintDevice)
*/
void StyleBuilder::buildStyleSheet(const QFontMetrics& metrics){
    QString stylesheet = "";
    stylesheet.append(this->buildLayout(metrics));
    stylesheet.append(this->buildLabel(metrics));
    stylesheet.append(this->buildPushButton());
    stylesheet.append(this->buildLineEdit());
    stylesheet.append(this->buildListView());
    stylesheet.append(this->buildTabWidget());
    stylesheet.append(this->buildScrollBar());
    stylesheet.append(this->buildCentralWindow());
    stylesheet.append(this->buildToolBar(metrics));
    stylesheet.append(this->buildLaserMenu(metrics));
    stylesheet.append(this->buildFluorMenu(metrics));
    stylesheet.append(this->buildGraph(metrics));
    stylesheet.append(this->buildAboutWindow(metrics));

    this->stylesheet = stylesheet;
}

/*
Builds the stylesheet for the main/central layouts
*/
QString StyleBuilder::buildLayout(const QFontMetrics& metrics) const {
    QString style;
    style = "Central--Controller {"
            " qproperty-layout_margins: %1;"
            " qproperty-layout_spacing: %2;"
            "} "
            "General--StyledWindow {"
            " qproperty-layout_margins: %1;"
            " qproperty-layout_spacing: %2;"
            "} ";
    style = style.arg(
        StyleBuilder::toPixels(metrics, this->layout_margins),
        StyleBuilder::toPixels(metrics, this->layout_spacing)
    );
    return style;
}

/*
Builds the stylesheet for QLabel
    :returns: stylesheet
*/
QString StyleBuilder::buildLabel(const QFontMetrics& metrics) const {
    QString style;
    style = "QLabel {"
            " border-style: none;"
            " color: %1;"
            " font: %2;"
            " padding: 6px;"
            "} "
            "QLabel[enabled=false] {"
            " color: %3;"
            "} "
            "QLabel#h1 {"
            " font-size: %4px;"
            " margin-top: %5px;"
            " margin-bottom: %5px;"
            " margin-left: 0;"
            " margin-right: 0;"
            "} "
            "QLabel#h2 {"
            " font-size: %6px;"
            " margin-top: %7px;"
            " margin-bottom: %7px;"
            " margin-left: 0;"
            " margin-right: 0;"
            "} "
            "QLabel#h3 {"
            "} "
            "QLabel#h4 {"
            " font-weight: normal;"
            "} ";
    style = style.arg(
        this->label_text,
        this->label_text_weight,
        this->label_disabled_text,
        StyleBuilder::toPixels(metrics, "2em"),
        StyleBuilder::toPixels(metrics, "0.67em"),
        StyleBuilder::toPixels(metrics, "1.5em"),
        StyleBuilder::toPixels(metrics, "0.83em")
    );
    return style;
}

/*
Builds the stylesheet for QPushButton
    :returns: stylesheet
*/
QString StyleBuilder::buildPushButton() const {
    QString style;
    style = "QPushButton {"
            " background-color: %1;"
            " border-style: solid;"
            " border-width: %2;"
            " border-color: %3;"
            " color: %4;"
            " font: %5;"
            " padding: 6px;"
            "} "
            "QPushButton::pressed::hover {"
            " background-color: %6;"
            " border-color: %7;"
            " color: %8;"
            "} "
            "QPushButton::hover {"
            " background-color: %9;"
            " border-color: %10;"
            " color: %11;"
            "} "
            "QPushButton[active=true] {"
            " background-color: %1;"
            " border-color: %3;"
            " color: %4;"
            "} "
            "QPushButton::pressed::hover[active=true] {"
            " background-color: %6;"
            " border-color: %7;"
            " color: %8;"
            "} "
            "QPushButton::hover[active=true] {"
            " background-color: %9;"
            " border-color: %10;"
            " color: %11;"
            "} "
            "QPushButton[active=false] {"
            " background-color: %12;"
            " border-color: %13;"
            " color: %14;"
            "} "
            "QPushButton::pressed::hover[active=false] {"
            " background-color: %15;"
            " border-color: %16;"
            " color: %17;"
            "} "
            "QPushButton::hover[active=false] {"
            " background-color: %18;"
            " border-color: %19;"
            " color: %20;"
            "} "
            "QPushButton::!enabled {"
            " background-color: %21;"
            " border-color: %22;"
            " color: %23;"
            "} ";

    style = style.arg(
        this->pushbutton_background,
        this->pushbutton_border_width,
        this->pushbutton_border, 
        this->pushbutton_text,
        this->pushbutton_text_weight,
        this->pushbutton_press_background,
        this->pushbutton_press_border,
        this->pushbutton_press_text
    );
    style = style.arg(
        this->pushbutton_hover_background,
        this->pushbutton_hover_border,
        this->pushbutton_hover_text,
        this->pushbutton_inactive_background,
        this->pushbutton_inactive_border,
        this->pushbutton_inactive_text,
        this->pushbutton_inactive_press_background,
        this->pushbutton_inactive_press_border,
        this->pushbutton_inactive_press_text
    );
    style = style.arg(
        this->pushbutton_inactive_hover_background,
        this->pushbutton_inactive_hover_border,
        this->pushbutton_inactive_hover_text,
        this->pushbutton_disabled_background,
        this->pushbutton_disabled_border,
        this->pushbutton_disabled_text
    );

    return(style);
}

/*
Builds the stylesheet for QLineEdit
    :returns: stylesheet
*/
QString StyleBuilder::buildLineEdit() const {
    QString style;
    style = "QLineEdit {"
            " background-color: %1;"
            " color: %2;"
            " font: %3;"
            " selection-background-color: %4;"
            " selection-color: %5;"
            " border-color: %6;"
            " border-style: solid;"
            " border-width: %7;"
            " padding: 6px;"
            "} "
            "QLineEdit::hover {"
            " background-color: %8;"
            " color: %9;"
            " border-color: %10;"
            "} "
            "QLineEdit[popup=false] {"
            " background-color: %1;"
            " color: %2;"
            " font: %3;"
            " selection-background-color: %4;"
            " selection-color: %5;"
            " border-color: %6;"
            " border-style: solid;"
            " border-width: %7;"
            " padding: 6px;"
            "} "
            "QLineEdit::hover[popup=false] {"
            " background-color: %8;"
            " color: %9;"
            " border-color: %10;"
            "} "
            "QLineEdit[popup=true] {"
            " background-color: %11;"
            " color: %12;"
            " font: %3;"
            " selection-background-color: %4;"
            " selection-color: %5;"
            " border-color: %13;"
            " border-style: solid;"
            " border-width: %7;"
            " padding: 6px;"
            "} "
            "QLineEdit::hover[popup=true] {"
            " background-color: %14;"
            " color: %15;"
            " border-color: %16;"
            "} ";
    
    style = style.arg(
        this->lineedit_background,
        this->lineedit_text,
        this->lineedit_text_weight,
        this->lineedit_selection,
        this->lineedit_selection_text,
        this->lineedit_border,
        this->lineedit_border_width
    );
    style = style.arg(
        this->lineedit_hover,
        this->lineedit_hover_text,
        this->lineedit_hover_border,
        this->lineedit_popup,
        this->lineedit_popup_text,
        this->lineedit_popup_border,
        this->lineedit_popup_hover,
        this->lineedit_popup_hover_text,
        this->lineedit_popup_hover_border
    );
    return(style);
}

/*
Builds the stylesheet for QListView
    :returns: stylesheet
*/
QString StyleBuilder::buildListView() const {
    QString style;
    style = "QListView {"
            " background-color: %1;"
            " color: %2;"
            " font: %3;"
            " border-color: %4;"
            " border-style: solid;"
            " border-width: %5;"
            " padding: 6px;"
            " outline: none;"
            "} "
            "QListView::item {"
            " background-color: %6;"
            " color: %7;"
            "} "
            "QListView::item::hover {"
            " background-color: %8;"
            " color: %9;"
            "} "
            "QListView::item::selected {"
            " background-color: %10;"
            " color: %11;"
            "} "
            "QListView::item::hover::selected {"
            " background-color: %10;"
            " color: %11;"
            "} "
            "QListView::item::disabled {"
            " background-color: %12;"
            " color: %13;"
            "} "
            "QListView::indicator {"
            " color: %7;"
            " border-color: %7;"
            " border-style: solid;"
            " border-width: %5;"
            "} "
            "QListView::indicator::unchecked {"
            " color: %7;"
            "} "
            "QListView::indicator::unchecked::hover {"
            " color: %9;"
            " border-color: %9;"
            "} "
            "QListView::indicator::unchecked::selected {"
            " color: %11;"
            " border-color: %11;"
            "} "
            "QListView::indicator::unchecked::hover::selected {"
            " color: %11;"
            " border-color: %11;"
            "} "
            "QListView::indicator::checked {"
            " color: %7;"
            " background-color: %7;"
            "} "
            "QListView::indicator::checked::hover {"
            " background-color: %9;"
            " color: %9;"
            " border-color: %9;"
            "} "
            "QListView::indicator::checked::selected {"
            " background-color: %11;"
            " color: %11;"
            " border-color: %11;"
            "} "
            "QListView::indicator::checked::hover::selected {"
            " background-color: %11;"
            " color: %11;"
            " border-color: %11;"
            "} "
            "QListView::indicator::checked::disabled {"
            " background-color: %13;"
            " color: %13;"
            " border-color: %13;"
            "} "
            "QListView .QWidget {"
            " background: %1;"
            "} ";

    style = style.arg(
        this->listview_background,
        this->listview_text,
        this->listview_text_weight,
        this->listview_border,
        this->listview_border_width
    );
    style = style.arg(
        this->listview_item,
        this->listview_item_text,
        this->listview_item_hover,
        this->listview_item_hover_text,
        this->listview_item_selection,
        this->listview_item_selection_text,
        this->listview_item_disable,
        this->listview_item_disable_text
    );

    return(style);
}

/*
Builds the stylesheet for QTabWidget
    :returns: stylesheet
*/
QString StyleBuilder::buildTabWidget() const {
    QString style;
    style = "QTabWidget::pane {"
            " padding: 0px;"
            "} "
            "QTabWidget::pane .QStackedWidget {"
            " background-color: %1;"
            "} "
            "QTabBar {"
            " color: %2;"
            " font: %3;"
            " border-style: solid;"
            " border-right-color: %4;"
            " border-right-width: %5;"
            "} "
            "QTabBar::tab::selected {"
            " background: %1;"
            " min-width: 10em;"
            "} "
            "QTabBar::tab::!selected {"
            " background: %6;"
            " margin-top: 2px;"
            "} "
            "QTabBar::tab::!selected:hover {"
            " background: %7;"
            " margin-top: 0px;"
            "} "
            "QTabBar::tab::first {"
            " margin-left: 6px;"
            "} "
            "QTabBar::tab::right {"
            " margin-right 6px;"
            "} ";

    style = style.arg(
        this->tabwidget_background,
        this->tabwidget_text,
        this->tabwidget_text_weight,
        this->tabwidget_border,
        this->tabwidget_border_width,
        this->tabwidget_unselected,
        this->tabwidget_unselected_hover
    );

    return(style);
}

/*
Builds the stylesheet for QScrollBar
    :returns: stylesheet
*/
QString StyleBuilder::buildScrollBar() const {
    QString style;
    style = "QScrollBar::vertical {"
            " background: none #000000;"
            " border-top: %1 solid %2;"
            " border-bottom: %1 solid %2;"
            " border-left: %1 solid %2;"
            " border-right: %1 solid %2;"
            " width: 0.3em;"
            " margin: 0px 0px 0px 0px;"
            "} "
            "QScrollBar::handle::vertical {"
            " background: %3;"
            "} "
            "QScrollBar::add-page::vertical {"
            " background: %4;"
            "} "
            "QScrollBar::sub-page::vertical {"
            " background: %5;"
            "} "
            "QScrollBar::add-line::vertical {"
            " background: none;"
            " border: 0px none #000000;"
            "} " 
            "QScrollBar::sub-line::vertical {"
            " background: none;"
            " border: 0px none #000000;"
            "} ";
    
    style = style.arg(
        this->scrollbar_border_width,
        this->scrollbar_border,
        this->scrollbar_handle,
        this->scrollbar_background,
        this->scrollbar_background
    );

    return(style);
}

/*
Builds the stylesheet for central_window
    :returns: stylesheet
*/
QString StyleBuilder::buildCentralWindow() const {
    QString style;
    style = "QMainWindow {"
            " background: %1;"
            "} ";
    style = style.arg(this->background);
    return(style);
}

/*
Builds the stylesheet for the toolbar
    :returns: stylesheet
*/
QString StyleBuilder::buildToolBar(const QFontMetrics& metrics) const {
    QString style;
    style = "Bar--Controller {"
            " qproperty-layout_spacing: %1;"
            "} "
            "Bar--IconPushButton[active=true] {"
            " qproperty-scale: %2;"
            " padding: 0px;"
            "} "
            "Bar--IconPushButton[active=false] {"
            " qproperty-scale: %2;"
            " padding: 0px;"
            "} "
            "Bar--LaserButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%3_laser_disabled.png);"
            "} "
            "Bar--LaserButton[enabled=false][active=false] {"
            " qproperty-icon: url(:/icons/%3_laser_disabled.png);"
            "} "
            "Bar--LaserButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%3_laser_active.png);"
            "} "
            "Bar--LaserButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%3_laser_inactive.png);"
            "} "
            "Bar--ExcitationButton[active=true] {"
            " qproperty-icon: url(:/icons/%3_excitation_active.png);"
            "} "
            "Bar--ExcitationButton[active=false] {"
            " qproperty-icon: url(:/icons/%3_excitation_inactive.png);"
            "} "
            "Bar--EmissionButton[active=true] {"
            " qproperty-icon: url(:/icons/%3_emission_active.png);"
            "} "
            "Bar--EmissionButton[active=false] {"
            " qproperty-icon: url(:/icons/%3_emission_inactive.png);"
            "} "
            "Bar--FilterButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%3_filter_disabled.png);"
            "} "
            "Bar--FilterButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%3_filter_active.png);"
            "} "
            "Bar--FilterButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%3_filter_inactive.png);"
            "} "
            "Bar--GraphAddButton[enabled=false]{"
            " qproperty-icon: url(:/icons/%3_graph_add_disabled.png);"
            "} "
            "Bar--GraphAddButton[enabled=true] {"
            " qproperty-icon: url(:/icons/%3_graph_add_active.png);"
            "} "
            "Bar--GraphRemoveButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%3_graph_remove_disabled.png);"
            "} "
            "Bar--GraphRemoveButton[enabled=true] {"
            " qproperty-icon: url(:/icons/%3_graph_remove_active.png);"
            "} "
            "Bar--LaserLinesButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%3_laserlines_disabled.png);"
            "} "
            "Bar--LaserLinesButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%3_laserlines_active.png);"
            "} "
            "Bar--LaserLinesButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%3_laserlines_inactive.png);"
            "} "
            "Bar--Label {"
            " min-width: 10em;"
            "} ";
    style = style.arg(
        StyleBuilder::toPixels(metrics, this->layout_spacing),
        StyleBuilder::toPixels(metrics, "1eh+8px"),
        this->icons
    );
    return(style);
}

/*
Builds the stylesheet for laser_menu
    :returns: stylesheet
*/
QString StyleBuilder::buildLaserMenu(const QFontMetrics& metrics) const {
    QString style;
    style = "Laser--PushButton {"
            " width: %1px;"
            "} "
            "Laser--LineEdit {"
            " width: %1px;"
            " max-height: %2px;"
            "} "
            "Laser--AbstractPopup {"
            " qproperty-viewport_margins_scroll_bar: %3;"
            "} "
            "Laser--AbstractPopup .QWidget {"
            " background: %4;"
            "} "
            "Laser--AbstractPopup QScrollBar::vertical {"
            " margin: 0px 0px 0px 0px;"
            " width: %5px;"
            "} "
            "General--Separator {"
            " qproperty-separator_height: %6;"
            " qproperty-separator_color: %7;"
            " qproperty-separator_width: %8;"
            "} ";
    
    style = style.arg(
        StyleBuilder::toPixels(metrics, "15em"),
        StyleBuilder::toPixels(metrics, "1eh"),
        StyleBuilder::toPixels(metrics, this->layout_sub_spacing),
        this->lasermenu_popup,
        StyleBuilder::toPixels(metrics, "0.5em"),
        StyleBuilder::toPixels(metrics, this->layout_spacing),
        this->listview_border,
        this->listview_border_width
    );

    return(style);
}

/*
Builds the stylesheet for the fluor_menu
    :returns: stylesheet
*/
QString StyleBuilder::buildFluorMenu(const QFontMetrics& metrics) const {
    QString style;
    style = "Fluor--Controller {"
            " qproperty-layout_spacing: %1;"
            "} "
            "Fluor--ScrollController {"
            " qproperty-layout_spacing: %1;"
            " qproperty-layout_margins_scroll_bar: %2;"
            "} "
            "Fluor--ButtonsController {"
            " qproperty-layout_spacing: %2;"
            "} "
            "Fluor--PushButton {"
            " width: %3px;"
            "} "
            "Fluor--EmissionButton {"
            " text-align: left;"
            "} "
            "Fluor--EmissionButton[active=true][select=true] {"
            " background-color: %4;"
            " border-color: %5;"
            " color: %6;"
            "} "
            "Fluor--EmissionButton::pressed::hover[active=true][select=true] {"
            " background-color: %7;"
            " border-color: %8;"
            " color: %9;"
            "} "
            "Fluor--EmissionButton::hover[active=true][select=true] {"
            " background-color: %4;"
            " border-color: %5;"
            " color: %6;"
            "} "
            "Fluor--EmissionButton[active=false][select=true] {"
            " background-color: %10;"
            " border-color: %11;"
            " color: %12;"
            "} "
            "Fluor--EmissionButton::pressed::hover[active=false][select=true] {"
            " background-color: %13;"
            " border-color: %14;"
            " color: %15;"
            "} "
            "Fluor--EmissionButton::hover[active=false][select=true] {"
            " background-color: %10;"
            " border-color: %11;"
            " color: %12;"
            "} "
            "Fluor--ExcitationButton {"
            " padding: 6px 0px 6px 0px;"
            " width: %22px;"
            "} "
            "Fluor--RemoveButton[active=true] {"
            " background-color: %16;"
            " padding: 6px 0px 6px 0px;"
            " width: %22px;"
            "} "
            "Fluor--RemoveButton::hover[active=true] {"
            " background-color: %17;"
            "} "
            "Fluor--RemoveButton::pressed::hover[active=true] {"
            " background-color: %18;"
            "} "
            "Fluor--LineEdit {"
            " width: %3px;"
            " max-height: %19px;"
            "} "
            "Fluor--Popup {"
            " qproperty-viewport_margins_scroll_bar: %2;"
            "} "
            "Fluor--Popup .QWidget {"
            " background: %20;"
            "} "
            "Fluor--ScrollController .QWidget {"
            " background: %21;"
            "} "
            "Fluor--Popup QScrollBar::vertical {"
            " margin: 0px 0px 0px 0px;"
            " width: %22px;"
            "} "
            "Fluor--ScrollController QScrollBar::vertical {"
            " margin: 0px 0px 0px 0px;"
            " width: %22px;"
            "} ";

    style = style.arg(
        StyleBuilder::toPixels(metrics, this->layout_spacing),
        StyleBuilder::toPixels(metrics, this->layout_sub_spacing),
        StyleBuilder::toPixels(metrics, "23em"),
        this->pushbutton_hover_background,
        this->pushbutton_hover_border,
        this->pushbutton_hover_text,
        this->pushbutton_press_background,
        this->pushbutton_press_border,
        this->pushbutton_press_text
    );
    style = style.arg(
        this->pushbutton_inactive_hover_background,
        this->pushbutton_inactive_hover_border,
        this->pushbutton_inactive_hover_text,
        this->pushbutton_inactive_press_background,
        this->pushbutton_inactive_press_border,
        this->pushbutton_inactive_press_text,
        this->fluormenu_remove,
        this->fluormenu_remove_hover,
        this->fluormenu_remove_press
    );
    style = style.arg(
        StyleBuilder::toPixels(metrics, "1eh"),
        this->fluormenu_popup,
        this->fluormenu_background,
        StyleBuilder::toPixels(metrics, "0.5em")
    );
    return(style);
}

/*
Builds the stylesheet for graph
    :returns: stylesheet
*/
QString StyleBuilder::buildGraph(const QFontMetrics& metrics) const {
    QString style;
    style = "Graph--ScrollController {"
            " qproperty-layout_spacing: %1;"
            " qproperty-layout_margins_scroll_bar: %2;"
            "} "
            "Graph--ScrollController .QWidget {"
            " background: %3;"
            "} "
            "Graph--ScrollController QScrollBar::vertical {"
            " margin: 0px 0px 0px 0px;"
            " width: %4px;"
            "} "
            "Graph--GraphicsView {"
            " border: 0px;"
            "} "
            "Graph--Format--Style {"
            " qproperty-scene: %5;"
            " qproperty-background: %6;"
            " qproperty-background_hover: %7;"
            " qproperty-background_press: %8;"
            " qproperty-label: %9;"
            " qproperty-label_weight: '%11';"
            " qproperty-grid_label: %12;"
            " qproperty-grid_label_weight: '%13';"
            " qproperty-axis: %14;"
            " qproperty-axis_hover: %15;"
            " qproperty-axis_press: %16;"
            " qproperty-absorption_width: %17;"
            " qproperty-absorption_style: '%18';"
            " qproperty-excitation_width: %19;"
            " qproperty-excitation_style: '%21';"
            " qproperty-emission_width: %22;"
            " qproperty-emission_style: '%23';"
            " qproperty-colorbar_height: %24;"
            "} ";
    style = style.arg(
        StyleBuilder::toPixels(metrics, this->layout_spacing),
        StyleBuilder::toPixels(metrics, this->layout_sub_spacing),
        this->background,
        StyleBuilder::toPixels(metrics, "0.5em"),
        this->graph_scene,
        this->graph_background,
        this->graph_background_hover,
        this->graph_background_press,
        this->graph_label
    );
    style = style.arg(
        this->graph_label_weight,
        this->graph_grid_label,
        this->graph_grid_label_weight,
        this->graph_axis,
        this->graph_axis_hover,
        this->graph_axis_press,
        StyleBuilder::toPixels(metrics, this->graph_absorption_width),
        this->graph_absorption_style,
        StyleBuilder::toPixels(metrics, this->graph_excitation_width)
    );
    style = style.arg(
        this->graph_excitation_style,
        StyleBuilder::toPixels(metrics, this->graph_emission_width),
        this->graph_emission_style,
        StyleBuilder::toPixels(metrics, "1.5em")
    );
    return(style);
}

/*
Builds the stylesheet for the about window
*/
QString StyleBuilder::buildAboutWindow(const QFontMetrics& metrics) const {
    QString style;
    style = "General--AboutWindow {"
            " background: %1;"
            "} "
            "General--AboutIcon {"
            " qproperty-pixmap: url(:/icons/%2_fluor.png);"
            " qproperty-scale: %3;"
            "} ";
    style = style.arg(
        this->background,
        this->icons,
        StyleBuilder::toPixels(metrics, "12eh")
    );
    return style;
}

/*
Transforms a px, em (=M width), ex (=x height), eh (=font height) measure into pixels
If unconvertable returns the fallback value
    :param font: dpi corrected font metrics
    :param text: input text
    :param fallback: if text is unconvertable, returns this value in pixels
    :returns: the value in pixels as string without 'px' unit
*/
QString StyleBuilder::toPixels(const QFontMetrics& metrics, const QString& text, QString fallback) {
    int em_pixels = metrics.width('M');
    int ex_pixels = metrics.xHeight();
    int eh_pixels = metrics.height();
   
    // Extract all parts of the input string
    QString number = "";
    QString qualifyer = "";
    QString modifyer = "";
    QString mod_number = "";
    QString mod_qualifyer = "";
    bool isNumber = true;
    bool isMod = false;
    bool isFallback = false;

    for(QChar letter : text){
        if(isMod){
            if(letter.isDigit() || letter == QChar('.')){
                if(isNumber){
                    mod_number.append(letter);
                }else{
                    // Means a new digit is found after the number section has ended
                    isFallback = true;
                }
            }else if(letter.isLetter()){
                if(isNumber){
                    isNumber = false;
                }
                mod_qualifyer.append(letter);
            }else if(letter.isSpace()){
                continue;
            }else{
                isFallback = true;
            }
        }else{
            if(letter.isDigit() || letter == QChar('.')){
                if(isNumber){
                    number.append(letter);
                }else{
                    // Means a new digit is found after the number section has ended
                    isFallback = true;
                }
            }else if(letter.isLetter()){
                if(isNumber){
                    isNumber = false;
                }
                qualifyer.append(letter);
            }else if(letter == QChar('-') or letter == QChar('+')){
                modifyer.append(letter);
                isMod = true;
                isNumber = true;
            }else if(letter.isSpace()){
                continue;
            }else{
                isFallback = true;
            }
        }
    }

    // Check if data extraction went without issues
    if(isFallback){
        qWarning() << "Application::textToPixel: unconvertable input text" << text;
        return fallback;
    }

    // Transform first value
    double number_px = 0.0;
    if(qualifyer == "px"){
        number_px = static_cast<double>(number.toInt());
    }else if(qualifyer == "em"){
        number_px = number.toDouble() * static_cast<double>(em_pixels);
    }else if(qualifyer == "ex"){
        number_px = number.toDouble() * static_cast<double>(ex_pixels);
    }else if(qualifyer == "eh"){
        number_px = number.toDouble() * static_cast<double>(eh_pixels);
    }else{
        qWarning() << "Application::textToPixel: unconvertable input text" << text;
        return fallback;
    }

    // Check if any modification
    if(!isMod){
        return QString::number(number_px, 'f', 0);;
    }

    // Transform modification
    double mod_number_px = 0.0;
    if(mod_qualifyer == "px"){
        mod_number_px = static_cast<double>(mod_number.toInt());
    }else if(mod_qualifyer == "em"){
        mod_number_px = mod_number.toDouble() * static_cast<double>(em_pixels);
    }else if(mod_qualifyer == "ex"){
        mod_number_px = mod_number.toDouble() * static_cast<double>(ex_pixels);
    }else if(mod_qualifyer == "eh"){
        mod_number_px = mod_number.toDouble() * static_cast<double>(eh_pixels);
    }else{
        qWarning() << "Application::textToPixel: unconvertable input text" << text;
        return fallback;
    }

    // Perfrom modification
    if(modifyer == "-"){
        number_px -= mod_number_px;
    }else if(modifyer == "+"){
        number_px += mod_number_px;
    }else{
        qWarning() << "Application::textToPixel: unconvertable input text" << text;
        return fallback;
    }

    return QString::number(number_px, 'f', 0);
}

} // Data namespace
