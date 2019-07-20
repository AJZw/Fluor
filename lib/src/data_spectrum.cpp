/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_spectrum.h"

#include <QDebug>

namespace Data {

/*
Constructor: Construct a standard Data::Color object (color=white (255, 255, 255))
*/
Color::Color() :
    red(255), green(255), blue(255)
{}

/*
Constructor: Construct a Data::Color object
    :param red: red color value 0-255
    :param green: green color value 0-255
    :param blue: blue color value 0-255
*/
Color::Color(int red, int green, int blue) :
    red(red), green(green), blue(blue)
{}

/* ######################################################################################### */

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
Meta::Meta(int excitation_max, int emission_max) :
    excitation_max(excitation_max), emission_max(emission_max)
{}

/* ######################################################################################### */

/*
Constructor: Constructs a Spectrum object. Contains all necessary plotting data for a fluophore
*/
Spectrum::Spectrum(QString fluor_id) :
    fluor_id(fluor_id),
    excitation_wavelength(),
    excitation_intensity(),
    emission_wavelength(),
    emission_intensity()
{}

Spectrum::Spectrum(
    QString fluor_id,
    std::vector<double> excitation_wavelength, 
    std::vector<double> excitation_intensity, 
    std::vector<double> emission_wavelength, 
    std::vector<double> emission_intensity) :
    fluor_id(fluor_id),
    excitation_wavelength(excitation_wavelength),
    excitation_intensity(excitation_intensity),
    emission_wavelength(emission_wavelength),
    emission_intensity(emission_intensity)
{}

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
    if(this->excitation_intensity.empty() || this->excitation_wavelength.empty()){
        return(false);
    }else if(this->emission_intensity.empty() || this->emission_wavelength.empty()){
        return(false);
    }else if(this->excitation_intensity.size() != this->excitation_wavelength.size()){
        return(false);
    }else if(this->emission_intensity.size() != this->emission_wavelength.size()){
        return(false);
    }else{
        return(true);
    }
}

/*
Sets all emission and excitation data to size 1 containing a 0
*/
void Spectrum::setToZero(){
    std::vector<double> vector_null{0.0};
    
    this->excitation_intensity = vector_null;
    this->excitation_wavelength = vector_null;
    this->emission_intensity = vector_null;
    this->emission_wavelength = vector_null;
}

/*
Setter for the excitation data, no validity checking
    :param wavelength: sets the excitation wavelength
    :param intensity: sets the excitation intensity
*/
void Spectrum::setExcitation(std::vector<double> wavelength, std::vector<double> intensity){
    this->excitation_wavelength = wavelength;
    this->excitation_intensity = intensity;
}

/*
Setter for the emission data, no validity checking
    :param wavelength: sets the emission wavelength
    :param intensity: sets the emission intensity
*/
void Spectrum::setEmission(std::vector<double> wavelength, std::vector<double> intensity){
    this->emission_wavelength = wavelength;
    this->emission_intensity = intensity;
}

/*
Getter for the excitation wavelength
    :returns: excitation wavelength
*/
std::vector<double> Spectrum::getExcitationWavelength() const {
    return(this->excitation_wavelength);
}

/*
Getter for the excitation intensity
    :returns: excitation intensity
*/
std::vector<double> Spectrum::getExcitationIntensity() const {
    return(this->excitation_intensity);
}

/*
Getter for the emission wavelength
    :returns: emission wavelength
*/
std::vector<double> Spectrum::getEmissionWavelength() const {
    return(this->emission_wavelength);
}

/*
Getter for the emission intensity
    :returns: emission intensity
*/
std::vector<double> Spectrum::getEmissionIntensity() const {
    return(this->emission_intensity);
}

/*
Getter for the emission intensity after intensity correction
    :param intensity: value in between 0%-100%
    :param cutoff: intensity threshold, any value lower will return 0 intensity
    :returns: corrected emission intensity 
*/
std::vector<double> Spectrum::getEmissionIntensity(const double intensity, const double cutoff) const {
    // If intensity too low, no reason to transform the vector, build a new vector with only 0.0s
    if(intensity <= cutoff){
        std::vector<double> zero_vector(this->emission_intensity.size());
        std::fill(zero_vector.begin(), zero_vector.end(), 0.0);
        return(zero_vector);
    }

    // Check if no recalculation is necessary
    if(intensity == 100.0){
        return(this->emission_intensity);
    }

    // Factorize the intensity
    double intensity_factor;
    intensity_factor = intensity / 100;

    // Transform a copy of the vector
    std::vector<double> transpose_vector(this->emission_intensity.size());
    std::transform(
        this->emission_intensity.begin(),
        this->emission_intensity.end(),
        transpose_vector.begin(),
        [intensity_factor](const double& emission_intensity){return emission_intensity * intensity_factor;}
    );
    return(transpose_vector);
}

