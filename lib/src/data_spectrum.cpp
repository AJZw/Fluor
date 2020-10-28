/**** General **************************************************************
** Version:    v0.9.12
** Date:       2020-10-28
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_spectrum.h"

namespace Data {

/*
Constructor: Construct a Meta object with default values of 0
*/
Meta::Meta() :
    excitation_max(-1), emission_max(-1)
{}

/*
Constructor: Construct a Meta object
    :param excitation_max: excitation max wavelength 
    :param emission_max: emission max wavelength 
*/
Meta::Meta(double excitation_max, double emission_max) :
    excitation_max(excitation_max), emission_max(emission_max)
{}

/* ######################################################################################### */

/*
Constructor: Construct an empty Polygon object with a 0-1 curve
*/
Polygon::Polygon() :
    x_min(-1.0),
    x_max(-1.0),
    y_min(-1.0),
    y_max(-1.0),
    curve_color(0, 0, 0),
    curve(2)
{
    // Build a mock curve to always have 'valid' data for calculations even if Polygon is empty
    this->curve << QPointF(0, 0) << QPointF(1, 1);
}

/*
Constructor: Construct a polygon object
    :param x_min: global scale x[0] value in wavelength nanometers
    :param x_max: global scale x[-1] value in wavelength nanometers
    :param y_min: global scale y[0] value in intensity percentage
    :param y_max: global scale y[-1] value in intensity percentage
*/
Polygon::Polygon(double x_min, double x_max, double y_min, double y_max, QPolygonF curve) :
    x_min(x_min),
    x_max(x_max),
    y_min(y_min),
    y_max(y_max),
    curve_color(0, 0, 0),
    curve(curve)
{}

/*
Copy constructor: copy construct a polygon object.
Necessary cause the internal QVector<QPointF> of QPolygonF defaults to shallow copies. As polygon objects are copied for plotting they will always be modified.
I would like to incur this performance hit upon curve adding and upon the first draw.
*/
Polygon::Polygon(const Polygon& other) :
    x_min(other.x_min),
    x_max(other.x_max),
    y_min(other.y_min),
    y_max(other.y_max),
    curve_color(other.curve_color),
    curve(other.curve)
{
    // Detach to make deepcopy
    this->curve.detach();
}

/*
Assignment operator. Deepcopies the QPolygonF curve.
*/
Polygon& Polygon::operator=(const Polygon& other) {
    if(this != &other){
        this->x_min = other.x_min;
        this->x_max = other.x_max;
        this->y_min = other.y_min;
        this->y_max = other.y_max;
        this->curve_color = other.curve_color;
        this->curve = other.curve;
        // Detach to make deepcopy
        this->curve.detach();
    }
    return *this;
}

//QDebug operator<<(QDebug stream, const Polygon& object){
//    if(object.empty()){
//        return stream << "Data::Polygon{}";
//    }
//    
//    return stream << "Data::Polygon{" << object.x_min << "-" << object.x_max << ":" << object.curve[0] << "-" << object.curve[object.curve.size() - 1] << "}";
//}


/*
Checks whether polygon is empty
    :returns: whether the polygon is empty. Althought the object is empty it is always in a defined state
*/
bool Polygon::empty() const {
    if(this->x_min != -1.0 && this->x_max != -1.0 && this->y_min != -1.0 && this->y_max != -1.0){
        return false;
    }
    return true;
}

/*
Returns the intensity at the specified wavelength. Calculates the index to it and returns that wavelengt.
Assumes a linear distribution of the curve to be able to calculate the index
    :param wavelength: global wavelength in nanometers
    :returns: intensity (0.0-1.0)
*/
double Polygon::intensityAt(double wavelength, double cutoff) const {
    if(wavelength < this->x_min || wavelength > this->x_max){
        return 0.0;
    }

    double fraction = (wavelength - x_min) / (x_max - x_min);
    fraction *= (this->curve.size() - 1);
    int index = static_cast<int>(fraction);

    double intensity = this->curve[index].y();

    if(intensity <= cutoff){
        intensity = 0.0;
    }

    return intensity;
}

/*
Returns the intensity at the specified wavelength. Uses binary search to find the wavelength.
    :param wavelength: global wavelength in nanometers
    :returns: intensity (0.0-1.0)
*/
double Polygon::intensityAtIter(double wavelength, double cutoff) const {
    if(wavelength < this->x_min || wavelength > this->x_max){
        return 0.0;
    }

    double fraction = (wavelength - x_min) / (x_max - x_min);
    auto index = std::lower_bound(this->curve.cbegin(), this->curve.cend(), fraction, [](const QPointF& lhs, double rhs){return lhs.x() < rhs;});

    double intensity = index->y();

    if(intensity <= cutoff){
        intensity = 0.0;
    }

    return intensity;
}

