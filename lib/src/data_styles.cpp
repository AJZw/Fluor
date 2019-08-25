/**** General **************************************************************
** Version:    v0.9.5
** Date:       2019-07-30
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_styles.h"

#include <QFont>
#include <QFontMetrics>
#include <QApplication>
#include <QDebug>

namespace Data {

namespace Style {

/* 
(Constructor) Builds a CSS stylesheet (from QSettings)
*/
Builder::Builder() :
    stylesheet(""),
    
    icons("light"),
    background("#FFFFFF"),

    label_background("#FFFFFF"),
    label_text("#000000"),
    label_text_weight("bold"),
    label_border("#000000"),
    label_border_width("1px"),
    label_disabled_background(this->label_background),
    label_disabled_text(this->label_text),
    label_disabled_border(this->label_border),

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

    graph_background("#FFFFFF"),
    graph_text("#000000"),
    graph_text_weight("bold"),
    graph_plot("#FFFFFF"),
    graph_plot_focus("#AAAAAA"),
    graph_axis("#000000"),
    graph_grid("#000000"),
    graph_excitation_width("2px"),
    graph_excitation_style("Dash"),
    graph_emission_width("2px"),
    graph_emission_style("Dash"),
    graph_detector("#000000")
{
    this->buildStylesheet();
}

/*
Getter: returns the StyleBuilder.stylesheet
    :returns: stylesheet
*/
QString Builder::getStyleSheet() const {
    return(this->stylesheet);
}

/*
Getter: returns all style names from the settings file
    :param data: a data factory
    :returns: vector of style_id(s)
*/
std::vector<QString> Builder::getStyleIDs(const Data::Factory& data) const {
    std::unique_ptr<QSettings> style;
    style = data.get(Data::Factory::styles);
    
    QStringList style_groups;
    style_groups = style->childGroups();
    
    return style_groups.toVector().toStdVector();
}

/*
Loads attributes from settings file
    :param data: a data factory
    :param style_id: the id of the style to load
*/
void Builder::loadStyle(const Data::Factory& data, const QString& style_id){
    std::unique_ptr<QSettings> style;
    style = data.get(Data::Factory::styles);

    QStringList style_groups;
    style_groups = style->childGroups();
    if(!style_groups.contains(style_id)){
        qWarning() << "StyleBuilder::loadStyle: " << style_id << " group is not found within the styles data object";
        return;
    }else{
        style->beginGroup(style_id);
    };

    this->icons = style->value("icons", "light").toString();
    this->background = style->value("main_background", "#FFFFFF").toString();

    this->label_background = style->value("widget", "#FFFFFF").toString();
    this->label_text = style->value("text_color", "#000000").toString();
    this->label_text_weight = style->value("text_weight", "bold").toString();
    this->label_border = style->value("border_color", "#000000").toString();
    this->label_border_width = style->value("border_width", "1px").toString();
    this->label_disabled_background = style->value("widget_disabled", this->label_background).toString();
    this->label_disabled_text = style->value("text_color_disabled", this->label_text).toString();
    this->label_disabled_border = style->value("border_color_disabled", this->label_border).toString();

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

    this->graph_background = style->value("graph_background", this->background).toString();
    this->graph_text = style->value("graph_text", this->pushbutton_text).toString();
    this->graph_text_weight = style->value("graph_text_weight", this->pushbutton_text_weight).toString();
    this->graph_plot = style->value("graph_plot", this->background).toString();
    this->graph_plot_focus = style->value("graph_plot_focus", "#AAAAAA").toString();
    this->graph_axis = style->value("graph_axis", this->pushbutton_text).toString();
    this->graph_grid = style->value("graph_grid", this->pushbutton_text).toString();
    this->graph_excitation_width = style->value("graph_excitation_width", "2px").toString();
    this->graph_excitation_style = style->value("graph_excitation_style", "dash").toString();
    this->graph_emission_width = style->value("graph_emission_width", "2px").toString();
    this->graph_emission_style = style->value("graph_emission_style", "solid").toString();
    this->graph_detector = style->value("graph_detector", "#000000").toString();

    style->endGroup();

    this->buildStylesheet();
}