/*
(Static) General function that returns intensity of a specific wavelength using a binary search
    :param wavelength: vector wavelength reference
    :param intensity: vector intensity reference
    :param at_wavelength: wavelength to return the intensity of
    :returns: intensity at the specified wavelength. Out of bounds returns 0, unspecified at_wavelength returns the first specified higher value
*/
double Spectrum::intensityAt(const std::vector<double>& wavelength, const std::vector<double>& intensity, const int& at_wavelength){
    // if vector is empty, any indexing causes undefined behavior. I dont want to check on every call, 
    // use isValid() upon creation to make sure all assumptions are met

    // Check if wavelength falls within the excitation_wavelength defined region
    if(wavelength.front() > at_wavelength || wavelength.back() < at_wavelength){
        return(0.0);
    }

    // I can use a binary search as the wavelength is sorted from low to high
    // Should be impossible to not find a lower_bound as I have already checked the vectors bounds
    std::vector<double>::const_iterator first;
    first = std::lower_bound(wavelength.begin(), wavelength.end(), at_wavelength);
    // first contains the first value that is lower or exact wavelength
    // substract iterator location from the starting location to get the index
    std::size_t index;
    index = first - wavelength.begin();

    // bound checking shouldnt be necessary as intensity and wavelength should be of equal length
    // and wavelength should always be found inside the vector, but to prevent segfaults:
    return(intensity.at(index));
}

/*
Returns the excitation intensity at a specific wavelength
    :param wavelength: the excitation wavelength
    :returns: the excitation intensity, out of bounds returns 0, an unspecified wavelength returns the first specified higher value
*/
double Spectrum::excitationAt(const int wavelength) const {
    return (Spectrum::intensityAt(this->excitation_intensity, this->excitation_wavelength, wavelength));
}

/*
Returns the emission intensity at a specific wavelength
    :param wavelength: the emission wavelength
    :returns: the emission intensity, out of bounds returns 0, an unspecified wavelength returns the first specified higher value
*/
double Spectrum::emissionAt(const int wavelength) const {
    return(Spectrum::intensityAt(this->emission_intensity, this->emission_wavelength, wavelength));
}

/*
Returns the wavelength of the maximum excitation intensity
    :returns: the wavelength of the (first) maximum excitation
*/
double Spectrum::excitationMax() const {
    // iterate over the excitation to get the maximum 
    std::vector<double>::const_iterator max = this->excitation_intensity.begin();

    std::vector<double>::const_iterator iter;
    for(iter = this->excitation_intensity.begin(); iter < this->excitation_intensity.end(); ++iter){
        if(*iter > *max){
            max = iter;
        }
    }

    std::size_t index;
    index = max - this->excitation_intensity.begin();

    return(this->excitation_wavelength[index]);
}

/*
Returns the wavelength of the maximum emission intensity
    :returns: the wavelength of the (first) maximum emission
*/
double Spectrum::emissionMax() const {
    // iterate over the excitation to get the maximum 
    std::vector<double>::const_iterator max = this->emission_intensity.begin();

    std::vector<double>::const_iterator iter;
    for(iter = this->emission_intensity.begin(); iter < this->emission_intensity.end(); ++iter){
        if(*iter > *max){
            max = iter;
        }
    }

    std::size_t index;
    index = max - this->emission_intensity.begin();

    return(this->emission_wavelength[index]);
}

/*
(Static) Returns the color of the maximum emission intensity, uses linear approximation
Source: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
    :param wavelength: wavelength to transform into visible RGB value
    :returns: Data::Color object; colors scale 0-255
*/
Data::Color Spectrum::color(double wavelength){
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

    Data::Color color(static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));

    return color;
}

/* ######################################################################################### */

/*
Constructor: Constructs a CacheSpectrum object. Combines a Spectrum object with additional cache parameters
*/
CacheSpectrum::CacheSpectrum(unsigned int index, Data::Spectrum spectrum) :
    build_index{index},
    modified{false},
    fluor_spectrum{std::move(spectrum)},
    fluor_name{std::move(this->fluor_spectrum.id())},
    fluor_meta{},
    fluor_visible_excitation{false},
    fluor_visible_emission{true},
    fluor_intensity_cutoff{0.0}
{}

