PROJ := rcc

CC := gcc
CXX := g++

3RDPDIR := ../../3rdp
BINDIR := ../../bin
LDFLAGS := -g -pthread -ldl -lm -lcurl -lboost_regex
CXXFLAGS := -std=c++0x -I$(3RDPDIR)
3RDPOBJS := $(3RDPDIR)/mongoose/mongoose.o \
$(3RDPDIR)/cJSON/cjson.o
OBJS := methodinterface.o \
rcosmethod.o \
rcocmethod.o \
rcosinterface.o \
rcoscreator.o \
rcosrefurbisher.o

%.o : %.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $<

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(3RDPOBJS) $(OBJS) main.cpp -o $(BINDIR)/$(PROJ) $(LDFLAGS)

clean:
	rm -f $(BINDIR)/$(PROJ) $(OBJS)
