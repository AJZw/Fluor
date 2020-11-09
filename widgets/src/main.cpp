/**** General **************************************************************
** Version:    v0.9.13
** Date:       2020-11-09
** Author:     AJ Zwijnenburg
** Copyright:  Copyright (C) 2020 - AJ Zwijnenburg
** License:    LGPLv3
***************************************************************************/

/**** DOC ******************************************************************
** Starts the viewer's QApplication and central Window
***************************************************************************/

#include "application.h"
#include "data_factory.h"
#include "state_program.h"

#include <QDebug>

int main(int argc, char **argv)
{
    // Load QApplication eventManager
    Application APP(argc, argv);

    // Load SettingsFactory
    Data::Factory FACTORY;

    // Load Default stylesheet (to make the warnings look pretty)
    Data::StyleBuilder STYLE;
    APP.setStyleSheet(STYLE.getStyleSheet());

    // Run error/warning messages and quits when invalid Data::Factory
    if(!FACTORY.isValid()){
        FACTORY.execMessages();
        APP.quit();
        return 1;
    }else if(FACTORY.isWarning()){
        FACTORY.execMessages();
    }

    State::Program STATE(FACTORY);

    return APP.exec();
}
