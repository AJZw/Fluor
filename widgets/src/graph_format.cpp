/**** General **************************************************************
** Version:    v0.10.1
** Date:       2020-11-16
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2022 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "graph_format.h"

#include <QDebug>
#include <QScreen>
#include <QWindow>

#include "application.h"

namespace Graph {

namespace Format {

/*
Constructor for Axis object
  :param min: absolute minimum value
  :param max: absolute maximum value, expected to be larger then min
*/
Axis::Axis(double min, double max, QString label) : min(min), max(max), label(label) {}

/*
Constructor for AxisRange object
  :param begin: the axis visible begin value, expected to be equal or bigger then the equivalent Axis.min value, and smaller then Axis.max
value :param end: the axis visible end value, expected to be equal or smaller then the equivalent Axis.max value, and bigger then Axis.min
value
*/
AxisRange::AxisRange(double begin, double end) : begin(begin), end(end), default_begin(begin), default_end(end) {}

/*
Constructor for Tick object, will not add a label for this Tick
  :param location: the tick mark location
*/
Tick::Tick(double location) : location(location), label() {}

/*
Constructor for Tick object, will add a label for this Tick
  :param location: tick mark location
  :param label: tick label
*/
Tick::Tick(double location, QString label) : location(location), label(label) {}

/*
Find the indexes which fit in between the begin and end value (if any)
  :param begin: lowest boundary
  :param end: highest boundary
*/
template <std::size_t TICK_COUNT>
void Ticks<TICK_COUNT>::findIndexes(double begin, double end) {
  if (TICK_COUNT <= 0) {
    return;
  }

  // depending on order, different search strategy has to be employed
  std::size_t index_begin = 0;
  std::size_t index_end = 0;
  if (begin <= end) {
    // Check if the value are within bounds of the array
    if (end < this->ticks[0].location || begin > this->ticks[this->ticks.size() - 1].location) {
      this->valid = false;
      return;
    }

    // Valid region so get indexes
    for (std::size_t i = 0; i < this->ticks.size(); ++i) {
      if (this->ticks[i].location >= begin) {
        index_begin = i;
        break;
      }
    }

    // std::size_t can be unsigned, so cannot be smaller then 0, so use i-1 for iteration
    for (std::size_t i = this->ticks.size(); i > 0; --i) {
      if (this->ticks[i - 1].location <= end) {
        index_end = i;
        break;
      }
    }
  } else {
    // Check if the value are within bounds of the array
    if (end > this->ticks[0].location || begin < this->ticks[this->ticks.size() - 1].location) {
      this->valid = false;
      return;
    }

    // Valid region so get indexes
    for (std::size_t i = 0; i < this->ticks.size(); ++i) {
      if (this->ticks[i].location <= begin) {
        index_begin = i;
        break;
      }
    }

    // std::size_t can be unsigned, so cannot be smaller then 0, so use i-1 for iteration
    for (std::size_t i = this->ticks.size(); i > 0; --i) {
      if (this->ticks[i - 1].location >= end) {
        index_end = i;
        break;
      }
    }
  }

  this->valid = true;
  this->index_begin = index_begin;
  this->index_end = index_end;
}

// ################################################################################## //

/*
Constructor: attributes are taken care of in header file, just to set correct state ticks
*/
Settings::Settings() {
  this->x_ticks.findIndexes(this->x_range.begin, this->x_range.end);
  this->y_ticks.findIndexes(this->y_range.begin, this->y_range.end);
}

/*
Updates the internal state of settings to apply any settings data change
*/
void Settings::update() {
  this->x_ticks.findIndexes(this->x_range.begin, this->x_range.end);
  this->y_ticks.findIndexes(this->y_range.begin, this->y_range.end);
}

// ################################################################################## //

/*
Constructor
  :param parent: parent object
*/
Style::Style(QWidget* parent)
    : QWidget(parent),
      style_scene("#ffffff"),
      style_background("#ffffff"),
      style_background_hover("#ffffff"),
      style_background_press("#ffffff"),
      style_axis("#ffffff"),
      style_axis_hover("#ffffff"),
      style_axis_press("#ffffff"),
      style_label("#ffffff"),
      style_label_weight(QFont::Normal),
      style_grid("#ffffff"),
      style_grid_label("#ffffff"),
      style_grid_label_weight(QFont::Normal),
      filter_width(2),
      style_filter("#000000"),
      absorption_width(1),
      absorption_style(Qt::DashDotLine),
      excitation_width(1),
      excitation_style(Qt::DashDotLine),
      emission_width(1),
      emission_style(Qt::DashDotLine),
      colorbar_height(10) {
  // I connect it to the Graph::Controller for lifetime management and event inheritance
  // But it should not be plotted
  this->setVisible(false);

  this->installEventFilter(this);
}

