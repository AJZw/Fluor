/**** General **************************************************************
** Version:    v0.9.1
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_fluorophores.h"

#include <QDebug>
#include <QVariant>

namespace Data {

/*
Constructor: Construct the fluorophore data types
*/
Fluorophores::Fluorophores() :
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
void Fluorophores::load(Data::Factory& data){
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::fluorophores);

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
    std::sort(this->fluor_name.begin(), this->fluor_name.end(), [](const QString& left, const QString& right){return left.toLower() < right.toLower();});
    
    // Sets the loaded flag
    data_loaded = true;
}

/*
Unallocates memory of all loaded data
*/
void Fluorophores::unload(){
    std::vector<QString>().swap(this->fluor_name);
    std::unordered_map<QString, QString>().swap(this->fluor_id);
    std::unordered_map<QString, QStringList>().swap(this->fluor_names);
    data_loaded = false;
}

/*
Returns whether Fluorophores is valid (has loaded fluorophore data)
    :returns: validity
*/
bool Fluorophores::isValid() const {
    return this->data_loaded;
}

/*
Fluorophore spectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is invalid returns are empty vectors
    :returns: DataSpectrum object
*/
Data::Spectrum Fluorophores::getSpectrum(const Data::Factory& data, const QString& id) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::fluorophores);

    fluorophores->beginGroup(id);

    // Load data and transform to vector of double
    std::vector<double> excitation_wavelength{Fluorophores::toStdVector(fluorophores->value("excitation_wavelength", "0.0").toStringList())};
    std::vector<double> excitation_intensity{Fluorophores::toStdVector(fluorophores->value("excitation_intensity", "0.0").toStringList())};
    std::vector<double> emission_wavelength{Fluorophores::toStdVector(fluorophores->value("emission_wavelength", "0.0").toStringList())};
    std::vector<double> emission_intensity{Fluorophores::toStdVector(fluorophores->value("emission_intensity", "0.0").toStringList())};

    fluorophores->endGroup();

    // Check if valid -> if not send qWarning and build a new (empty) DataSpectrum
    Data::Spectrum spectrum(id, excitation_wavelength, excitation_intensity, emission_wavelength, emission_intensity);

    // Verify validity, if invalid return
    if(!spectrum.isValid()){
        qWarning() << "Data::Fluorophores::getSpectrum: DataSpectrum object of id " << id << " is invalid.";

        spectrum.setToZero();
    }
    return spectrum;
}

/*
Fluorophore CacheSpectrum accessor
    :param data: the DataFactory which builds a QSettings object containing the fluorophore data
    :param id: the fluorophore id, if id is invalid returns are empty vectors
    :param name: the fluorophore name
    :param index: the creation index
    :returns: CacheSpectrum object
*/
Data::CacheSpectrum Fluorophores::getCacheSpectrum(const Data::Factory& data, const QString& id, const QString& name, unsigned int index) const {
    // Retrieve QSetting
    std::unique_ptr<QSettings> fluorophores = data.get(Data::Factory::fluorophores);

    fluorophores->beginGroup(id);

    // Load data and transform to vector of double
    std::vector<double> excitation_wavelength{Fluorophores::toStdVector(fluorophores->value("excitation_wavelength", "0.0").toStringList())};
    std::vector<double> excitation_intensity{Fluorophores::toStdVector(fluorophores->value("excitation_intensity", "0.0").toStringList())};
    std::vector<double> emission_wavelength{Fluorophores::toStdVector(fluorophores->value("emission_wavelength", "0.0").toStringList())};
    std::vector<double> emission_intensity{Fluorophores::toStdVector(fluorophores->value("emission_intensity", "0.0").toStringList())};

    Data::Meta meta;
    meta.excitation_max = fluorophores->value("excitation_max", -1).toInt();
    meta.emission_max = fluorophores->value("emission_max", -1).toInt();

    fluorophores->endGroup();

    // Verify validity, if invalid return
    Data::Spectrum spectrum(id, excitation_wavelength, excitation_intensity, emission_wavelength, emission_intensity);
    
    if(!spectrum.isValid()){      
        qWarning() << "Data::Fluorophores::getSpectrum: DataSpectrum object of id " << id << " is invalid.";
        
        spectrum.setToZero();
    }
    
    Data::CacheSpectrum cache(index, name, std::move(spectrum), std::move(meta));

    return(cache);
}

/*
Getter for the ordered fluorophore name vector
    :returns: reference to fluor_name
*/
const std::vector<QString>& Fluorophores::getFluorName() const {
    return this->fluor_name;
}

/*
Getter for the fluorophore id map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QString>& Fluorophores::getFluorID() const {
    return this->fluor_id;
}

/*
Getter for the fluorophore names map
    :returns: reference to fluor_id
*/
const std::unordered_map<QString, QStringList>& Fluorophores::getFluorNames() const {
    return this->fluor_names;
}

/*
(Static) Converts a QStringList into a std::vector of doubles
    :returns: vector of double typeconverted QStrings
*/
std::vector<double> Fluorophores::toStdVector(const QStringList& stringlist){
    std::vector<double> double_vector(static_cast<std::size_t>(stringlist.size()));

    // Iterate using index, because I need to read the input to the output at the same time   
    for(int i=0; i<stringlist.size(); ++i){
        double_vector[static_cast<std::size_t>(i)] = stringlist[i].toDouble();
    }

    return double_vector;
} 

/*
(Static) Debugging functions, unpacks Map/Set for parsing to qDebug()
*/
void Fluorophores::qDebugMap(const std::unordered_map<QString, QString>& map){
    qDebug() << "std::unordered_map<QString, QString>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void Fluorophores::qDebugMap(const std::unordered_map<QString, QStringList>& map){
    qDebug() << "std::unordered_map<QString, QStringList>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void Fluorophores::qDebugMap(const std::unordered_map<QString, std::vector<double>>& map){
    qDebug() << "std::unordered_map<QString, std::vector<int>>:";
    for(const auto &pair : map){
        qDebug() << "{" << pair.first << ":" << pair.second << "}";
    }
}
void Fluorophores::qDebugSet(const std::unordered_set<QString>& map){
    qDebug() << "std::unordered_set<QString>:";
    for(const QString &key: map){
        qDebug() << "{" << key << "}";
    }
}

} // Data namespace 
