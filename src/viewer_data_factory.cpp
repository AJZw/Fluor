/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_data_factory.cpp is part of Spectral Viewer
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

#include "viewer_data_factory.h"

#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QDebug>

/*
Constructor - Factory that checks path validity upon construction and returns QSettings
*/
DataFactory::DataFactory() :
    file_settings("data/settings.ini"),
    file_styles("data/styles.ini"),
    file_cytometers("data/cytometers.ini"),
    file_fluorophores("data/fluorophores.ini"),
    path_exe(QApplication::applicationDirPath()),
    valid_settings(false), valid_defaults(false), valid_styles(false),
    valid_cytometers(false), valid_fluorophores(false),
    error_fatal(false), error_warning(false)
    {
    // Build paths:
    this->path_settings = QDir(this->path_exe).filePath(this->file_settings);
    this->path_defaults = QDir(this->path_exe).filePath(this->file_settings);
    this->path_styles = QDir(this->path_exe).filePath(this->file_styles);
    this->path_cytometers = QDir(this->path_exe).filePath(this->file_cytometers);
    this->path_fluorophores = QDir(this->path_exe).filePath(this->file_fluorophores);

    // Check path validity
    this->valid_settings = DataFactory::exists(this->path_settings);
    this->valid_defaults = DataFactory::exists(this->path_defaults);
    this->valid_styles = DataFactory::exists(this->path_styles);
    this->valid_cytometers = DataFactory::exists(this->path_cytometers);
    this->valid_fluorophores = DataFactory::exists(this->path_fluorophores);

    // Here you could check for file validity

    // Set error/warning parameters/debug message
    if(!this->valid_settings || !this->valid_defaults){
        qWarning() << "SettingsFactory::SettingsFactory(): cannot find settings.ini";
        this->error_fatal = true;
    }
    if(!this->valid_styles || !this->valid_cytometers || !this->valid_fluorophores){
        qWarning() << "SettingsFactory::SettingsFactory(): cannot find styles.ini/cytometers.ini/fluorophores.ini";
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
DataFactory::DataFactory(
    const QString settings,
    const QString styles,
    const QString cytometers,
    const QString fluorophores) : 
    file_settings(settings),
    file_styles(styles),
    file_cytometers(cytometers),
    file_fluorophores(fluorophores),
    path_exe(QApplication::applicationDirPath()),
    valid_settings(false), valid_defaults(false), valid_styles(false),
    valid_cytometers(false), valid_fluorophores(false),
    error_fatal(false), error_warning(false)
    {
    // Build paths:
    this->path_settings = QDir(this->path_exe).filePath(this->file_settings);
    this->path_defaults = QDir(this->path_exe).filePath(this->file_settings);
    this->path_styles = QDir(this->path_exe).filePath(this->file_styles);
    this->path_cytometers = QDir(this->path_exe).filePath(this->file_cytometers);
    this->path_fluorophores = QDir(this->path_exe).filePath(this->file_fluorophores);

    // Check path validity
    this->valid_settings = DataFactory::exists(this->path_settings);
    this->valid_defaults = DataFactory::exists(this->path_defaults);
    this->valid_styles = DataFactory::exists(this->path_styles);
    this->valid_cytometers = DataFactory::exists(this->path_cytometers);
    this->valid_fluorophores = DataFactory::exists(this->path_fluorophores);

    // Here you could check for file validity

    // Set error/warning parameters/debug message
    if(!this->valid_settings || !this->valid_defaults){
        qWarning() << "SettingsFactory::SettingsFactory(): cannot find settings.ini";
        this->error_fatal = true;
    }
    if(!this->valid_styles || !this->valid_cytometers || !this->valid_fluorophores){
        qWarning() << "SettingsFactory::SettingsFactory(): cannot find styles.ini / cytometers.ini / fluorophores.ini";
        this->error_warning = true;
    }
}

/*
Returns whether the SettingsFactory is valid for further use. Aka whether settings.ini could be found.
    :returns bool: valid (true) or not (false)
*/
bool DataFactory::isValid() const {
    return !this->error_fatal;
}

/*
Returns whether the SettingsFactory is in warning state, aka whether unessential settings are missing.
    :returns bool: warning (true) or not (false)
*/
bool DataFactory::isWarning() const {
    return this->error_warning;
}

/*
Show SettingsError/SettingsWarning based on file validity
*/
void DataFactory::execMessages() const {
    if(this->error_fatal){
        QString message = "";
        if(!this->valid_settings){
            message.append("Settings file could not be found.\n");
        }
        if(!this->valid_defaults){
            message.append("Defaults file could not be found.\n");
        }
        message.append("\nSpectral viewer will now terminate.");
        DataError(message).exec();
    }else if(this->error_warning){
        QString message = "";
        if(!this->valid_styles){
            message.append("Styles data could not be found.\n");
        }
        if(!this->valid_cytometers){
            message.append("Cytometers data could not be found.\n");
        }
        if(!this->valid_fluorophores){
            message.append("Fluorophores data could not be found.\n");
        }
        message.append("\nSpectral viewer will continue in limited mode.");
        DataWarning(message).exec();
    }
}

/*
Getter for path_settings
    :returns: absolute path to the settings source file
*/
QString DataFactory::getPathSettings() const {
    return this->path_settings;
}

/*
Getter for path_defaults
    :returns: absolute path to the default settings source file
*/
QString DataFactory::getPathDefaults() const {
    return this->path_defaults;
}

/*
Getter for path_styles
    :returns: absolute path to the styles source file
*/
QString DataFactory::getPathStyles() const {
    return this->path_styles;
}

/*
Getter for path_cytometers
    :returns: absolute path to the cytometers source file
*/
QString DataFactory::getPathCytometers() const {
    return this->path_cytometers;
}

/*
Getter for path_fluorophores
    :returns: absolute path to the fluorophores source file
*/
QString DataFactory::getPathFluorophores() const {
    return this->path_fluorophores;
}

/*
Builds the QSettings as specified by the parameter
    :param settings: a string that specifies the settings file to return
    :returns: a QSettings object
*/
std::unique_ptr<QSettings> DataFactory::get(const QString& settings) const {
    if(settings == "settings"){
        if(this->valid_settings){
            return std::make_unique<QSettings>(this->path_settings, QSettings::IniFormat);
        }else{
            qFatal("SettingsFactory::get(): requested invalid data source");
        }
    }else if(settings == "defaults"){
        if(this->valid_defaults){
            return std::make_unique<QSettings>(this->path_defaults, QSettings::IniFormat);
        }else{
            qFatal("SettingsFactory::get(): requested invalid data source");
        }
    }else if(settings == "styles"){
        if(this->valid_styles){
            return std::make_unique<QSettings>(this->path_styles, QSettings::IniFormat);
        }else{
            qFatal("SettingsFactory::get(): requested invalid data source");
        }
    }else if(settings == "cytometers"){
        if(this->valid_cytometers){
            return std::make_unique<QSettings>(this->path_cytometers, QSettings::IniFormat);
        }else{
            qFatal("SettingsFactory::get(): requested invalid data source");
        }
    }else if(settings == "fluorophores"){
        if(this->valid_fluorophores){
            return std::make_unique<QSettings>(this->path_fluorophores, QSettings::IniFormat);
        }else{
            qFatal("SettingsFactory::get(): requested invalid data source");
        }
    }else{
        qFatal("SettingsFactory::get(): unknown settings object, cannot return a QSetting");
    }
    return(std::make_unique<QSettings>());
}

/*
(Static) Convenience function: checks if file exists
    :returns [bool]: whether file exists or not
*/
bool DataFactory::exists(const QString& path_file){
    bool file_exists = QDir().exists(path_file);
    return file_exists;
}

/*
QMessageBox for showing a standard fatal error message
*/
DataError::DataError(){
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
DataError::DataError(const QString& message){
    this->setText("Spectral Viewer encountered a fatal error.");
    this->setInformativeText(message);
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Critical);
}

/*
QMessageBox for showing a standard warning error message
*/
DataWarning::DataWarning(){
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
DataWarning::DataWarning(const QString& message){
    this->setText("Spectral Viewer encountered a problem.");
    this->setInformativeText(message);
    this->setStandardButtons(QMessageBox::Ok);
    this->setDefaultButton(QMessageBox::Ok);
    this->setIcon(QMessageBox::Warning);
}