/*
Finds the wavelength of the (first) highest y value. Technically could look for value >=1.0, but quite some uncleaned datasets dont reach that value.
    :returns: wavelength in nanometers
*/
double Polygon::intensityMax() const {
    double max = 0.0;
    int max_index = 0;

    for(int i=0; i < this->curve.size(); ++i){
        if(this->curve[i].y() > max){
            max = this->curve[i].y();
            max_index = i;
        }
    }

    // Transform back to global wavelength
    double fraction = (this->x_max - this->x_min);
    fraction *= this->curve[max_index].x();
    fraction += this->x_min;

    return fraction;
}

/*
Getter for the line color
    :returns: line color
*/
const QColor& Polygon::color() const {
    return this->curve_color;
}

/*
Setter for the line color. Calculates the max intensity and uses that to determine the color
*/
void Polygon::setColor() {
    double wavelength = this->intensityMax();
    this->setColor(wavelength);
}

/* 
Setter for the line color. Uses the supplied wavelength to determine the color
    :param wavelength: wavelength in nanometers
*/
void Polygon::setColor(double wavelength) {
    this->curve_color = Polygon::visibleSpectrum(wavelength);
}

/*
Setter for the line color. Sets supplied QColor
    :param color: line color
*/
void Polygon::setColor(QColor color) {
    this->curve_color = color;
}

/*
Getter for the curve
    :returns: the curve polygon
*/
QPolygonF& Polygon::polygon(){
    return this->curve;
}

/*
Calculates whether a point is contained within the polygon.
As the scaling is unknown, will use a binary search instead
    :param point: the point to check, in the this->curve coordinates
    :param line_width: the polygon line width size. If not applicable use 0.
    :returns: true if the point is on or below the polygon, otherwise false.
*/
bool Polygon::contains(const QPointF& point, double line_width) const{
    // Polygon is empty if it is outside of the axis
    if(this->curve.empty()){
        return false;
    }

    // Determine if point is within the curve range
    double start = this->curve[0].x();
    double end = this->curve[this->curve.size()-1].x();
    if(point.x() < start || point.x() > end){
        return false;
    }
        
    // Now determine the entree using a binary search
    auto entree_y = std::lower_bound(
        this->curve.begin(),
        this->curve.end(),
        point.x(), 
        [](const QPointF& obj_a, double obj_b){
                return obj_a.x() < obj_b;
        }
    );
    
    double height_y = entree_y->y();
    height_y -= (0.5 * line_width);

    if(point.y() >= height_y){
        return true;
    }else{
        return false;
    }
}

/*
Calculates whether a point is contained within the polygon.
Function uses the scaling functions for lookup in scaled curves
Note: this function assumes a x-stepsize of 1 after scaling (to global)
    :param point: the poin to check in local coordinates
    :param line_width: the polygon line width size. If not applicable use 0.
    :param scale_x: the local to global scaling function for the x-value
    :returns: true if the point is on or below the polygon, otherwise false.
*/
bool Polygon::contains(const QPointF& point, double line_width, std::function<double(double)> scale_x) const {
    // Polygon is empty if it is outside of the axis
    if(this->curve.empty()){
        return false;
    }

    // Determine if point is within the curve range
    double start = this->curve[0].x();
    double end = this->curve[this->curve.size()-1].x();
    if(point.x() < start || point.x() > end){
        return false;
    }

    double point_x_global = scale_x(point.x());
    double point_begin_global = scale_x(this->curve[0].x());

    // Determine entree index in 
    int index = static_cast<int>(point_x_global - point_begin_global + 0.5);
    double entree_y = this->curve[index].y();

    // Correct for line tickness
    entree_y -= (0.5 * line_width);

    if(point.y() >= entree_y){
        return true;
    }else{
        return false;
    }
}

