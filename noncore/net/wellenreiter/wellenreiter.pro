DESTDIR         = $(OPIEDIR)/bin
TEMPLATE        = app
CONFIG          = qt warn_on debug
#CONFIG         = qt warn_on release
HEADERS         = gui/mainwindowbase.h gui/wellenreiter.h gui/scanlistitem.h
SOURCES         = gui/main.cpp gui/wellenreiter.cpp gui/scanlistitem.cpp
INCLUDEPATH     += $(OPIEDIR)/include gui daemon
DEPENDPATH      += $(OPIEDIR)/include gui daemon
LIBS            += -lqpe
INTERFACES      = gui/wellenreiterbase.ui
TARGET          = wellenreiter
