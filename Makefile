RM = rm -f
EXE =
OPT = -O3
#
#	Compilation on Windows for Windows
#
ifeq '$(OS)' "Windows_NT"
FLTK_VERSION = lib/fltk-1.4-windows
EXE = .exe
LD = ld
RM = del
CXX = g++
FLAGS =		$(OPT)

CONSOLE =		-Wl,--subsystem,console
LINKFLTK_IMG =	$(CONSOLE) -mwindows -L$(FLTK_VERSION) \
				-lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl -lfltk \
				-lwinmm -lglu32 -lopengl32 -lole32 -luuid \
				-lcomctl32 -lws2_32 -lwinpthread -static

else
#
#	Compilation on MacOSX for MacOSX
#
ifeq '$(shell uname -s)' "Darwin"
FLTK_VERSION = lib/fltk-1.4-macosx
OS = Darwin
CXX = g++
LD = ld
FLAGS =		-Wno-invalid-source-encoding -Wno-deprecated-declarations \
				-I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl \
				-lfltk -framework Cocoa -framework AGL -framework OpenGL \
				-framework ApplicationServices
else
ifeq '$(OS)' "Ubuntu"
#
#	Compilation on Linux for Linux (Ubuntu)
#
FLTK_VERSION = lib/fltk-1.4-ubuntu
CXX = g++
LD = ld
FLAGS =		-DUBUNTU -I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
else
#
#	Compilation on Linux for Linux (Fedora et autres)
#
FLTK_VERSION = lib/fltk-1.4-linux
FMOD = lib/fmod/4.3
CXX = g++
LD = ld
FLAGS =		-I $(FMOD) -I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	$(FMOD)/libfmodex64.so -L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
endif
endif
endif

CXXFLAGS = $(FLAGS) -Wall -I include -I include/immutable -I lib/fmt-9.1.0/include -DFMT_HEADER_ONLY

#
#	Début du Makefile propre au projet
#

SRC	=	src/Labyrinthe.cc	\
		src/Character.cc	\
		src/Chasseur.cc		\
		src/Gardien.cc		\
		src/Position.cc		\
		src/HUD.cc

BASE_H	=	include/immutable/FireBall.h		\
		include/immutable/Sound.h		\
		include/immutable/Environnement.h	\
		include/immutable/Mover.h

H	=	include/Labyrinthe.h	\
		include/Cell.h		\
		include/Mark.h		\
		include/Character.h	\
		include/DummyMover.h	\
		include/Chasseur.h	\
		include/Gardien.h	\
		include/Exception.h	\
		include/HUD.h		\
		include/Position.h	\
		include/Audio.h		\
		include/Util.h		\
		$(BASE_H)

O	=	Labyrinthe.o Character.o Chasseur.o Gardien.o Position.o HUD.o

ifeq '$(OS)' "Darwin"
OPENGL_O = OpenGL-macosx.o
else
ifeq '$(OS)' "Windows_NT"
OPENGL_O = OpenGL-windows.o
else
ifeq '$(OS)' "Ubuntu"
OPENGL_O = OpenGL-ubuntu.o
else
OPENGL_O = OpenGL-linux.o
endif
endif
endif

.SUFFIXES: .h .o .cc $(EXE)

%.o: src/%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

labh$(EXE):	$(O)
ifeq '$(OS)' "Darwin"
	$(CXX) -o $@ $(O) lib/$(OPENGL_O) $(LINKFLTK_IMG)
else
	$(CXX) -no-pie -o $@ $(O) lib/$(OPENGL_O) $(LINKFLTK_IMG)
endif

clean:
	$(RM) labh labh.exe $(O)

Labyrinthe.o:	src/Labyrinthe.cc $(H)
Character.o:	src/Character.cc $(H)
Chasseur.o:	src/Chasseur.cc $(H)
Gardien.o:	src/Gardien.cc $(H)
Position.o:	src/Position.cc include/Position.h include/immutable/Environnement.h
HUD.o:		src/HUD.cc include/Util.h include/Character.h include/Labyrinthe.h
