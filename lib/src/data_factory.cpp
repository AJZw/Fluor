/**** General **************************************************************
** Version:    v0.9.12
** Date:       2020-10-28
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

#include "data_factory.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QDebug>

#include <QJsonParseError>

namespace Data {

/*
Constructor - Factory that checks path validity upon construction and returns QSettings
*/
Factory::Factory() :
    file_settings("data/settings.ini"),
    file_styles("data/styles.ini"),
    file_instruments("data/instruments.json"),
    file_fluorophores("data/fluorophores.json"),
    path_exe(QApplication::instance()->applicationDirPath()),
    valid_settings(false), valid_defaults(false), valid_styles(false),
    valid_instruments(false), valid_fluorophores(false),
    error_fatal(false), error_warning(false)
    {
    // Build paths:
    this->path_settings = QDir(this->path_exe).filePath(this->file_settings);
    this->path_defaults = QDir(this->path_exe).filePath(this->file_settings);
    this->path_styles = QDir(this->path_exe).filePath(this->file_styles);
    this->path_instruments = QDir(this->path_exe).filePath(this->file_instruments);
    this->path_fluorophores = QDir(this->path_exe).filePath(this->file_fluorophores);

    // Check path validity
    this->valid_settings = Factory::exists(this->path_settings);
    this->valid_defaults = Factory::exists(this->path_defaults);
    this->valid_styles = Factory::exists(this->path_styles);
    this->valid_instruments = Factory::exists(this->path_instruments);
    this->valid_fluorophores = Factory::exists(this->path_fluorophores);

    // Here you could check for file validity

    // Set error/warning parameters/debug message
    if(!this->valid_settings || !this->valid_defaults){
        qWarning() << "Data::Factory::Factory(): cannot find settings.ini";
        this->error_fatal = true;
    }
    if(!this->valid_styles || !this->valid_instruments || !this->valid_fluorophores){
        qWarning() << "Data::Factory::Factory(): cannot find styles.ini/instruments.json/fluorophores.json";
        this->error_warning = true;
    }
}

/*
Constructor - builds factory with custom relative paths to the data files
    :param settings: relative path to settings file
    :param styles: relative path to styles file
    :param cytometers: relative path to cytometers file
    :param fluorophores: relative path to fluorophores file
*/
Factory::Factory(
    const QString settings,
    const QString styles,
    const QString instruments,
    const QString fluorophores) : 
    file_settings(std::move(settings)),
    file_styles(std::move(styles)),
    file_instruments(std::move(instruments)),
    file_fluorophores(std::move(fluorophores)),
    path_exe(QApplication::applicationDirPath()),
    valid_settings(false), valid_defaults(false), valid_styles(false),
    valid_instruments(false), valid_fluorophores(false),
    error_fatal(false), error_warning(false)
    {
    // Build paths:
    this->path_settings = QDir(this->path_exe).filePath(this->file_settings);
    this->path_defaults = QDir(this->path_exe).filePath(this->file_settings);
    this->path_styles = QDir(this->path_exe).filePath(this->file_styles);
    this->path_instruments = QDir(this->path_exe).filePath(this->file_instruments);
    this->path_fluorophores = QDir(this->path_exe).filePath(this->file_fluorophores);

    // Check path validity
    this->valid_settings = Factory::exists(this->path_settings);
    this->valid_defaults = Factory::exists(this->path_defaults);
    this->valid_styles = Factory::exists(this->path_styles);
    this->valid_instruments = Factory::exists(this->path_instruments);
    this->valid_fluorophores = Factory::exists(this->path_fluorophores);

    // Here you could check for file validity

    // Set error/warning parameters/debug message
    if(!this->valid_settings || !this->valid_defaults){
        qWarning() << "Data::Factory::Factory(): cannot find settings.ini";
        this->error_fatal = true;
    }
    if(!this->valid_styles || !this->valid_instruments || !this->valid_fluorophores){
        qWarning() << "Data::Factory::Factory(): cannot find styles.ini / instruments.json / fluorophores.json";
        this->error_warning = true;
    }
}

