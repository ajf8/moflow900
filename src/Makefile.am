INCLUDES = --pedantic -Wall -std=c99 -O2 $(jsonglib_CFLAGS) $(moflow_CFLAGS) $(gthread_CFLAGS) $(gtk_CFLAGS) $(hildon_CFLAGS)

pixmapdir = $(datadir)/pixmaps/moflow
pixmap_DATA = images/arrowdown.png images/arrowup.png images/back.png images/cctv.png images/close.png images/down.png images/forward.png images/greencircle.png images/itsbroken.png images/motorway.png images/orangecircle.png images/queueing.png images/refresh.png images/roadworks.png images/up.png

bin_PROGRAMS = moflow
moflow_SOURCES = main.c mainwindow.c wizard.c camera.c pixbufstore.c util.c
moflow_LDFLAGS = -lcurl $(moflow_LIBS) $(jsonglib_LIBS) $(gtk_LIBS) $(gthread_LIBS) $(hildon_LIBS) -pthread

icondir = $(datadir)/icons
dist_icon_DATA = images/moflow900.png
 
desktopdir = $(datadir)/applications/hildon
dist_desktop_DATA = moflow900.desktop
