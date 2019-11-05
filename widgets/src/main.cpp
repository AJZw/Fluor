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

#include "application.h"
#include "data_factory.h"
#include "state.h"


#include <QDebug>


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

    State::State STATE(FACTORY);

    return APP.exec();
}
