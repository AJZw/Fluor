/**** General **************************************************************
** Version:    v0.9.0
** Date:       2018-12-08
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2018 - AJ Zwijnenburg
** License:    GPLv3
***************************************************************************/

/**** GPLv3 License ******************************************************** 
** viewer_main.cpp is part of Spectral Viewer
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

/**** To do ****************************************************************
** General:
** > add code signing
** >> Make licensing window
** >> Make about window
** Implement the ASCII/CSV transformers
**   Interpolation
**   Normalize Y to 100%
**   Smoothen
** Add Controller
** Write UNIT tests -> some internal changes screwed up bar/laser with graph connections; testing prevents these errors
** 
** Add a way to intercept all Errors and parse those to a popup window / possible?
**   Probably not, but implement logging as a better alternative!
**   Including logging window! which is openable independently and is updated on every log event
** 
** A way to save the view
** A way to load the view
** A way to export the plots
** 
** Parsers
** > TEST QSettings settings on macintosh, does it follow the QApp defined names
** 
** Window:
** mainwindowpressevent needs some love -> build controller
** 
** Main:
** if fluorophore name too long gets squeezed by scroll_widget, way to solve this nicely?
** 
** Bar:
** Properly toggle_emission/excitation icons -> properly resize button / icon to work with different font sizes (on macOS you need to use QSize(25,25) for example)
** BarLayout - if the graph reloads with anything else but 1 graph, the +/- buttons are faulty. Fix when implemented controller
** 
** Laser:
** > If selected plot with laser -> see if entree in QModel, if so -> select
** showPopup -> see if I can get the global coordinates from a layout instead of being forced to use a widget
** 
** Graph:
** Save graph properties and reload upon opening -> for now disabled cause it breaks the BarLayout
** GraphPlot in Square instead of line upon certain size / option?
** 'black' axes face_color option allow for white out of visible spectrum plotting
** Check visibility from buttons settings not universal ones ...
**   this works now, but maybe more sexy alternative (instead of to reading the first plot)
** Add Object for non-sequential lasers
**   Settings have been adjusted to allow this, now the alternative GraphPlot and switching
** Matplotlib is slow(ish)
** Implement PyQtGraph or GPLv3 license PyQtChart
** Selection in Plot -> see statistics of each plot
** More indepth detector plotting, also taking into account the dichromatic mirrors etc.
**
** Data:
** Implement validity check upon new data: check if sorted low->high. 
** I use binary searches for speedy data manipulation. This require a sorted std::vector
***************************************************************************/

/**** DOC ******************************************************************
** Starts the viewer's QApplication and central Window
***************************************************************************/

#include <QApplication>
#include <QString>
#include <iostream>

#include <QDebug>

// Data headers
#include "viewer_data_factory.h"
#include "viewer_data_styles.h"
#include "viewer_data_fluorophores.h"

// Controller widgets
#include "viewer_central_window.h"

int main(int argc, char **argv)
{
    // Load QApplication eventManager
    Application app (argc, argv);

    // Load SettingsFactory
    DataFactory settings;

    // Load Default stylesheet (to make the warnings look pretty)
    StyleBuilder style;
    style.loadStyle(settings, "BLACKBLUEWHITE");
    app.setStyleSheet(style.getStyleSheet());

    // Run error/warning messages and quit incase of invalid DataFactory
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
    MainWindow gui;
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