/*
(Static) Returns the color of the maximum emission intensity, uses linear approximation
Source: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
    :param wavelength: wavelength to transform into visible RGB value
    :returns: QColor representation of the wavelength
*/
QColor Polygon::visibleSpectrum(double wavelength){
    double red, green, blue, intensity;
    if(wavelength >= 380.0 && wavelength < 440.0){
        red = -(wavelength - 440.0) / (440.0 - 380.0);
        green = 0.0;
        blue = 1.0;
    }else if(wavelength >= 440.0 && wavelength < 490.0){
        red = 0.0;
        green = (wavelength - 440.0) / (490.0 - 440.0);
        blue = 1.0;
    }else if(wavelength >= 490.0 && wavelength < 510.0){
        red = 0.0;
        green = 1.0;
        blue = -(wavelength - 510.0) / (510.0 - 490.0);
    }else if(wavelength >= 510.0 && wavelength < 580.0){
        red = (wavelength - 510.0) / (580.0 - 490.0);
        green = 1.0;
        blue = 0.0;
    }else if(wavelength >= 580.0 && wavelength < 645.0){
        red = 1.0;
        green = -(wavelength - 645.0) / (645.0 - 580.0);
        blue = 0.0;
    }else if(wavelength >= 645.0 && wavelength <= 780){
        red = 1.0;
        green = 0.0;
        blue = 0.0;
    }else{
        red = 0.0;
        green = 0.0;
        blue = 0.0;
    }

    // Intensity correction
    if(wavelength >= 380.0 && wavelength < 420.0){
        intensity = 0.3 + 0.7 * (wavelength - 380.0) / (420.0 - 380.0);
    }else if(wavelength >= 420.0 && wavelength <= 700.0){
        intensity = 1.0;
    }else if(wavelength > 700.0 && wavelength <= 780.0){
        intensity = 0.3 + 0.7 * (780.0 - wavelength) / (780.0 - 700.0);
    }else{
        intensity = 0.0;
    }
    intensity *= 255.0;

    red = intensity * red;
    blue = intensity * blue;
    green = intensity * green;

    return QColor(static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
}

/*
Scales the curve in the given/local space according to the global space
    :param base: the unmodified original of this polygon; used as base for all the calculations
    :param size: the local size the curve is fitted into
    :param xg_begin: the global x begin value, eg wavelength (nm)
    :param xg_end: the global x end value, eg wavelength (nm)
    :param yg_begin: the global y begin value, eg intensity (%)
    :param yg_end: the global y end value, eg intensity (%)
    :param intensity: the y (intensity) scaling value
*/
void Polygon::scale(const Data::Polygon& base, const QRectF& size, const double xg_begin, const double xg_end, const double yg_begin, const double yg_end, const double intensity){
    // Double check for base size, there should not be any resizing of the internal QVector
    if(base.curve.capacity() != this->curve.capacity()){
        qWarning() << "Data::Polygon::scale: base and this object have different capacity QPolygonF, operation is not guaranteed to be safe, function call is ignored.";
        return;
    }

    // Check for fully out of bound curve
    if(xg_begin > this->x_max || xg_end < this->x_min){
        // Empty curve
        this->curve.resize(0);
        return;
    }

    // First resize polygon to base size to prevent undefined behavior
    this->curve.resize(base.curve.size());

    // Calculate x parameters
    double x_fraction = size.width() / (xg_end - xg_begin);
    double xl_start = (this->x_min - xg_begin) * x_fraction;
    double xl_end = (this->x_max - xg_begin) * x_fraction;
    double xl_diff = xl_end - xl_start;

    // Calculate y parameters
    double y_fraction = size.height() / (yg_end - yg_begin);
    double yl_start = (this->y_min - yg_begin) * y_fraction;
    double yl_end = (this->y_max - yg_begin) * y_fraction;
    double yl_diff = yl_start - yl_end;

    // Iterate through polygons, while keeping in mind the x-limits
    int this_i = 0;
    for(int base_i=0; base_i<base.curve.size(); ++base_i){
        double x = base.curve.at(base_i).x();
        double y = base.curve.at(base_i).y();

        // Calculate scaled x and y; y has to be reversed because the local coordinate system of y is from top to bottom
        x = xl_start + (x * xl_diff);
        y = yl_start - (y * yl_diff * intensity);

        // Make sure that the y values are not outside the size rectangle
        if(y < size.top()){
            y = size.top();
        }else if(y > size.bottom()){
            y = size.bottom();
        }

        // Make sure that x is within the size rectangle
        if(x < size.left()){
            this->curve[0].setX(size.left());
            this->curve[0].setY(y);

            // Set i to 1, to allow for normal continuation, but not throw-off repeated hits of this if statement
            this_i = 1;
            continue;
        }else if(x > size.right()){
            // further x-values are outside of the plotting rect
            this->curve[this_i].setX(size.right());
            this->curve[this_i].setY(y);
            ++this_i;
            break;
        }

        // Parse values
        this->curve[this_i].setX(x);
        this->curve[this_i].setY(y);
        ++this_i;
    }

    // Remove unused polygon entrees
    this->curve.remove(this_i, this->curve.size() - this_i);
}

/*
Scales the curve in the given/local space according to the global space
    :param base: the unmodified original of this polygon; used as base for all the calculations
    :param scale_x: the scaling function for the x value
    :param scale_y: the scaling function for the y value
    :param intensity: the y (intensity) scaling value
*/
void Polygon::scale(const Data::Polygon& base, const QRectF& size, std::function<double(double)> scale_x, std::function<double(double, double)> scale_y, const double intensity){
    // Double check for base size, there should not be any resizing of the internal QVector
    if(base.curve.capacity() != this->curve.capacity()){
        qWarning() << "Data::Polygon::scale: base and this object have different capacity QPolygonF, operation is not guaranteed to be safe, function call is ignored.";
        return;
    }

    // Check for fully out of bound curve
    if(size.left() > scale_x(this->x_max) || size.right() < scale_x(this->x_min)){
        // Empty curve
        this->curve.resize(0);
        return;
    }

    // First resize polygon to base size to prevent undefined behavior
    this->curve.resize(base.curve.size());

    // Iterate through polygons, while keeping in mind the x-limits
    int this_i = 0;
    for(int base_i=0; base_i<base.curve.size(); ++base_i){
        double x = base.curve.at(base_i).x();
        double y = base.curve.at(base_i).y();

        x = scale_x(x);
        y = scale_y(y, intensity);

        // Make sure that the y values are not outside the size rectangle
        if(y < size.top()){
            y = size.top();
        }else if(y > size.bottom()){
            y = size.bottom();
        }

        // Make sure that x is within the size rectangle
        if(x < size.left()){
            this->curve[0].setX(size.left());
            this->curve[0].setY(y);

            // Set i to 1, to allow for normal continuation, but not throw-off repeated hits of this if statement
            this_i = 1;
            continue;
        }else if(x > size.right()){
            // further x-values are outside of the plotting rect
            this->curve[this_i].setX(size.right());
            this->curve[this_i].setY(y);
            ++this_i;
            break;
        }

        // Parse values
        this->curve[this_i].setX(x);
        this->curve[this_i].setY(y);
        ++this_i;
    }

    // Remove unused polygon entrees
    this->curve.remove(this_i, this->curve.size() - this_i);
}

/*
Hard copies the curve data from base into the current polygon
    :param base: base containing the curve data
*/
void Polygon::copyCurve(const Data::Polygon& base){
    // First resize polygon to base size to prevent undefined behavior
    this->curve = base.curve;
    this->curve.detach();
}

/*
Adds two coordinates at the end of the curve to 'close' the polygon.
If you would draw a line from a[0] to a[-1], and connect a[0] & a[-1] you have a proper polygon (this is mainly for QPainter::drawPolygon())
    :param rect: the local space allocated to the curve
*/
void Polygon::closeCurve(const QRectF& size){
    // Make sure there is enough space for the additional two QPointF.
    // Standard capacity of the curves should be enough for this
    if(this->curve.empty()){
        return;
    }

    this->curve.resize(this->curve.size() + 2);

    int length = this->curve.length();
    this->curve[length - 2] = QPointF(this->curve[length -3].x(), size.bottom());
    this->curve[length - 1] = QPointF(this->curve[0].x(), size.bottom());
}

/* ######################################################################################### */

/*
Constructor: Constructs a Spectrum object. Contains all necessary plotting data for a fluorophore
    :param id: the spectrum id value
*/
Spectrum::Spectrum(QString id) :
    absorption(false),
    two_photon(false),
    fluor_id(id),
    polygon_excitation(Polygon()),
    polygon_emission(Polygon())
{}

/*
Constructor: Constructs a Spectrum object. Contains all necessary plotting data for a fluorophore
    :param id: the spectrum id value
    :param absorption: the absorption polygon
    :param excitation: the excitation polygon
    :param two_photon: the two_photon polygon
    :param emission: the emission polygon
*/
Spectrum::Spectrum(QString id, Polygon excitation, Polygon emission) :
    absorption(false),
    two_photon(false),
    fluor_id(id),
    polygon_excitation(excitation),
    polygon_emission(emission)
{
}

/*
Getter - returns the fluorophore id of the Spectrum object
*/
QString Spectrum::id() const {
    return(this->fluor_id);
}

/*
Checks whether the excitation/emission parameters are valid
    :returns: true if valid otherwise false
*/
bool Spectrum::isValid() const {
    if(this->polygon_emission.empty()){
        return false;
    }

    if(this->polygon_excitation.empty()){
        return false;
    }

    return true;
}

/*
Getter for the excitation polygon
    :returns: excitation polygon
*/
const Data::Polygon& Spectrum::excitation() const {
    return this->polygon_excitation;
}

/*
Getter for the emission polygon
    :returns: emission polygon
*/
const Data::Polygon& Spectrum::emission() const {
    return this->polygon_emission;
}

/*
Setter for the excitation data,
    :param polygon: the excitation curve
*/
void Spectrum::setExcitation(Polygon polygon){
    this->polygon_excitation = std::move(polygon);
}

/*
Setter for the emission data
    :param polygon: the emission curve
*/
void Spectrum::setEmission(Polygon polygon){
    this->polygon_emission = std::move(polygon);
}

/*
Getter for the absorption flag
    :returns: is the excitation curve actually an absorption curve
*/
bool Spectrum::absorptionFlag() const {
    return this->absorption;
}

/*
Getter for the two photon flag
    :returns: is the emission curve actually an two photon curve
*/
bool Spectrum::twoPhotonFlag() const {
    return this->two_photon;
}

/*
Setter for the absorption flag; to-be used when instead of excitation an absorption polygon is used
    :param flag: whether or not absorption is used
*/
void Spectrum::setAbsorptionFlag(bool flag){
    this->absorption = flag;
}

/*
Setter for the two_photon flag; to-be used when instead of emission a two_photon polygon is used
    :param flag: whether or not absorption is used
*/
void Spectrum::setTwoPhotonFlag(bool flag){
    this->two_photon = flag;
}

/*
Returns the excitation intensity at a specific wavelength
    :param wavelength: the excitation wavelength
    :returns: the excitation intensity fraction, out of bounds returns 0.0, an unspecified wavelength returns the first specified lower value
*/
double Spectrum::excitationAt(double wavelength, double cutoff) const {
    return this->polygon_excitation.intensityAt(wavelength, cutoff);
}

/*
Returns the emission intensity at a specific wavelength
    :param wavelength: the emission wavelength
    :returns: the emission intensity fraction, out of bounds returns 0.0, an unspecified wavelength returns the first specified higher value
*/
double Spectrum::emissionAt(double wavelength, double cutoff) const {
    return this->polygon_emission.intensityAt(wavelength, cutoff);
}

/*
Returns the wavelength of the maximum excitation intensity
    :returns: the wavelength of the (first) maximum excitation
*/
double Spectrum::excitationMax() const {
    return this->polygon_excitation.intensityMax();
}

/*
Returns the wavelength of the maximum emission intensity
    :returns: the wavelength of the (first) maximum emission
*/
double Spectrum::emissionMax() const {
    return this->polygon_emission.intensityMax();
}

/* ######################################################################################### */

/*
Constructor: Constructs a CacheSpectrum object. Combines a Spectrum object with additional cache parameters
*/
CacheSpectrum::CacheSpectrum(unsigned int index, Data::Spectrum spectrum) :
    cache_index(index),
    spectrum_data(std::move(spectrum)),
    spectrum_meta(),
    visible_excitation(false),
    visible_emission(true),
    select_excitation(false),
    select_emission(false),
    intensity_cutoff(0.0),
    modified(false)
{}

CacheSpectrum::CacheSpectrum(unsigned int index, Data::Spectrum spectrum, Data::Meta meta) :
    cache_index(index),
    spectrum_data(std::move(spectrum)),
    spectrum_meta(std::move(meta)),
    visible_excitation(false),
    visible_emission(true),
    select_excitation(false),
    select_emission(false),
    intensity_cutoff(0.0),
    modified(false)
{}

/*
Getter for the build index
    :returns: build index
*/
unsigned int CacheSpectrum::index() const {
    return(this->cache_index);
}

/*
Setter for the build index
    :param: index
*/
void CacheSpectrum::setIndex(unsigned int index){
    this->cache_index = index;
}

/*
Getter for the fluorophore id
    :returns: fluorophore id
*/
QString CacheSpectrum::id() const {
    return(this->spectrum_data.id());
}

/*
Getter for the wavelength of the max excitation
    :returns: wavelength of the maximum excitation
*/
double CacheSpectrum::excitationMax() const {
    if(this->spectrum_meta.excitation_max == -1){
        return(this->spectrum_data.excitationMax());
    }else{
        return(this->spectrum_meta.excitation_max);
    }
}

/*
Getter for the wavelength of the max emission
    :returns: wavelength of the maximum emission
*/
double CacheSpectrum::emissionMax() const {
    if(this->spectrum_meta.emission_max == -1){
        return(this->spectrum_data.emissionMax());
    }else{
        return(this->spectrum_meta.emission_max);
    }
}

/*
Getter for the visibility of the excitation plot
    :returns: excitation visibility
*/
bool CacheSpectrum::visibleExcitation() const {
    //qDebug() << "ex get:" << this->spectrum_data.id() << this->visible_excitation;
    return this->visible_excitation;
}

/*
Getter for the visibility of the emission plot
    :returns: emission visibility
*/
bool CacheSpectrum::visibleEmission() const {
    //qDebug() << "em get:" << this->spectrum_data.id() << this->visible_emission;
    return this->visible_emission;
}

/*
Setter for the visibility of the excitation plot
    :params: excitation visibility
*/
void CacheSpectrum::setVisibleExcitation(bool visibility) {
    if(visibility != this->visible_excitation){
        this->visible_excitation = visibility;
        this->modified = true;
    }
}

/*
Setter for the visibility of the emission plot
    :params: emission visibility
*/
void CacheSpectrum::setVisibleEmission(bool visibility) {
    if(visibility != this->visible_emission){
        this->visible_emission = visibility;
        this->modified = true;
    }
}

/*
Getter for the select parameter of the excitation plot
    :returns: excitation select
*/
bool CacheSpectrum::selectExcitation() const {
    return this->select_excitation;
}

/*
Getter for the select parameter of the emission plot
    :returns: emission select
*/
bool CacheSpectrum::selectEmission() const {
    return this->select_emission;
}

/*
Setter for the select parameter of the excitation plot
    :params: excitation select
*/
void CacheSpectrum::setSelectExcitation(bool select) {
    if(select != this->select_excitation){
        this->select_excitation = select;
        this->modified = true;
    }
}

/*
Setter for the select parameter of the emission plot
    :params: emission select
*/
void CacheSpectrum::setSelectEmission(bool select) {
    if(select != this->select_emission){
        this->select_emission = select;
        this->modified = true;
    }
}

/*
Getter for the intensity cutoff
    :returns: intensity cutoff
*/
double CacheSpectrum::intensityCutoff() const {
    return(this->intensity_cutoff);
}

/*
Setter for the intensity cutoff
    :params: intensity cutoff
*/
void CacheSpectrum::setIntensityCutoff(const double cutoff) {
    this->intensity_cutoff = cutoff;
}

/*
Resets the modified flag to unmodified
*/
void CacheSpectrum::resetModified() {
    this->modified = false;
}

/*
Getter for the internal spectrum object
    :returns: spectrum reference
*/
const Data::Spectrum& CacheSpectrum::spectrum() const {
    return this->spectrum_data;
}

/*
Getter for the internal spectrum object.
    :returns: a copy of the spectrum
*/
Data::Spectrum CacheSpectrum::copySpectrum() const {
    return this->spectrum_data;
}

/*
Getter for the polygon absorption flag
    :returns: absorption flag
*/
bool CacheSpectrum::absorptionFlag() const {
    return this->spectrum_data.absorptionFlag();
}

/*
Getter for the polygon two photon flag
    :returns: two photon flag
*/
bool CacheSpectrum::twoPhotonFlag() const {
    return this->spectrum_data.twoPhotonFlag();
}

/*
Returns the excitation intensity (0.0 - 100.0) at the specified wavelength
    :param wavelength: the wavelength to find the intensity of
*/
double CacheSpectrum::excitationAt(double wavelength) const {
    return this->spectrum_data.excitationAt(wavelength, 0.0);
}
/*
Returns the emission intensity (0.0 - 100.0) at the specified wavelength
    :param wavelength: the wavelength to find the intensity of
*/
double CacheSpectrum::emissionAt(double wavelength) const {
    return this->spectrum_data.emissionAt(wavelength, 0.0);
}

} // Data namespace
