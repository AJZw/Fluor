/**** General **************************************************************
** Version:    v0.9.8
** Date:       2020-08-05
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_fluorophores.h"

#include <QPolygonF>
#include <QDebug>

namespace Data {

/*
Constructor: Construct the fluorophore data types
*/
FluorophoreReader::FluorophoreReader() :
    data_loaded(false),
    fluor_name(),
    fluor_id(),
    fluor_names()
{}

/*
Loads the fluorophores QSettings
Note: do not call this if DataFactory 'fluorophores' is invalid, that causes the factory to qFatal()
    :param data: the DataFactory to request the source data from
*/
void FluorophoreReader::load(Data::Factory& data){
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::Fluorophores);

    // Get default parameters
    bool default_enable = fluorophores->value("DEFAULT/enable", true).toBool();

    // Reserving memory to prevent unnessary growing
    std::size_t data_size = static_cast<std::size_t>(fluorophores->allKeys().size());   // No sign conversion, size is always =<0 
    std::size_t multi_size = 5; // A guess, depends on amount of alternative names, if vector/unordered_map is too small, will auto grow anyway
    
    this->fluor_id.reserve(data_size + multi_size);
    this->fluor_name.reserve(data_size + multi_size);
    this->fluor_names.reserve(multi_size);

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

        // Builds list of names for menu's etc.
        QStringList group_names{fluorophores->value("names", group).toStringList()};
        this->fluor_name.insert(std::end(this->fluor_name), std::begin(group_names), std::end(group_names));          

        // Build fluorophore name to fluorophore ID lookup map
        for(const QString &name : group_names){
            // Add fluorophore name corresponding fluorophore ID for lookup
            this->fluor_id[name] = group;
        }

        // Build fluorophore names map
        for(const QString &name : group_names){
            // Add fluorophore name corresponding fluorophore ID for lookup
            this->fluor_names[name] = group_names;
        }

        fluorophores->endGroup();
    }

    // Sort case-insensitive alphabetical order
    std::sort(
        this->fluor_name.begin(),
        this->fluor_name.end(),
        [](const QString& left, const QString& right){return left.toLower() < right.toLower();}
    );
    
    // Sets the loaded flag
    data_loaded = true;
}

/*
Unallocates memory of all loaded data
*/
void FluorophoreReader::unload(){
    std::vector<QString>().swap(this->fluor_name);
    std::unordered_map<QString, QString>().swap(this->fluor_id);
    std::unordered_map<QString, QStringList>().swap(this->fluor_names);
    data_loaded = false;
}

/*
Returns whether Fluorophores is valid (has loaded fluorophore data)
    :returns: validity
*/
bool FluorophoreReader::isValid() const {
    return this->data_loaded;
}

/*
Fluorophore spectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is not in source data, returns valid but otherwise useless data
*/
Data::Spectrum FluorophoreReader::getSpectrum(const Data::Factory& data, const QString& id) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::Fluorophores);

    fluorophores->beginGroup(id);

    // Load data and transform to Data::Polygon's
    bool is_absorption = false;
    QStringList excitation_wavelength = fluorophores->value("excitation_wavelength", QVariant()).toStringList();
    QStringList excitation_intensity = fluorophores->value("excitation_intensity", QVariant()).toStringList();

    // If no excitation is available fallback to absorption
    if(excitation_wavelength.empty() || excitation_intensity.empty()){
        is_absorption = true;
        excitation_wavelength = fluorophores->value("absorption_wavelength", "0.0,0.0").toStringList();
        excitation_intensity = fluorophores->value("absorption_intensity", "0.0,0.0").toStringList();
    }
    Data::Polygon excitation = this->toPolygon(excitation_wavelength, excitation_intensity);

    QStringList emission_wavelength = fluorophores->value("emission_wavelength", "0.0,0.0").toStringList();
    QStringList emission_intensity = fluorophores->value("emission_intensity", "0.0,0.0").toStringList();
    Data::Polygon emission = this->toPolygon(emission_wavelength, emission_intensity);

    fluorophores->endGroup();

    // Set linecolor, cannot use the meta-data shortcut as spectrum doesnt request this info
    emission.setColor();
    excitation.setColor(emission.color());

    // Verify validity, if invalid return
    Data::Spectrum spectrum(id, excitation, emission);
    spectrum.setAbsorptionFlag(is_absorption);
    
    if(!spectrum.isValid()){      
        qWarning() << "Data::FluorophoreReader::getSpectrum: Data::Spectrum object of id " << id << " is invalid. Is the Data complete?";
    }

    return spectrum;
}

