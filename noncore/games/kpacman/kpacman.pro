CONFIG		= qt quick-app
HEADERS		= kpacmanwidget.h \
		    referee.h \
		    status.h \
		    painter.h \
		    score.h \
		    pacman.h \
		    monster.h \
		    keys.h \
		    fruit.h \
		    energizer.h \
		    board.h \
		    bitfont.h \
		    kpacman.h \
		    bitmaps.h \
		    colors.h \
		    portable.h
SOURCES		= kpacmanwidget.cpp \
		    referee.cpp \
		    status.cpp \
		    painter.cpp \
		    score.cpp \
		    pacman.cpp \
		    monster.cpp \
		    keys.cpp \
		    fruit.cpp \
		    energizer.cpp \
		    board.cpp \
		    bitfont.cpp \
		    kpacman.cpp \
		    main.cpp 
INCLUDEPATH	+= $(OPIEDIR)/include
DEPENDPATH	+= $(OPIEDIR)/include
LIBS            += -lqpe -lopiecore2
TARGET		= kpacman

include( $(OPIEDIR)/include.pro )