/*
Returns whether the SettingsFactory is valid for further use. Aka whether settings.ini could be found.
    :returns bool: valid (true) or not (false)
*/
bool Factory::isValid() const {
    return !this->error_fatal;
}

/*
Returns the validity for the specified data type
    :returns bool: valid (true) or not (false)
*/
bool Factory::isValid(Factory::type data) const {
    switch(data){
    case Factory::Settings:
        return this->valid_settings;
    case Factory::Defaults:
        return this->valid_defaults;
    case Factory::Styles:
        return this->valid_styles;
    case Factory::Instruments:
        return this->valid_instruments;
    case Factory::Fluorophores:
        return this->valid_fluorophores;
    default:
        return false;
    }
}

/*
Returns whether the SettingsFactory is in warning state, aka whether unessential settings are missing.
    :returns bool: warning (true) or not (false)
*/
bool Factory::isWarning() const {
    return this->error_warning;
}

/*
Show SettingsError/SettingsWarning based on file validity
*/
void Factory::execMessages() const {
    if(this->error_fatal){
        QString message = "";
        if(!this->valid_settings){
            message.append("Settings file could not be found.\n");
        }
        if(!this->valid_defaults){
            message.append("Defaults file could not be found.\n");
        }
        if(!this->valid_styles){
            message.append("Styles data could not be found.\n");
        }
        if(!this->valid_instruments){
            message.append("Instruments data could not be found.\n");
        }
        if(!this->valid_fluorophores){
            message.append("Fluorophores data could not be found.\n");
        }
        message.append("\nFluor will now terminate.");
        Data::Error(message).exec();
    }else if(this->error_warning){
        QString message = "";
        if(!this->valid_styles){
            message.append("Styles data could not be found.\n");
        }
        if(!this->valid_instruments){
            message.append("Instruments data could not be found.\n");
        }
        if(!this->valid_fluorophores){
            message.append("Fluorophores data could not be found.\n");
        }
        message.append("\nFluor will continue in limited mode.");
        Data::Warning(message).exec();
    }
}

/*
Getter for path_settings
    :returns: absolute path to the settings source file
*/
QString Factory::getPathSettings() const {
    return this->path_settings;
}

/*
Getter for path_defaults
    :returns: absolute path to the default settings source file
*/
QString Factory::getPathDefaults() const {
    return this->path_defaults;
}

/*
Getter for path_styles
    :returns: absolute path to the styles source file
*/
QString Factory::getPathStyles() const {
    return this->path_styles;
}

/*
Getter for path_cytometers
    :returns: absolute path to the cytometers source file
*/
QString Factory::getPathInstruments() const {
    return this->path_instruments;
}

/*
Getter for path_fluorophores
    :returns: absolute path to the fluorophores source file
*/
QString Factory::getPathFluorophores() const {
    return this->path_fluorophores;
}

/*
Builds the QSettings as specified by the parameter.
This function (again) checks whether the file exists before opening it. Because in the timespan
between factory construction and the get call the file could have been removed.
    :param settings: a enum specifying the settings type
    :returns: a QSettings object
*/
std::unique_ptr<QSettings> Factory::get(const Factory::type type) const {
    switch(type){
    case Factory::Settings:{
        if(this->valid_settings){
            if(!Factory::exists(this->path_settings)){
                qFatal("Data::Factory::get(): settings data file does not exist anymore");
            }
            return std::make_unique<QSettings>(this->path_settings, QSettings::IniFormat);
        }else{
            qFatal("Data::Factory::get(): requested invalid data source");
        }
        break;
    }
    case Factory::Defaults:{
        if(this->valid_defaults){
            if(!Factory::exists(this->path_defaults)){
                qFatal("Data::Factory::get(): defaults data file does not exist anymore");
            }
            return std::make_unique<QSettings>(this->path_defaults, QSettings::IniFormat);
        }else{
            qFatal("Data::Factory::get(): requested invalid data source");
        }
        break;
    }
    case Factory::Styles:{
        if(this->valid_styles){
            if(!Factory::exists(this->path_styles)){
                qFatal("Data::Factory::get(): styles data file does not exist anymore");
            }
            return std::make_unique<QSettings>(this->path_styles, QSettings::IniFormat);
        }else{
            qFatal("Data::Factory::get(): requested invalid data source");
        }
        break;
    }
    case Factory::Instruments:{
        qFatal("Data::Factory::get(): invalid for instruments Factory::type, use get_json()");
        break;
    }
    case Factory::Fluorophores:{
        qFatal("Data::Factory::get(): invalid for fluorophores Factory::type, use get_json()");
        break;
    }
    default:
        qFatal("Data::Factory::get(): unknown settings object, cannot return QSetting");
    }
    return std::make_unique<QSettings>();
}