/*
Combines all individual stylesheet sections and sets StyleBuilder.stylesheet with the build stylesheet
*/
void Builder::buildStylesheet(){
    QString stylesheet;
    stylesheet.append(this->buildLabel());
    stylesheet.append(this->buildPushButton());
    stylesheet.append(this->buildLineEdit());
    stylesheet.append(this->buildListView());
    stylesheet.append(this->buildTabWidget());
    stylesheet.append(this->buildScrollBar());
    stylesheet.append(this->buildCentralWindow());
    stylesheet.append(this->buildToolBar());
    stylesheet.append(this->buildLaserMenu());
    stylesheet.append(this->buildFluorMenu());
    stylesheet.append(this->buildGraph());

    this->stylesheet = stylesheet;
}

/*
Builds the stylesheet for QLabel
    :returns: stylesheet
*/
QString Builder::buildLabel() const {
    QString style;
    style = "QLabel {"
            " background-color: %1;"
            " border-style: solid;"
            " border-width: %2;"
            " border-color: %3;"
            " color: %4;"
            " font: %5;"
            " padding: 6px;"
            "} "
            "QLabel[enabled=false] {"
            " background-color: %6;"
            " border-color: %7;"
            " color: %8;"
            "} ";
    style = style.arg(
        this->label_background,
        this->label_border_width,
        this->label_border, 
        this->label_text,
        this->label_text_weight,
        this->label_disabled_background,
        this->label_disabled_border,
        this->label_disabled_text);
    return style;
}

/*
Builds the stylesheet for QPushButton
    :returns: stylesheet
*/
QString Builder::buildPushButton() const {
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
        this->pushbutton_press_text);
    style = style.arg(
        this->pushbutton_hover_background,
        this->pushbutton_hover_border,
        this->pushbutton_hover_text,
        this->pushbutton_inactive_background,
        this->pushbutton_inactive_border,
        this->pushbutton_inactive_text,
        this->pushbutton_inactive_press_background,
        this->pushbutton_inactive_press_border,
        this->pushbutton_inactive_press_text);
    style = style.arg(
        this->pushbutton_inactive_hover_background,
        this->pushbutton_inactive_hover_border,
        this->pushbutton_inactive_hover_text,
        this->pushbutton_disabled_background,
        this->pushbutton_disabled_border,
        this->pushbutton_disabled_text);

    return(style);
}

