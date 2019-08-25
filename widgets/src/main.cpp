/**** General **************************************************************
** Version:    v0.9.2
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2019 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** DOC ******************************************************************
** Starts the viewer's QApplication and central Window
***************************************************************************/

#include <QApplication>
#include <QObject>
#include <QString>
#include <iostream>

#include <QDebug>

// Data headers
#include "data_factory.h"
#include "data_styles.h"
#include "data_fluorophores.h"

// Cache header
#include "cache.h"

// Controller widgets
#include "application.h"
#include "main_controller.h"

int main(int argc, char **argv)
{
    // Load QApplication eventManager
    Application APP{argc, argv};

    // Load SettingsFactory
    Data::Factory FACTORY;

    // Load Default stylesheet (to make the warnings look pretty)
    Data::Style::Builder STYLE;
    APP.setStyleSheet(STYLE.getStyleSheet());

    // Run error/warning messages and quits when invalid DataFactory
    if(!FACTORY.isValid()){
        FACTORY.execMessages();
        APP.quit();
        return 1;
    }

    // Factory is fully valid, load proper stylesheet
    STYLE.loadStyle(FACTORY, "BLACKBLUE");
    APP.setStyleSheet(STYLE.getStyleSheet());
    
    // Start data import etc
    Data::Fluorophores DATA;
    DATA.load(FACTORY);

    // Build Cache
    Cache::Cache CACHE(FACTORY, DATA);

    // Loads GUI
    Main::Controller GUI;
    GUI.show();

    // Connect Cache to GUI
    QObject::connect(&GUI, &Main::Controller::sendCacheAdd, &CACHE, &Cache::Cache::cacheAdd);
    QObject::connect(&GUI, &Main::Controller::sendCacheRemove, &CACHE, &Cache::Cache::cacheRemove);
    QObject::connect(&GUI, &Main::Controller::sendCacheRequestUpdate, &CACHE, &Cache::Cache::cacheRequestUpdate);
    
    QObject::connect(&CACHE, &Cache::Cache::cacheSync, &GUI, &Main::Controller::receiveCacheSync);
    QObject::connect(&CACHE, &Cache::Cache::cacheUpdate, &GUI, &Main::Controller::receiveCacheUpdate);

    // Start data loading
    emit GUI.sendData(DATA);

    

    return APP.exec();
}


/*

import sys
from timeit import default_timer as timer
import viewer_main_window
import viewer_parsers

if __name__ == "__main__":
    print("Starting APP and GUI", flush=True)
    START = timer()

    APP = viewer_main_window.Application(sys.argv)
    SETTINGS = viewer_parsers.Settings()
    APP.addSettings(SETTINGS)

    GUI = viewer_main_window.Window(SETTINGS)
    GUI.show()

    END = timer()
    print("Loaded:", END - START, flush=True)
    sys.exit(APP.exec_())

*/