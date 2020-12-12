include $(ROOT)/usr/include/make/PRdefs

N64KITDIR    = c:/nintendo/n64kit
NUSYSINCDIR  = $(N64KITDIR)/nusys/include
NUSYSLIBDIR  = $(N64KITDIR)/nusys/lib
NUSTDINCDIR  = $(N64KITDIR)/nustd/include
NUSTDLIBDIR  = $(N64KITDIR)/nustd/lib

LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include
CC  = gcc
LD  = ld
MAKEROM = mild

NUAUDIOLIB = -lnualstl_n_d -ln_gmus_d -ln_gaudio_sc

LCDEFS =	-DF3DEX_GBI_2 # -DNU_DEBUG
LCINCS =	-I. -I./src -I$(NUSYSINCDIR) -I$(NUSTDINCDIR) -I$(ROOT)/usr/include/PR
LCOPTS =	-G 0 -O2

LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -L$(NUSTDLIBDIR) $(NUAUDIOLIB) -lnustd_d -lnusys_d -lgultra_d -L$(GCCDIR)/mipse/lib -lkmc

OPTIMIZER =	-g

APP =		riistahillo.out

TARGETS =	riistahillo.n64

CODEFILES   = 	$(wildcard src/*.c)

CODEOBJECTS =	$(CODEFILES:src/%.c=build/%.o)  $(NUSYSLIBDIR)/nusys.o

CODESEGMENT =	build/codesegment.o

OBJECTS =	$(CODESEGMENT)

default:        $(TARGETS)

build/%.o : src/%.c
		$(CC) $(CFLAGS) $< -o $@

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	$(OBJECTS)
		$(MAKEROM) src/spec -I$(NUSYSINCDIR) -r $(TARGETS) -e $(APP)

ifdef MAKEMASK
		makemask $(TARGETS)
endif