/*
Fluorophore CacheSpectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is not in source data, returns valid but otherwise useless data
    :param index: the creation index
    :returns: CacheSpectrum object
*/
Data::CacheSpectrum FluorophoreReader::getCacheSpectrum(const Data::Factory& data, const QString& id, unsigned int index) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::Fluorophores);

    fluorophores->beginGroup(id);

    // Load data and transform to Data::Polygon's
    // Needs to do proper selection and flag setting for absorption / excitation curves - for now ignore two_photon
    bool is_absorption = false;
    int excitation_max = fluorophores->value("excitation_max", -1).toInt();
    QStringList excitation_wavelength = fluorophores->value("excitation_wavelength", QVariant()).toStringList();
    QStringList excitation_intensity = fluorophores->value("excitation_intensity", QVariant()).toStringList();

    // If no excitation is available fallback to absorption
    if(excitation_wavelength.empty() || excitation_intensity.empty()){
        is_absorption = true;
        excitation_max = fluorophores->value("absorption_max", -1).toInt();
        excitation_wavelength = fluorophores->value("absorption_wavelength", "0.0,0.0").toStringList();
        excitation_intensity = fluorophores->value("absorption_intensity", "0.0,0.0").toStringList();
    }
    Data::Polygon excitation = this->toPolygon(excitation_wavelength, excitation_intensity);

    int emission_max = fluorophores->value("emission_max", -1).toInt();
    QStringList emission_wavelength = fluorophores->value("emission_wavelength", "0.0,0.0").toStringList();
    QStringList emission_intensity = fluorophores->value("emission_intensity", "0.0,0.0").toStringList();
    Data::Polygon emission = this->toPolygon(emission_wavelength, emission_intensity);

    // Get meta data
    Data::Meta meta;
    meta.excitation_max = excitation_max;
    meta.emission_max = emission_max;

    fluorophores->endGroup();

    // Set linecolor
    if(meta.emission_max != -1){
        emission.setColor(meta.emission_max);
    }else{
        emission.setColor();
    }
    excitation.setColor(emission.color());

    // Verify validity, if invalid return
    Data::Spectrum spectrum(id, excitation, emission);
    spectrum.setAbsorptionFlag(is_absorption);
    
    if(!spectrum.isValid()){      
        qWarning() << "Data::FluorophoreReader::getCacheSpectrum: DataSpectrum object of id " << id << " is invalid. Is the fluorophores.ini complete?";
    }
    
    // Wrap in cache
    Data::CacheSpectrum cache(index, spectrum, meta);

    return cache;
}

/*
Getter for the ordered fluorophore name vector
    :returns: reference to fluor_name
*/
const std::vector<QString>& FluorophoreReader::getFluorName() const {
    return this->fluor_name;
}

/*
Getter for the fluorophore id map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QString>& FluorophoreReader::getFluorID() const {
    return this->fluor_id;
}

/*
Getter for the fluorophore names map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QStringList>& FluorophoreReader::getFluorNames() const {
    return this->fluor_names;
}

/*
(Static) Convers a QStringList into a Data::Polygon object.
    :param list_x: the list containing the values for the x-axis
    :param list_y: the list containing the values for the y-axis
    :returns: a Data::Polygon object containing the curve
*/
Data::Polygon FluorophoreReader::toPolygon(const QStringList& list_x, const QStringList& list_y){
    if(list_x.size() != list_y.size()){
        qWarning() << "Fluorophores::toPolygon: x and y stringlist are of unequal size, cannot be parsed, returns default Data::Polygon";
        return Data::Polygon();
    }
    if(list_x.size() < 2){
        qFatal("Fluorophores::toPolygon: x and y values should each consist of a list atleast two values");
    }

    // Reserve enough space, with two additional entrees, which can be used to make a properly closed polygon
    QPolygonF temp_polygon = QPolygonF();
    temp_polygon.reserve(list_x.size() + 2);
    temp_polygon.resize(list_x.size());

    // Get calculation temporaries
    double x_min = list_x[0].toDouble();
    double x_max = list_x[list_x.size() - 1].toDouble();

    double y_min = 0.0;
    double y_max = 100.0;

    // Fill temp QPolygonF
    for(int i=0; i < list_x.size(); ++i){
        double x = list_x[i].toDouble();
        double y = list_y[i].toDouble();

        // Set QPointF values        
        temp_polygon[i].setX(x);
        temp_polygon[i].setY(y);
    }
    Data::Polygon polygon(x_min, x_max, y_min, y_max, std::move(temp_polygon));

    return polygon;
}

/*
(Static) Debugging functions, unpacks Map/Set for parsing to qDebug()
*/
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, QString>& map){
    qDebug() << "std::unordered_map<QString, QString>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, QStringList>& map){
    qDebug() << "std::unordered_map<QString, QStringList>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void FluorophoreReader::qDebugMap(const std::unordered_map<QString, std::vector<double>>& map){
    qDebug() << "std::unordered_map<QString, std::vector<int>>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void FluorophoreReader::qDebugSet(const std::unordered_set<QString>& map){
    qDebug() << "std::unordered_set<QString>:";
    for(const QString &key: map){
        qDebug() << "{" << key << "}";
    }
}

} // Data namespace 
