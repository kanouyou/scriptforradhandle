
TARGET = respro.exe
CXX    = g++

#MAINSRC = main.cxx
#LIBSRCS = TCometGeoConstruction.cxx

SRCS = $(wildcard *.cxx)
OBJS = $(SRCS:.cxx=.o)
PREFIX = .

CXXLIBS  = 
CXXFLAGS = -O3 -Wall #-std=c++11

CXXLIBS += $(shell root-config --evelibs)
CXXFLAGS += $(shell root-config --cflags)

.PHONY: all install clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXLIBS) $^ -o $@

#$(OBJS): $(SRCS)
%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) $(TARGET) $(OBJS)

install:
	mkdir -p $(PREFIX)/bin
	cp -p $(TARGET) $(PREFIX)/bin