/*
Builds the QJSONDocument as specified by the parameter.
This function (again) checks whether the file exists before opening it. Because in the timespan
between factory construction and the get call the file could have been removed.
    :param settings: a enum specifying the settings type
    :returns: a QJsonDocument object
*/
QJsonDocument Factory::get_json(const Factory::type type) const {
    switch(type){
    case Factory::Settings:
    case Factory::Defaults:
    case Factory::Styles:{
            qFatal("Data::Factory::get_json(): invalid for settings/defaults/styles data type, use get()");
        break;
        }
    case Factory::Fluorophores:{
        if(this->valid_fluorophores){
            if(!Factory::exists(this->path_fluorophores)){
                qFatal("Data::Factory::get_json(): fluorophores data file does not exist anymore");
            }

            QFile file(this->path_fluorophores);
            if(!file.open(QIODevice::ReadOnly)){
                qWarning("Data::Factory::get_json(): couldnt open fluorophores.json");
                return QJsonDocument::fromVariant(QVariant());
            }

            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
            if(error.error != QJsonParseError::NoError){
                qWarning() << "Data::Factory::get_json: invalid fluorophore.json:" << error.errorString();
            }

            return document;
        }else{
            qFatal("Data::Factory::get_json(): requested invalid data source");
        }
        break;
    }
    case Factory::Instruments:{
        if(this->valid_instruments){
            if(!Factory::exists(this->path_instruments)){
                qFatal("Data::Factory::get_json(): instruments data file does not exist anymore");
            }

            QFile file(this->path_instruments);
            if(!file.open(QIODevice::ReadOnly)){
                qWarning("Data::Factory::get_json(): couldnt open instruments.json");
                return QJsonDocument::fromVariant(QVariant());
            }

            QJsonParseError error;
            QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
            if(error.error != QJsonParseError::NoError){
                qWarning() << "Data::Factory::get_json: invalid instrument.json:" << error.errorString();
            }

            return document;
        }else{
            qFatal("Data::Factory::get_json(): requested invalid data source");
        }
        break;
    }
    default:
        qFatal("Data::Factory::get_json(): unknown type requested, cannot return a valid QJsonDocument");
    }
    return QJsonDocument::fromVariant(QVariant());
}

/*
(Static) Convenience function: checks whether file exists
    :returns [bool]: whether file exists
*/
bool Factory::exists(const QString& path_file){
    bool file_exists = QDir().exists(path_file);
    return file_exists;
}

/*
QMessageBox for showing a standard fatal error message
*/
Error::Error(){
    // Set minimum size to prevent a setGeometry error
    this->setMinimumSize(200, 200);
    this->setText("Spectral Viewer encountered a fatal error");
    this->setInformativeText("Spectral Viewer will now close.");
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Critical);
}

/*
QMessageBox for showing a customized fatal error message
    :param message: information message to show
*/
Error::Error(const QString& message){
    this->setMinimumSize(200, 200);
    this->setText("Spectral Viewer encountered a fatal error.");
    this->setInformativeText(message);
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Critical);
}

/*
QMessageBox for showing a standard warning error message
*/
Warning::Warning(){
    this->setMinimumSize(200, 200);
    this->setText("Spectral Viewer encountered a problem.");
    this->setInformativeText("Databases are not found. Data may be missing.");
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Warning);
}

/*
QMessageBox for showing a customized warning error message
    :param message: information message to show
*/
Warning::Warning(const QString& message){
    // Set minimum size to prevent a setGeometry error
    this->setMinimumSize(200, 200);
    this->setText("Spectral Viewer encountered a problem.");
    this->setInformativeText(message);
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Warning);
}

}   // Data namespace
