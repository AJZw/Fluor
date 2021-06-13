TEMPLATE += app
CONFIG += debug console c++14
QT += widgets
HEADER += src/viewer_central_window.h src/viewer_data_factory.h src/viewer_data_fluorophores.h src/viewer_data_style.h
SOURCES += src/viewer_main.cpp src/viewer_central_window.cpp src/viewer_data_factory.cpp src/viewer_data_fluorophores.cpp src/viewer_data_style.cpp
RESOURCES = src/viewer_resources.qrc

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

OBJECTS_DIR = $${DESTDIR}/.generatedfiles
MOC_DIR = $${DESTDIR}/.generatedfiles
RCC_DIR = $${DESTDIR}/.generatedfiles
UI_DIR = $${DESTDIR}/.generatedfiles
