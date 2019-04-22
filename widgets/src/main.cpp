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
#include <QString>
#include <iostream>

#include <QDebug>

// Data headers
#include "data_factory.h"
#include "data_styles.h"
#include "data_fluorophores.h"

// Controller widgets
#include "application.h"
#include "main_controller.h"

int main(int argc, char **argv)
{
    // Load QApplication eventManager
    Application app{argc, argv};

    // Load SettingsFactory
    DataFactory settings;

    // Load Default stylesheet (to make the warnings look pretty)
    StyleBuilder style;
    style.loadStyle(settings, "BLACKBLUE");
    app.setStyleSheet(style.getStyleSheet());

    // Run error/warning messages and quits when invalid DataFactory
    settings.execMessages();
    if(!settings.isValid()){
        app.quit();
        return 1;
    }
    
    // Start controller etc
    DataFluorophores data;
    data.load(settings);

    // std::unique_ptr<DataSpectrum> color = data.getSpectrum(settings, "APC");

    // Loads GUI
    Main::Controller gui;
    gui.show();

    return app.exec();
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