CXX = g++
AR = ar
LD = g++

INC = -Iinclude -Isrc
CFLAGS = -Wall -fexceptions -std=c++17 -O2
LIB = -lzip -ltinyxml2
LDFLAGS = -s
OBJDIR = obj/Release
OUT = bin/Release/KiCad2sycira

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

OBJ = $(OBJDIR)/src/CreatMaximaSession.o $(OBJDIR)/src/KiCad2sycira.o

all: release

before: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR)/src || mkdir -p $(OBJDIR)/src

release: before out

out: before $(OBJ)
	$(LD) -o $(OUT) $(OBJ)  $(LDFLAGS) $(LIB)

$(OBJDIR)/src/CreatMaximaSession.o: src/CreatMaximaSession.cpp
	$(CXX) $(CFLAGS) $(INC) -c src/CreatMaximaSession.cpp -o $(OBJDIR)/src/CreatMaximaSession.o

$(OBJDIR)/src/KiCad2sycira.o: src/KiCad2sycira.cpp
	$(CXX) $(CFLAGS) $(INC) -c src/KiCad2sycira.cpp -o $(OBJDIR)/src/KiCad2sycira.o

clean: 
	rm -f $(OBJ) $(OUT)
	rm -rf bin/Release
	rm -rf $(OBJDIR)/src

install:
	install -d $(PREFIX)/bin/
	install -m 755 $(OUT) $(PREFIX)/bin/
	install -d $(PREFIX)/share/KiCad2sycira/
	install -m 644 sycira.lib $(PREFIX)/share/KiCad2sycira/

.PHONY: before clean

