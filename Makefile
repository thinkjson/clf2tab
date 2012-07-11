CC=g++
CFLAGS=-Wall -Wextra -ansi -pedantic -O3
OBJECTS=clf2tab.o
EXE=clf2tab
INSTALL=/usr/bin/install
INSTALLPATH=/usr/local/bin/

$(EXE) : $(OBJECTS)
	$(CC) -o clf2tab $(OBJECTS)

clf2tab.o : clf2tab.cpp
	$(CC) -c -o clf2tab.o $(CFLAGS) clf2tab.cpp

all : 
	$(MAKE) $(EXE)

rebuild : 
	$(MAKE) clean
	$(MAKE) all

install :
	$(INSTALL) $(EXE) $(INSTALLPATH)

clean :
	rm $(EXE) $(OBJECTS)