CacheSpectrum::CacheSpectrum(unsigned int index, QString fluor_name, Data::Spectrum spectrum) :
    build_index{index},
    modified{false},
    fluor_spectrum{std::move(spectrum)},
    fluor_name{std::move(fluor_name)},
    fluor_meta{},
    fluor_visible_excitation{false},
    fluor_visible_emission{true},
    fluor_intensity_cutoff{0.0}
{}

CacheSpectrum::CacheSpectrum(unsigned int index, QString fluor_name, Data::Spectrum spectrum, Data::Meta meta) :
    build_index{index},
    modified{false},
    fluor_spectrum{std::move(spectrum)},
    fluor_name{std::move(fluor_name)},
    fluor_meta{std::move(meta)},
    fluor_visible_excitation{false},
    fluor_visible_emission{true},
    fluor_intensity_cutoff{0.0}
{}

/*
Getter for the build index
    :returns: build index
*/
unsigned int CacheSpectrum::index() const {
    return(this->build_index);
}

/*
Setter for the build index
    :param: index
*/
void CacheSpectrum::setIndex(unsigned int index){
    this->build_index = index;
}

/*
Getter for the fluorophore id
    :returns: fluorophore id
*/
QString CacheSpectrum::id() const {
    return(this->fluor_spectrum.id());
}

/*
Getter for the fluorophore name
    :returns: fluorophore name
*/
QString CacheSpectrum::name() const {
    return(this->fluor_name);
}

/*
Setter for the fluorophore name
    :param: name
*/
void CacheSpectrum::setName(const QString& name){
    this->fluor_name = name;
}

/*
Getter for the wavelength of the max excitation
    :returns: wavelength of the maximum excitation
*/
double CacheSpectrum::excitationMax() const {
    if(this->fluor_meta.excitation_max == -1){
        return(this->fluor_spectrum.excitationMax());
    }else{
        return(this->fluor_meta.excitation_max);
    }
}

/*
Getter for the wavelength of the max emission
    :returns: wavelength of the maximum emission
*/
double CacheSpectrum::emissionMax() const {
    if(this->fluor_meta.emission_max == -1){
        return(this->fluor_spectrum.emissionMax());
    }else{
        return(this->fluor_meta.emission_max);
    }
}

/*
Getter for the visibility of the excitation plot
    :returns: excitation visibility
*/
bool CacheSpectrum::visibleExcitation() const {
    return(this->fluor_visible_excitation);
}

/*
Getter for the visibility of the emission plot
    :returns: emission visibility
*/
bool CacheSpectrum::visibleEmission() const {
    return(this->fluor_visible_emission);
}

/*
Setter for the visibility of the excitation plot
    :params: excitation visibility
*/
void CacheSpectrum::setVisibleExcitation(bool visibility) {
    if(visibility != this->fluor_visible_excitation){
        this->fluor_visible_excitation = visibility;
        this->modified = true;
    }
}

/*
Setter for the visibility of the emission plot
    :params: emission visibility
*/
void CacheSpectrum::setVisibleEmission(bool visibility) {
    if(visibility != this->fluor_visible_emission){
        this->fluor_visible_emission = visibility;
        this->modified = true;
    }
}

/*
Getter for the intensity cutoff
    :returns: intensity cutoff
*/
double CacheSpectrum::intensityCutoff() const {
    return(this->fluor_intensity_cutoff);
}

/*
Setter for the intensity cutoff
    :params: intensity cutoff
*/
void CacheSpectrum::setIntensityCutoff(const double cutoff) {
    this->fluor_intensity_cutoff = cutoff;
}

/*
Getter for the excitation wavelength
    :returns: excitation wavelength
*/
std::vector<double> CacheSpectrum::getExcitationWavelength() const {
    return(this->fluor_spectrum.getExcitationWavelength());
}

/*
Getter for the excitation intensity
    :returns: excitation intensity
*/
std::vector<double> CacheSpectrum::getExcitationIntensity() const {
    return(this->fluor_spectrum.getExcitationIntensity());
}

/*
Getter for the emission wavelength
    :returns: emission wavelength
*/
std::vector<double> CacheSpectrum::getEmissionWavelength() const {
    return(this->fluor_spectrum.getEmissionWavelength());
}

/*
Getter for the emission intensity
    :returns: emission intensity
*/
std::vector<double> CacheSpectrum::getEmissionIntensity() const{
    return(this->fluor_spectrum.getEmissionIntensity());
}

/*
Getter for the intensity adjusted emission intensity
    :param intensity: the intensity percentage between 0-100
    :returns: intensity corrected emission intensity
*/
std::vector<double> CacheSpectrum::getEmissionIntensity(const double intensity) const {
    return(this->fluor_spectrum.getEmissionIntensity(std::move(intensity), this->fluor_intensity_cutoff));
}

} // Data namespace