/*
Builds the stylesheet for QLineEdit
    :returns: stylesheet
*/
QString Builder::buildLineEdit() const {
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
QString Builder::buildListView() const {
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
QString Builder::buildTabWidget() const {
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
QString Builder::buildScrollBar() const {
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
QString Builder::buildCentralWindow() const {
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
QString Builder::buildToolBar() const {
    QString style;
    style = "Bar--IconPushButton[active=true] {"
            " padding: 0px;"
            "} "
            "Bar--IconPushButton[active=false] {"
            " padding: 0px;"
            "} "
            "Bar--LaserButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%1_laser_disabled.png);"
            "} "
            "Bar--LaserButton[enabled=false][active=false] {"
            " qproperty-icon: url(:/icons/%1_laser_disabled.png);"
            "} "
            "Bar--LaserButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%1_laser_active.png);"
            "} "
            "Bar--LaserButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%1_laser_inactive.png);"
            "} "
            "Bar--ExcitationButton[active=true] {"
            " qproperty-icon: url(:/icons/%1_excitation_active.png);"
            "} "
            "Bar--ExcitationButton[active=false] {"
            " qproperty-icon: url(:/icons/%1_excitation_inactive.png);"
            "} "
            "Bar--EmissionButton[active=true] {"
            " qproperty-icon: url(:/icons/%1_emission_active.png);"
            "} "
            "Bar--EmissionButton[active=false] {"
            " qproperty-icon: url(:/icons/%1_emission_inactive.png);"
            "} "
            "Bar--DetectorButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%1_detector_disabled.png);"
            "} "
            "Bar--DetectorButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%1_detector_active.png);"
            "} "
            "Bar--DetectorButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%1_detector_inactive.png);"
            "} "
            "Bar--GraphAddButton[enabled=false]{"
            " qproperty-icon: url(:/icons/%1_graph_add_disabled.png);"
            "} "
            "Bar--GraphAddButton[enabled=true] {"
            " qproperty-icon: url(:/icons/%1_graph_add_active.png);"
            "} "
            "Bar--GraphRemoveButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%1_graph_remove_disabled.png);"
            "} "
            "Bar--GraphRemoveButton[enabled=true] {"
            " qproperty-icon: url(:/icons/%1_graph_remove_active.png);"
            "} "
            "Bar--LasersButton[enabled=false] {"
            " qproperty-icon: url(:/icons/%1_lasers_disabled.png);"
            "} "
            "Bar--LasersButton[enabled=true][active=true] {"
            " qproperty-icon: url(:/icons/%1_lasers_active.png);"
            "} "
            "Bar--LasersButton[enabled=true][active=false] {"
            " qproperty-icon: url(:/icons/%1_lasers_inactive.png);"
            "} "
            "Bar--Label {"
            " min-width: 10em;"
            "} ";

    style = style.arg(this->icons);
    return(style);
}

/*
Builds the stylesheet for laser_menu
    :returns: stylesheet
*/
QString Builder::buildLaserMenu() const {
    // The scrollbar needs to be the same width as the buttons
    // This is effectively 0.5em + 3px (= margin correction)
    // Margin correction is necessary as Qt's width is the content+padding+border+margin
    QFontMetrics font_metric = QFontMetrics(static_cast<QApplication*>(QApplication::instance())->font());
    double width_m = static_cast<double>(font_metric.width('M'));
    double scrollbar_margin = (3.0 / width_m) + 0.3;
    
    QString style;
    style = "Laser--PushButton {"
            " min-width: 10em;"
            "} "
            "Laser--LineEdit {"
            " max-height: 1em;"
            "} "
            "Laser--Popup .QWidget {"
            " background: %1;"
            "} "
            "Laser--Popup QScrollBar::vertical {"
            " margin: 0px 0px 0px 3px;"
            " width: %2em;"
            "} ";
    
    style = style.arg(
        this->lasermenu_popup,
        QString::number(scrollbar_margin, 'f', 3)
    );
    return(style);
}

/*
Builds the stylesheet for the fluor_menu
    :returns: stylesheet
*/
QString Builder::buildFluorMenu() const {
    // The scrollbar needs to be the same width as the buttons
    // This is effectively 0.5em + 3px (= margin correction)
    // Margin correction is necessary as Qt's width is the content+padding+border+margin
    QFontMetrics font_metric = QFontMetrics(static_cast<QApplication*>(QApplication::instance())->font());
    double width_m = static_cast<double>(font_metric.width('M'));
    double scrollbar_margin = (3.0 / width_m) + 0.3;

    QString style;
    style = "Fluor--PushButton {"
            " min-width: 10em;"
            "} "
            "Fluor--EmissionButton {"
            " text-align: left;"
            "} "
            "Fluor--ExcitationButton {"
            " padding: 6px 0px 6px 0px;"
            " width: 0.3em;"
            "} "
            "Fluor--RemoveButton[active=true] {"
            " background-color: %1;"
            " padding: 6px 0px 6px 0px;"
            " width: 0.3em;"
            "} "
            "Fluor--RemoveButton::hover[active=true] {"
            " background-color: %2;"
            "} "
            "Fluor--RemoveButton::pressed::hover[active=true] {"
            " background-color: %3;"
            "} "
            "Fluor--LineEdit {"
            " max-height: 1em;"
            "} "
            "Fluor--Popup .QWidget {"
            " background: %4;"
            "} "
            "Fluor--ScrollController .QWidget {"
            " background: %5;"
            "} "
            "Fluor--Popup QScrollBar::vertical {"
            " margin: 0px 0px 0px 3px;"
            " min-width: %6em;"
            "} "
            "Fluor--ScrollController QScrollBar::vertical {"
            " margin: 0px 0px 0px 3px;"
            " min-width: %6em;"
            "} ";
    
    style = style.arg(
        this->fluormenu_remove,
        this->fluormenu_remove_hover,
        this->fluormenu_remove_press,
        this->fluormenu_popup,
        this->fluormenu_background,
        QString::number(scrollbar_margin, 'f', 3)
    );
    return(style);
}

/*
Builds the stylesheet for graph
    :returns: stylesheet
*/
QString Builder::buildGraph() const {
    QString style;
    style = "Graph--ScrollArea .QWidget {"
            " background: %1;"
            "} "
            "Graph--ScrollArea QScrollBar::vertical {"
            " margin: 0px 0px 0px 5px;"
            " width: 20px;"
            "} ";
    
    style = style.arg(
        this->graph_background
    );
    return(style);
}

} // Style namespace

} // Data namespace
