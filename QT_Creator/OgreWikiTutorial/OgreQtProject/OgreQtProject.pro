TEMPLATE = app
TARGET = OgreQtProject

DEFINES -= UNICODE

CONFIG -= qt

unix {
    # You may need to change this include directory
    INCLUDEPATH += /usr/include/OGRE
    CONFIG += link_pkgconfig
    PKGCONFIG += OGRE
}
win32 {
    LIBS *= user32.lib
    LIBS += -L$(OGRE_HOME)\\boost\\lib
    release:LIBS += -L$(OGRE_HOME)\\lib\\release
    debug:LIBS += -L$(OGRE_HOME)\\lib\\debug
    INCLUDEPATH += $(OGRE_HOME)\\include
    INCLUDEPATH += $(OGRE_HOME)\\include\\OIS
    INCLUDEPATH += $(OGRE_HOME)\\include\\OGRE
    INCLUDEPATH += $(OGRE_HOME)\\boost
    # If you are using Ogre 1.9 also include the following line:
    INCLUDEPATH += $(OGRE_HOME)\\include\\OGRE\\Overlay
}
debug {
    TARGET = $$join(TARGET,,,d)
    LIBS *= -lOgreMain_d -lOIS_d
    # If you are using Ogre 1.9 also include -lOgreOverlay_d, like this:
    LIBS *= -lOgreMain_d -lOIS_d -lOgreOverlay_d
}
release {
    LIBS *= -lOgreMain -lOIS
    # If you are using Ogre 1.9 also include -lOgreOverlay, like this:
    LIBS *= -lOgreMain -lOIS -lOgreOverlay
}


HEADERS += \
    BaseApplication.h \
    TutorialApplication.h

SOURCES += \
    BaseApplication.cpp \
    TutorialApplication.cpp
