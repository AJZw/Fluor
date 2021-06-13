/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_data_fluorophores.cpp is part of Spectral Viewer
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

#include "viewer_data_fluorophores.h"
#include <QDebug>

/*
Constructor: Construct a standard DataColor object (color=white (255, 255, 255))
*/
DataColor::DataColor() :
    red(255), green(255), blue(255)
{}

/*
Constructor: Construct a DataColor object
    :param red: red color value 0-255
    :param green: green color value 0-255
    :param blue: blue color value 0-255
*/
DataColor::DataColor(int red, int green, int blue) :
    red(red), green(green), blue(blue)
{}

/* ######################################################################################### */

/*
Constructor: Constructs a DataSpectrum object. Contains all necessary plotting data for a fluophore
*/
DataSpectrum::DataSpectrum(QString fluor_id) :
    fluor_id(fluor_id),
    excitation_wavelength(),
    excitation_intensity(),
    emission_wavelength(),
    emission_intensity()
{}

DataSpectrum::DataSpectrum(
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
Getter - returns the fluorophore id of the DataSpectrum object
*/
QString DataSpectrum::id() const {
    return(this->fluor_id);
}

/*
Checks whether the excitation/emission parameters are valid
    :returns: true if valid otherwise false
*/
bool DataSpectrum::isValid() const {
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
Setter for the excitation data, no validity checking
    :param wavelength: sets the excitation wavelength
    :param intensity: sets the excitation intensity
*/
void DataSpectrum::setExcitation(std::vector<double> wavelength, std::vector<double> intensity){
    this->excitation_wavelength = wavelength;
    this->excitation_intensity = intensity;
}

/*
Setter for the emission data, no validity checking
    :param wavelength: sets the emission wavelength
    :param intensity: sets the emission intensity
*/
void DataSpectrum::setEmission(std::vector<double> wavelength, std::vector<double> intensity){
    this->emission_wavelength = wavelength;
    this->emission_intensity = intensity;
}

/*
Getter for the excitation wavelength
    :returns: excitation wavelength
*/
std::vector<double> DataSpectrum::getExcitationWavelength() const {
    return(this->excitation_wavelength);
}

/*
Getter for the excitation intensity
    :returns: excitation intensity
*/
std::vector<double> DataSpectrum::getExcitationIntensity() const {
    return(this->excitation_intensity);
}

/*
Getter for the emission wavelength
    :returns: emission wavelength
*/
std::vector<double> DataSpectrum::getEmissionWavelength() const {
    return(this->emission_wavelength);
}

/*
Getter for the emission intensity
    :returns: emission intensity
*/
std::vector<double> DataSpectrum::getEmissionIntensity() const {
    return(this->emission_intensity);
}

/*
Getter for the emission intensity after intensity correction
    :param intensity: value in between 0%-100%
    :param cutoff: intensity threshold, any value lower will return 0 intensity
    :returns: corrected emission intensity 
*/
std::vector<double> DataSpectrum::getEmissionIntensity(const double intensity, const double cutoff) const {
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
double DataSpectrum::intensityAt(const std::vector<double>& wavelength, const std::vector<double>& intensity, const int& at_wavelength){
    // if vector is empty, any indexing causes undefined behavior. I dont want to check on every call, 
    // use isValid() upon creation to make sure all assumptions are met
    // if(wavelength.empty() || intensity.empty()){
    //     return(0.0);
    // }

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
    int index;
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
double DataSpectrum::excitationAt(const int wavelength) const {
    return (DataSpectrum::intensityAt(this->excitation_intensity, this->excitation_wavelength, wavelength));
}

/*
Returns the emission intensity at a specific wavelength
    :param wavelength: the emission wavelength
    :returns: the emission intensity, out of bounds returns 0, an unspecified wavelength returns the first specified higher value
*/
double DataSpectrum::emissionAt(const int wavelength) const {
    return(DataSpectrum::intensityAt(this->emission_intensity, this->emission_wavelength, wavelength));
}

/*
Returns the wavelength of the maximum excitation intensity
    :returns: the wavelength of the (first) maximum excitation
*/
double DataSpectrum::excitationMax() const {
    // iterate over the excitation to get the maximum 
    std::vector<double>::const_iterator max = this->excitation_intensity.begin();

    std::vector<double>::const_iterator iter;
    for(iter = this->excitation_intensity.begin(); iter < this->excitation_intensity.end(); iter++){
        if(*iter > *max){
            max = iter;
        }
    }

    int index;
    index = max - this->excitation_intensity.begin();

    return(this->excitation_wavelength[index]);
}

/*
Returns the wavelength of the maximum emission intensity
    :returns: the wavelength of the (first) maximum emission
*/
double DataSpectrum::emissionMax() const {
    // iterate over the excitation to get the maximum 
    std::vector<double>::const_iterator max = this->emission_intensity.begin();

    std::vector<double>::const_iterator iter;
    for(iter = this->emission_intensity.begin(); iter < this->emission_intensity.end(); iter++){
        if(*iter > *max){
            max = iter;
        }
    }

    int index;
    index = max - this->emission_intensity.begin();

    return(this->emission_wavelength[index]);
}

/*
(Static) Returns the color of the maximum emission intensity 
    :param wavelength: wavelength to transform into visible RGB value
    :returns: DataColor object; colors scale 0-255
*/
DataColor DataSpectrum::color(double wavelength){
    double red, green, blue, intensity;
    if(wavelength >= 380.0 && wavelength < 440.0){
        red = -(wavelength - 440.0) / (440.0 - 350.0);
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
        intensity = 0.3 + 0.7 * (wavelength - 350.0) / (420.0 - 350.0);
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

    DataColor color(static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));

    return color;
}

/* ######################################################################################### */

/*
Constructor: Construct a DataMeta object with default values of 0
*/
DataMeta::DataMeta() :
    excitation_max(0), emission_max(0)
{}

/*
Constructor: Construct a DataMeta object
    :param excitation_max: excitation max wavelength 
    :param emission_max: emission max wavelength 
*/
DataMeta::DataMeta(int excitation_max, int emission_max) :
    excitation_max(excitation_max), emission_max(emission_max)
{}

/* ######################################################################################### */

/*
Constructor: Construct the fluorophore data types
*/
DataFluorophores::DataFluorophores() :
    fluor_name(),
    fluor_id(),
    fluor_multiname(),
    fluor_meta()
{}

/*
Loads the fluorophores QSettings
Note: do not call this if DataFactory 'fluorophores' is invalid, that causes the factory to qFatal()
    :param data: the DataFactory to request the source data from
*/
void DataFluorophores::load(DataFactory& data){
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = data.get("fluorophores");

    // Get default parameters
    QString default_enable;
    QString default_excitation_max;
    QString default_emission_max;

    default_enable = fluorophores->value("DEFAULT/enable", true).toBool();
    default_excitation_max = fluorophores->value("DEFAULT/excitation_max", 0).toInt();
    default_emission_max = fluorophores->value("DEFAULT/emission_max", 0).toInt();

    // Reserving memory to prevent unnessary growing
    int data_size;
    int multi_size;
    data_size = fluorophores->allKeys().size();
    multi_size = 5; // A guess, depends on amount of alternative names, if vector/unordered_map is too small, will auto grow
    this->fluor_id.reserve(data_size + multi_size);
    this->fluor_name.reserve(data_size + multi_size);
    this->fluor_meta.reserve(data_size);
    this->fluor_multiname.reserve(multi_size);

    // Iterate over data file to load all data objects
    for(const QString &group : fluorophores->childGroups()){
        if(group == "DEFAULT"){
            continue;
        }

        fluorophores->beginGroup(group);

        if(!fluorophores->value("enable", default_enable).toBool()){
            fluorophores->endGroup();
            continue;
        }

        // Build meta tags
        DataMeta group_meta;
        group_meta.excitation_max = fluorophores->value("excitation_max", default_excitation_max).toInt();
        group_meta.emission_max = fluorophores->value("emission_max", default_emission_max).toInt();
        this->fluor_meta[group] = group_meta;

        // Builds map for alternative name lookup
        QStringList group_multi;
        group_multi = fluorophores->value("alternative_name").toStringList();
        if(group_multi.isEmpty()){
            // No alternative name, add group name identical to group id
            this->fluor_name.push_back(group);
            this->fluor_id[group] = group;
        }else{
            // Alternative names
            group_multi.append(group);

            this->fluor_name.insert(std::end(this->fluor_name), std::begin(group_multi), std::end(group_multi));          

            int index = 0;
            QStringList group_multi_temp;
            for(const QString &name : group_multi){
                // Add fluorophore name corresponding fluorophore ID for lookup
                this->fluor_id[name] = group;

                //QStringList group_multi_temp;
                group_multi_temp = group_multi;   // technically makes a shallow copy
                group_multi_temp.removeAt(index); // but upon modification becomes a deep copy
                this->fluor_multiname[name] = group_multi_temp;

                ++index;
            }
        }

        fluorophores->endGroup();
    }

    // Sort alphabetical order
    std::sort(this->fluor_name.begin(), this->fluor_name.end());
    
    // Sets the loaded flag
    data_loaded = true;
}

/*
Unallocates memory of all loaded data
*/
void DataFluorophores::unload(){
    std::unordered_map<QString, QString>().swap(this->fluor_id);
    std::vector<QString>().swap(this->fluor_name);
    std::unordered_map<QString, DataMeta>().swap(this->fluor_meta);
    std::unordered_map<QString, QStringList>().swap(this->fluor_multiname);
    data_loaded = false;
}

/*
Returns whether DataFluorophores is valid (has loaded fluorophore data)
    :returns: validity
*/
bool DataFluorophores::isValid() const {
    if(this->data_loaded){
        return true;
    }else{
        return false;
    }
}

/*
Fluorophore spectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is invalid returns are empty vectors
    :returns: DataSpectrum object
*/
std::unique_ptr<DataSpectrum> DataFluorophores::getSpectrum(const DataFactory& data, const QString& id) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores;
    fluorophores = data.get("fluorophores");

    //std::vector<double> excitation_wavelength;
    std::vector<double> excitation_wavelength;
    std::vector<double> excitation_intensity;
    std::vector<double> emission_wavelength;
    std::vector<double> emission_intensity;

    fluorophores->beginGroup(id);

    // Load data and transform to vector of double
    excitation_wavelength = DataFluorophores::toStdVector(fluorophores->value("excitation_wavelength", "0.0").toStringList());
    excitation_intensity = DataFluorophores::toStdVector(fluorophores->value("excitation_intensity", "0.0").toStringList());
    emission_wavelength = DataFluorophores::toStdVector(fluorophores->value("emission_wavelength", "0.0").toStringList());
    emission_intensity = DataFluorophores::toStdVector(fluorophores->value("emission_intensity", "0.0").toStringList());

    fluorophores->endGroup();

    // Check if valid -> if not send qWarning and build a new (empty) DataSpectrum
    std::unique_ptr<DataSpectrum> spectrum;
    spectrum = std::make_unique<DataSpectrum>(id, excitation_wavelength, excitation_intensity, emission_wavelength, emission_intensity);

    // Verify validity, if invalid return
    if(spectrum->isValid()){
        return spectrum;
    }else{
        std::vector<double> vector_null(1);
        vector_null = {0.0};
        
        qWarning() << "DataFluorophores::getSpectrum: DataSpectrum object of id " << id << " is invalid.";
        spectrum = std::make_unique<DataSpectrum>(id, vector_null, vector_null, vector_null, vector_null);
        return spectrum;
    }
}

/*
Getter for the ordered fluorophore name vector
    :returns: reference to fluor_name
*/
const std::vector<QString>& DataFluorophores::getFluorName() const {
    return this->fluor_name;
}

/*
Getter for the fluorophore id map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QString>& DataFluorophores::getFluorID() const {
    return this->fluor_id;
}

/*
Getter for the fluorophore multiname map
    :returns: reference to fluor_multiname
*/
const std::unordered_map<QString, QStringList>& DataFluorophores::getFluorMultiname() const {
    return this->fluor_multiname;
}

/*
Getter for the fluorophore metadata map
    :returns: reference to fluor_meta
*/
const std::unordered_map<QString, DataMeta>& DataFluorophores::getFluorMeta() const {
    return this->fluor_meta;
}

/*
(Static) Converts a QStringList into a std::vector of doubles
    :returns: vector of double typeconverted QStrings
*/
std::vector<double> DataFluorophores::toStdVector(const QStringList& stringlist){
    std::vector<double> double_vector(stringlist.size());

    // Iterate using index, because I need to read the input to the output at the same time   
    for(int i=0; i<stringlist.size(); ++i){
        double_vector[i] = stringlist[i].toDouble();
    }

    return double_vector;
} 

/*
(Static) Debugging functions, unpacks Map/Set for parsing to qDebug()
*/
void DataFluorophores::qDebugMap(const std::unordered_map<QString, QString>& map){
    qDebug() << "std::unordered_map<QString, QString>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugMap(const std::unordered_map<QString, QStringList>& map){
    qDebug() << "std::unordered_map<QString, QStringList>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugMap(const std::unordered_map<QString, std::vector<int>>& map){
    qDebug() << "std::unordered_map<QString, std::vector<int>>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void DataFluorophores::qDebugSet(const std::unordered_set<QString>& map){
    qDebug() << "std::unordered_set<QString>:";
    for(const QString &key: map){
        qDebug() << "{" << key << "}";
    }
}
