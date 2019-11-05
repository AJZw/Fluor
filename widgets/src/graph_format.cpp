/**** General **************************************************************
** Version:    v0.9.4
** Date:       2019-07-24
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_format.h"
#include "application.h"

#include <QWindow>
#include <QScreen>
#include <QDebug>

namespace Graph {

namespace Format {

/*
Constructor for Axis object
    :param min: absolute minimum value
    :param max: absolute maximum value, expected to be larger then min
*/
Axis::Axis(int min, int max, QString label) :
    min(min),
    max(max),
    label(label)
{}

/*
Constructor for AxisRange object
    :param begin: the axis visible begin value, expected to be equal or bigger then the equivalent Axis.min value, and smaller then Axis.max value
    :param end: the axis visible end value, expected to be equal or smaller then the equivalent Axis.max value, and bigger then Axis.min value
*/
AxisRange::AxisRange(int begin, int end) :
    begin(begin),
    end(end),
    default_begin(begin),
    default_end(end)
{}

/*
Constructor for Tick object, will not add a label for this Tick
    :param location: the tick mark location
*/
Tick::Tick(int location) :
    location(location),
    label()
{}

/*
Constructor for Tick object, will add a label for this Tick
    :param location: tick mark location
    :param label: tick label
*/
Tick::Tick(int location, QString label) :
    location(location),
    label(label)
{}

/*
Find the indexes which fit in between the begin and end value (if any)
    :param begin: lowest boundary
    :param end: highest boundary
*/
template<std::size_t TICK_COUNT>
void Ticks<TICK_COUNT>::findIndexes(int begin, int end){
    if(TICK_COUNT <= 0){
        return;
    }
    
    // Check is begin and end are reversed (for Y-axis)
    if(begin > end){
        // Reversed order, so swap begin and end values
        std::swap(begin, end);
    }

    // Check if the value are within bounds of the array
    if(end < this->ticks[0].location || begin > this->ticks[this->ticks.size() -1].location){
        this->valid = false;
        return;
    }

    // Valid region so get indexes
    std::size_t index_begin = 0;
    for(std::size_t i=0; i<this->ticks.size(); ++i){
        if(this->ticks[i].location >= begin){
            index_begin = i;
            break;
        }
    }

    // std::size_t can be unsigned, so cannot be smaller then 0, so use index + 1 for iteration
    std::size_t index_end = 0;
    for(std::size_t i=this->ticks.size(); i>0; --i){
        if(this->ticks[i-1].location <= end){
            index_end = i;
            break;
        }
    }

    this->valid = true;
    this->index_begin = index_begin;
    this->index_end = index_end;

}

/*
Constructor: attributes are taken care of in header file, just to set correct state ticks
*/
Settings::Settings(){
    this->x_ticks.findIndexes(this->x_range.begin, this->x_range.end);
    this->y_ticks.findIndexes(this->y_range.begin, this->y_range.end);
}

// ################################################################################## //

/*
Constructor
    :param parent: parent object
*/
Style::Style(QWidget* parent) :
    QWidget(parent),
    style_scene("#FFFFFF"),
    style_background("#FFFFFF"),
    style_background_hover("#FFFFFF"),
    style_background_press("#FFFFFF"),
    style_label("#FFFFFF"),
    style_label_weight(QFont::Normal),
    style_grid_label("#FFFFFF"),
    style_grid_label_weight(QFont::Normal),
    style_axis("#FFFFFF"),
    style_axis_hover("#FFFFFF"),
    style_axis_press("#FFFFFF"),
    absorption_width(1),
    absorption_style(Qt::DashDotLine),
    excitation_width(1),
    excitation_style(Qt::DashDotLine),
    emission_width(1),
    emission_style(Qt::DashDotLine)
{
    this->installEventFilter(this);
}

bool Style::eventFilter(QObject* obj, QEvent* event){
    Q_UNUSED(obj);

    switch(event->type()){
    case QEvent::StyleChange:
    case QEvent::DynamicPropertyChange:
        emit this->styleChanged();
        return true;
    default:
        return false;
    }
    return false;
}

QString Style::scene() const {
    return this->style_scene.name(QColor::HexRgb);
}
void Style::setScene(QString scene) {
    this->style_scene = QColor(scene);
}

QString Style::background() const {
    return this->style_background.name(QColor::HexRgb);
}
void Style::setBackground(QString background){
    this->style_background = QColor(background);
}

QString Style::backgroundHover() const {
    return this->style_background_hover.name(QColor::HexRgb);
}
void Style::setBackgroundHover(QString background_hover){
    this->style_background_hover = QColor(background_hover);
}

QString Style::backgroundPress() const {
    return this->style_background_press.name(QColor::HexRgb);
}
void Style::setBackgroundPress(QString background_press){
    this->style_background_press = QColor(background_press);
}

