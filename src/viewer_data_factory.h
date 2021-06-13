/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_data_factory.h is part of Spectral Viewer
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

/**** DOC ******************************************************************
** Settings and data file parsers
** 
** :class: DataFactory
** A factory that checks existance of data paths. If path is invalid, will fatal
** error upon data object request
** 
** :class: DataError
** A QMessageBox to notify the user of a fatal data error
** 
** :class: DataWarning
** A QMessageBox to warn the user for missing data
** 
***************************************************************************/

#ifndef VIEWER_DATA_FACTORY_H
#define VIEWER_DATA_FACTORY_H

#include <memory>

#include <QString>
#include <QSettings>
#include <QMessageBox>

// Adds hashing function for QStrings
namespace std {
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}

class DataFactory {
    public:
        DataFactory();
        DataFactory(const QString settings, const QString styles, const QString cytometers, const QString fluorophores);

        bool isValid() const;
        bool isWarning() const;

        void execMessages() const;

        QString getPathSettings() const;
        QString getPathDefaults() const;
        QString getPathStyles() const;
        QString getPathCytometers() const;
        QString getPathFluorophores() const;
        std::unique_ptr<QSettings> get(const QString& settings) const;

    private:
        const QString file_settings;
        const QString file_styles;
        const QString file_cytometers;
        const QString file_fluorophores;
        const QString path_exe;

        QString path_settings;
        QString path_defaults;
        QString path_cytometers;
        QString path_fluorophores;
        QString path_styles;

        bool valid_settings;
        bool valid_defaults;
        bool valid_styles;
        bool valid_cytometers;
        bool valid_fluorophores;

        bool error_fatal;
        bool error_warning;
    
        static bool exists(const QString& path_file);
};

class DataError : public QMessageBox {
    public:
        DataError();
        DataError(const QString& message);
};

class DataWarning : public QMessageBox {
    public:
        DataWarning();
        DataWarning(const QString& message);
};  

#endif //VIEWER_DATA_FACTORY_H