bool Style::eventFilter(QObject* obj, QEvent* event) {
  Q_UNUSED(obj);

  switch (event->type()) {
    case QEvent::StyleChange:
    case QEvent::DynamicPropertyChange:
      emit this->styleChanged();
      return true;
    default:
      return false;
  }
  return false;
}

QString Style::scene() const { return this->style_scene.name(QColor::HexRgb); }
void Style::setScene(QString scene) { this->style_scene = QColor(scene); }

QString Style::background() const { return this->style_background.name(QColor::HexRgb); }
void Style::setBackground(QString background) { this->style_background = QColor(background); }

QString Style::backgroundHover() const { return this->style_background_hover.name(QColor::HexRgb); }
void Style::setBackgroundHover(QString background_hover) { this->style_background_hover = QColor(background_hover); }

QString Style::backgroundPress() const { return this->style_background_press.name(QColor::HexRgb); }
void Style::setBackgroundPress(QString background_press) { this->style_background_press = QColor(background_press); }

QString Style::axis() const { return this->style_axis.name(QColor::HexRgb); }
void Style::setAxis(QString axis) { this->style_axis = QColor(axis); }

QString Style::axisHover() const { return this->style_axis_hover.name(QColor::HexRgb); }
void Style::setAxisHover(QString axis_hover) { this->style_axis_hover = QColor(axis_hover); }

QString Style::axisPress() const { return this->style_axis_press.name(QColor::HexRgb); }
void Style::setAxisPress(QString axis_press) { this->style_axis_press = QColor(axis_press); }

QString Style::label() const { return this->style_label.name(QColor::HexRgb); }
void Style::setLabel(QString label) { this->style_label = QColor(label); }

QString Style::labelWeight() const { return Style::fontWeightToText(this->style_label_weight); }
void Style::setLabelWeight(QString label_weight) { this->style_label_weight = Style::textToFontWeight(label_weight, QFont::Normal); }

QString Style::grid() const { return this->style_grid.name(QColor::HexRgb); }
void Style::setGrid(QString grid) { this->style_grid = QColor(grid); }

QString Style::gridLabel() const { return this->style_grid_label.name(QColor::HexRgb); }
void Style::setGridLabel(QString grid_label) { this->style_grid_label = QColor(grid_label); }

QString Style::gridLabelWeight() const { return Style::fontWeightToText(this->style_grid_label_weight); }
void Style::setGridLabelWeight(QString grid_label_weight) {
  this->style_grid_label_weight = Style::textToFontWeight(grid_label_weight, QFont::Normal);
}

QString Style::filter() const { return this->style_filter.name(QColor::HexRgb); }
void Style::setFilter(QString filter) { this->style_filter = QColor(filter); }

QString Style::filterWidth() const { return QString::number(this->filter_width, 'f', 0); }
void Style::setFilterWidth(QString filter_width) { this->filter_width = filter_width.toInt(); }

QString Style::absorptionWidth() const { return QString::number(this->absorption_width, 'f', 0); }
void Style::setAbsorptionWidth(QString absorption_width) { this->absorption_width = absorption_width.toInt(); }

QString Style::absorptionStyle() const { return Style::penStyleToText(this->absorption_style); }
void Style::setAbsorptionStyle(QString absorption_style) { this->absorption_style = Style::textToPenStyle(absorption_style, Qt::DotLine); }

QString Style::excitationWidth() const { return QString::number(this->excitation_width, 'f', 0); }
void Style::setExcitationWidth(QString excitation_width) { this->excitation_width = excitation_width.toInt(); }

QString Style::excitationStyle() const { return Style::penStyleToText(this->excitation_style); }
void Style::setExcitationStyle(QString excitation_style) { this->excitation_style = Style::textToPenStyle(excitation_style, Qt::DashLine); }

QString Style::emissionWidth() const { return QString::number(this->emission_width, 'f', 0); }
void Style::setEmissionWidth(QString emission_width) { this->emission_width = emission_width.toInt(); }

QString Style::emissionStyle() const { return Style::penStyleToText(this->emission_style); }
void Style::setEmissionStyle(QString emission_style) { this->emission_style = Style::textToPenStyle(emission_style, Qt::SolidLine); }

QString Style::colorbarHeight() const { return QString::number(this->colorbar_height, 'f', 0); }
void Style::setColorbarHeight(QString height) { this->colorbar_height = height.toInt(); }

/*
Tries to tranforms the text into a QFont::Weight enum. If transform is not possible returns fallback
  :param text: input
  :param fallback: value to return if input transform fails
*/
QFont::Weight Style::textToFontWeight(const QString& text, QFont::Weight fallback) {
  if (text == "light") {
    return QFont::Light;
  } else if (text == "normal") {
    return QFont::Normal;
  } else if (text == "demibold") {
    return QFont::DemiBold;
  } else if (text == "bold") {
    return QFont::Bold;
  } else if (text == "black") {
    return QFont::Black;
  } else {
    qWarning() << "Graph::Format::Style::textToFontWeight: cannot convert text into QFont::Weight" << text;
    return fallback;
  }
}