QString Style::label() const {
    return this->style_label.name(QColor::HexRgb);
}
void Style::setLabel(QString label) {
    this->style_label = QColor(label);
}

QString Style::labelWeight() const {
    return Style::fontWeightToText(this->style_label_weight);
}
void Style::setLabelWeight(QString label_weight){
    this->style_label_weight = Style::textToFontWeight(label_weight, QFont::Normal);
}

QString Style::gridLabel() const {
    return this->style_grid_label.name(QColor::HexRgb);
}
void Style::setGridLabel(QString grid_label){
    this->style_grid_label = QColor(grid_label);
}

QString Style::gridLabelWeight() const {
    return Style::fontWeightToText(this->style_grid_label_weight);
}
void Style::setGridLabelWeight(QString grid_label_weight){
    this->style_grid_label_weight = Style::textToFontWeight(grid_label_weight, QFont::Normal);
}

QString Style::axis() const {
    return this->style_axis.name(QColor::HexRgb);
}
void Style::setAxis(QString axis){
    this->style_axis = QColor(axis);
}

QString Style::axisHover() const {
    return this->style_axis_hover.name(QColor::HexRgb);
}
void Style::setAxisHover(QString axis_hover){
    this->style_axis_hover = QColor(axis_hover);
}

QString Style::axisPress() const {
    return this->style_axis_press.name(QColor::HexRgb);
}
void Style::setAxisPress(QString axis_press){
    this->style_axis_press = QColor(axis_press);
}

QString Style::absorptionWidth() const{
    return QString::number(this->absorption_width, 'f', 0);
}
void Style::setAbsorptionWidth(QString absorption_width){
    this->absorption_width = absorption_width.toInt();
}

QString Style::absorptionStyle() const {
    return Style::penStyleToText(this->absorption_style);
}
void Style::setAbsorptionStyle(QString absorption_style){
    this->absorption_style = Style::textToPenStyle(absorption_style, Qt::DotLine);
}

QString Style::excitationWidth() const {
    return QString::number(this->excitation_width, 'f', 0);
}
void Style::setExcitationWidth(QString excitation_width){
    this->excitation_width = excitation_width.toInt();
}

QString Style::excitationStyle() const {
    return Style::penStyleToText(this->excitation_style);
}
void Style::setExcitationStyle(QString excitation_style){
    this->excitation_style = Style::textToPenStyle(excitation_style, Qt::DashLine);
}

QString Style::emissionWidth() const{
    return QString::number(this->emission_width, 'f', 0);
}
void Style::setEmissionWidth(QString emission_width){
    this->emission_width = emission_width.toInt();
}

QString Style::emissionStyle() const {
    return Style::penStyleToText(this->emission_style);
}
void Style::setEmissionStyle(QString emission_style){
    this->emission_style = Style::textToPenStyle(emission_style, Qt::SolidLine);
}

/*
Tries to tranforms the text into a QFont::Weight enum. If transform is not possible returns fallback
    :param text: input
    :param fallback: value to return if input transform fails
*/
QFont::Weight Style::textToFontWeight(const QString& text, QFont::Weight fallback){
    if(text == "light"){
        return QFont::Light;
    }else if(text == "normal"){
        return QFont::Normal;
    }else if(text == "demibold"){
        return QFont::DemiBold;
    }else if(text == "bold"){
        return QFont::Bold;
    }else if(text == "black"){
        return QFont::Black;
    }else{
        qWarning() << "Graph::Format::Style::textToFontWeight: cannot convert text into QFont::Weight" << text;
        return fallback;
    }
}

/*
Tries to transform a QFont::Weight enum into text
    :param weight: input
*/
QString Style::fontWeightToText(QFont::Weight weight){
    switch(weight){
    case QFont::Light:
        return QString("light");
    case QFont::Normal:
        return QString("normal");
    case QFont::DemiBold:
        return QString("demibold");
    case QFont::Bold:
        return QString("bold");
    case QFont::Black:
        return QString("black");
    default:
        qWarning() << "Graph::Format::Style::fontWeightToText: unknown QFont::Weight enum" << weight;
        return QString("normal");
    }
}

/*
Tries to tranforms the text into a Qt::PenStyle enum. If transform is not possible returns fallback
    :param text: input
    :param fallback: value to return if input transform fails
*/
Qt::PenStyle Style::textToPenStyle(const QString& text, Qt::PenStyle fallback){
    if(text == "solid"){
        return Qt::SolidLine;
    }else if(text == "dash"){
        return Qt::DashLine;
    }else if(text == "dot"){
        return Qt::DotLine;
    }else if(text == "dashdot"){
        return Qt::DashDotLine;
    }else if(text == "dashdotdot"){
        return Qt::DashDotDotLine;
    }else{
        qWarning() << "Graph::Format::Style::textToPenStyle: cannot convert text into Qt::PenStyle" << text;
        return fallback;
    }
}