/*
Tries to transform a QFont::Weight enum into text
  :param weight: input
*/
QString Style::fontWeightToText(QFont::Weight weight) {
  switch (weight) {
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
Qt::PenStyle Style::textToPenStyle(const QString& text, Qt::PenStyle fallback) {
  if (text == "solid") {
    return Qt::SolidLine;
  } else if (text == "dash") {
    return Qt::DashLine;
  } else if (text == "dot") {
    return Qt::DotLine;
  } else if (text == "dashdot") {
    return Qt::DashDotLine;
  } else if (text == "dashdotdot") {
    return Qt::DashDotDotLine;
  } else {
    qWarning() << "Graph::Format::Style::textToPenStyle: cannot convert text into Qt::PenStyle" << text;
    return fallback;
  }
}

/*
Tries to transform a Qt::PenStylet enum into text
  :param style: input
*/
QString Style::penStyleToText(Qt::PenStyle style) {
  switch (style) {
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
Constructs and returns a pen for the Graph::TickLines, Graph::Outline, or Graph::Colorbar
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
Constructs and returns a pen for the Graph::GridLines
*/
QPen Style::penGrid() const {
  QPen pen(Qt::SolidLine);
  pen.setWidth(1);
  pen.setCapStyle(Qt::SquareCap);
  pen.setJoinStyle(Qt::MiterJoin);
  pen.setColor(this->style_grid);
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
  color.setAlpha(215);

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
  color.setAlpha(215);

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
  color.setAlpha(215);

  QPen pen(this->emission_style);
  pen.setWidth(this->emission_width);
  pen.setCapStyle(Qt::FlatCap);
  pen.setJoinStyle(Qt::MiterJoin);
  pen.setColor(color);
  return pen;
}

/*
Constructs and returns a pen for a Graph::Laser object
  :param color: the color of the pen
*/
QPen Style::penLaser(QColor color) const {
  QPen pen(Qt::SolidLine);
  pen.setWidth(2);
  pen.setCapStyle(Qt::SquareCap);
  pen.setJoinStyle(Qt::MiterJoin);
  pen.setColor(color);
  return pen;
}

/*
Constructs and returns a pen for a Graph::Detector object
  :param line_style: the linestyle of the pen
*/
QPen Style::penFilter(Qt::PenStyle line_style) const {
  QPen pen(line_style);
  pen.setWidth(this->filter_width);
  pen.setCapStyle(Qt::FlatCap);  // A SquareCap will cause line overlaps in the Graph::Filter painting
  pen.setJoinStyle(Qt::MiterJoin);
  pen.setColor(this->style_filter);
  return pen;
}

/*
Returns the stylized height of the colorbar
*/
int Style::heightColorbar() const { return this->colorbar_height; }

}  // namespace Format

// ################################################################################## //

/*
Constructor: Construct a default PlotRectF
*/
PlotRectF::PlotRectF() : margins_settings(2, 2, 3, 2), rect_local(0, 0, 0, 0), rect_settings(0, 0, 0, 0), rect_global(0, 0, 0, 0) {}

/*
Constructor: Construct a PlotRectF with the supplied settings and margins.
*/
PlotRectF::PlotRectF(const QRectF& settings, const QMargins& margins)
    : margins_settings(margins), rect_local(0, 0, 0, 0), rect_settings(settings), rect_global(0, 0, 0, 0) {
  this->calculate();
}

/*
Getter. Returns the margins applied to the settings.
*/
const QMargins& PlotRectF::margins() const { return this->margins_settings; }

/*
Setter. Sets the margins that are applied to the settings to construct the global rectangle
*/
void PlotRectF::setMargins(const QMargins& margins) {
  this->margins_settings = margins;

  this->calculate();
}

/*
Getter. Returns a reference to the local plotting rectangle. Use this rectangle to find the
scenes plotting region
*/
const QRectF& PlotRectF::local() const { return this->rect_local; }

/*
Getter. Returns a reference to the settings rectangle. This is the global rectangle before margins adjustment.
*/
const QRectF& PlotRectF::settings() const { return this->rect_settings; }

/*
Getter. Returns a reference to the global rectangle. Use this rectangle to find the represented
x and y values of the plot
*/
const QRectF& PlotRectF::global() const { return this->rect_global; }

/*
Sets a new local rectangle. This forces recalculation of the toLocal/toGlobal functions
  :param rect: the new local rectangle
*/
void PlotRectF::setLocal(const QRectF& rect) {
  this->rect_local = rect;

  this->calculate();
}

/*
Sets a new global rectangle. This forces recalculation of the toLocal/toGlobal functions
  :param rect: the new global rectangle
*/
void PlotRectF::setSettings(const QRectF& rect) {
  this->rect_settings = rect;

  // Calculate to construct rect_global
  this->calculate();
}

/*
Transforms the global X value into the local equivalent
*/
double PlotRectF::toLocalX(double global) const {
  global *= this->x_slope_global_to_local;
  global += this->x_intercept;
  return global;
}

/*
Transform the local X value into the global equivalent
*/
double PlotRectF::toGlobalX(double local) const {
  local -= this->x_intercept;
  local *= this->x_slope_local_to_global;
  return local;
}

/*
Transforms the global Y value into the local equivalent
  :param global: the global Y value (0-100)
  :param intensity: the intensity scaling (0.0-1.0)
*/
double PlotRectF::toLocalY(double global, double intensity) const {
  global *= intensity;
  global *= this->y_slope_global_to_local;
  global += this->y_intercept;
  return global;
}

/*
Transform the local Y value into the global equivalent
  :param global: the local Y value (0-100)
  :param intensity: the intensity scaling (0.0-1.0)
*/
double PlotRectF::toGlobalY(double local, double intensity) const {
  local -= this->y_intercept;
  local *= this->y_slope_local_to_global;
  local /= intensity;
  return local;
}

/*
Returns the function that transforms the global X value into the local X value
*/
std::function<double(double)> PlotRectF::toLocalXFunction() const {
  double slope = this->x_slope_global_to_local;
  double intercept = this->x_intercept;

  auto lambda = [slope, intercept](double global) {
    global *= slope;
    global += intercept;
    return global;
  };

  return lambda;
}

/*
Returns the function that transforms the local X value into the global X value
*/
std::function<double(double)> PlotRectF::toGlobalXFunction() const {
  double slope = this->x_slope_local_to_global;
  double intercept = this->x_intercept;

  std::function<double(double)> functor = [slope, intercept](double local) {
    local *= slope;
    local += intercept;
    return local;
  };
  return functor;
}

/*
Returns the function that transforms the global Y value into the local Y value
*/
std::function<double(double, double)> PlotRectF::toLocalYFunction() const {
  double slope = this->y_slope_global_to_local;
  double intercept = this->y_intercept;

  std::function<double(double, double)> functor = [slope, intercept](double global, double intensity) {
    global *= intensity;
    global *= slope;
    global += intercept;
    return global;
  };
  return functor;
}

/*
Returns the function that transforms the local Y value into the global Y value
*/
std::function<double(double, double)> PlotRectF::toGlobalYFunction() const {
  double slope = this->y_slope_global_to_local;
  double intercept = this->y_intercept;

  std::function<double(double, double)> functor = [slope, intercept](double local, double intensity) {
    local -= intercept;
    local *= slope;
    local /= intensity;
    return local;
  };
  return functor;
}

/*
Calculates the linear transformation equations and stores the slope
*/
void PlotRectF::calculate() {
  // Keep in mind that a QRect 0x0 coordinate is positioned in the topleft!

  // Calculate X-axis parameters
  this->x_slope_global_to_local =
      (this->rect_local.width() - this->margins_settings.left() - this->margins_settings.right()) / this->rect_settings.width();
  this->x_slope_local_to_global =
      1 / this->x_slope_global_to_local;  // this->x_slope_local_to_global = this->rect_settings.width() / (this->rect_local.width() -
                                          // this->margins_settings.left() - this->margins_settings.right());

  double global_left = this->rect_settings.left() - (this->x_slope_local_to_global * this->margins_settings.left());
  double global_right = this->rect_settings.right() + (this->x_slope_local_to_global * this->margins_settings.right());

  this->x_intercept = this->rect_local.left() - (global_left * this->x_slope_global_to_local);

  // Calculate Y-axis parameters.
  this->y_slope_global_to_local =
      (this->rect_local.height() - this->margins_settings.top() - this->margins_settings.bottom()) / this->rect_settings.height();
  this->y_slope_local_to_global =
      1 / this->y_slope_global_to_local;  // this->y_slope_local_to_global = this->rect_settings.height() / (this->rect_local.height() -
                                          // this->margins_settings.top() - this->margins_settings.bottom());

  double global_top = this->rect_settings.top() - (this->y_slope_local_to_global * this->margins_settings.top());
  double global_bottom = this->rect_settings.bottom() + (this->y_slope_local_to_global * this->margins_settings.bottom());

  this->y_intercept = this->rect_local.top() - (global_top * this->y_slope_global_to_local);

  this->rect_global = QRectF(QPointF(global_left, global_top), QPointF(global_right, global_bottom));
}

}  // namespace Graph