/*
Tries to transform a Qt::PenStylet enum into text
    :param style: input
*/
QString Style::penStyleToText(Qt::PenStyle style){
    switch(style){
    case Qt::SolidLine:
        return QString("solid");
    case Qt::DashLine:
        return QString("dash");
    case Qt::DotLine:
        return QString("dot");
    case Qt::DashDotLine:
        return QString("dashdot");
    case Qt::DashDotDotLine:
        return QString("dashdotdot");
    default:
        qWarning() << "Graph::Format::Style::penStyleToText: unconvertable Qt::PenStyle enum" << style;
        return QString("normal");
    }
}

/*
Construct and returns a brush for the Graph::GraphicsScene
*/
QBrush Style::brushScene() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_scene);
    return brush;
}

/*
Construct and returns a brush for the Graph::Label
*/
QBrush Style::brushLabel() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_label);
    return brush;
}

/*
Construct and returns a brush for the Graph::GridLabel
*/
QBrush Style::brushGridLabel() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_grid_label);
    return brush;
}

/*
Construct and returns a brush for the Graph::Background
*/
QBrush Style::brushBackground() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_background);
    return brush;
}

/*
Construct and returns a brush for the Graph::Background in mouse hover state
*/
QBrush Style::brushBackgroundHover() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_background_hover);
    return brush;
}

/*
Construct and returns a brush for the Graph::Background in mouse press state
*/
QBrush Style::brushBackgroundPress() const {
    QBrush brush(Qt::SolidPattern);
    brush.setColor(this->style_background_press);
    return brush;
}

/*
Construct and returns a brush for the Graph::Spectrum emission plot
*/
QBrush Style::brushEmission(QColor color) const {
    color.setAlpha(75);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(color);
    return brush;
}

/*
Construct and returns a brush for the selected Graph::Spectrum emission plot
*/
QBrush Style::brushEmissionSelect(QColor color) const {
    color.setAlpha(170);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(color);
    return brush;
}

/*
Constructs and returns a font for the Graph::Label
*/
QFont Style::fontLabel() const {
    QFont font(this->window()->font(), this->window());
    font.setWeight(this->style_label_weight);
    return font;
}

/*
Constructs and returns a font for the Graph::GridLabel 
*/
QFont Style::fontGridLabel() const {
    QFont font(this->window()->font(), this->window());
    font.setWeight(this->style_grid_label_weight);
    return font;
}

/*
Constructs and returns a pen for the Graph::GridLines, Graph::TickLines, Graph::Outline, or Graph::Colorbar
*/
QPen Style::penAxis() const {
    QPen pen(Qt::SolidLine);
    pen.setWidth(1);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(this->style_axis);
    return pen;
}

/*
Constructs and returns a pen for the Graph::TickLines, Graph::Outline, or Graph::Colorbar in hover state
*/
QPen Style::penAxisHover() const {
    QPen pen(Qt::SolidLine);
    pen.setWidth(1);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(this->style_axis_hover);
    return pen;
}

/*
Constructs and returns a pen for the Graph::TickLines, Graph::Outline, or Graph::Colorbar in press state
*/
QPen Style::penAxisPress() const {
    QPen pen(Qt::SolidLine);
    pen.setWidth(1);
    pen.setCapStyle(Qt::SquareCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(this->style_axis_press);
    return pen;
}

/*
Constructs and returns a pen for the Graph::Spectrum absorption curve
*/
QPen Style::penAbsorption(QColor color) const {
    color.setAlpha(170);
    
    QPen pen(this->absorption_style);
    pen.setWidth(this->absorption_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

/*
Constructs and returns a pen for the Graph::Spectrum excitation curve
*/
QPen Style::penExcitation(QColor color) const {
    color.setAlpha(170);

    QPen pen(this->excitation_style);
    pen.setWidth(this->excitation_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

/*
Constructs and returns a pen for the Graph::Spectrum emission curve
*/
QPen Style::penEmission(QColor color) const {
    color.setAlpha(170);
    
    QPen pen(this->emission_style);
    pen.setWidth(this->emission_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

/*
Constructs and returns a pen for the selected Graph::Spectrum absorption curve 
*/
QPen Style::penAbsorptionSelect(QColor color) const {
    color.setAlpha(255);
    
    QPen pen(this->absorption_style);
    pen.setWidth(this->absorption_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

/*
Constructs and returns a pen for the selected Graph::Spectrum excitation curve
*/
QPen Style::penExcitationSelect(QColor color) const {
    color.setAlpha(255);

    QPen pen(this->excitation_style);
    pen.setWidth(this->excitation_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

/*
Constructs and returns a pen for the selected Graph::Spectrum emission curve
*/
QPen Style::penEmissionSelect(QColor color) const {
    color.setAlpha(255);
    
    QPen pen(this->emission_style);
    pen.setWidth(this->emission_width);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    pen.setColor(color);
    return pen;
}

} // Format namespace

} // Graph namespace
