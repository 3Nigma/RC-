PROJ := crep

CC := gcc
CXX := g++

3RDPDIR := ../../3rdp
BINDIR := ../../bin
LDFLAGS := -pthread -ldl -lm
CXXFLAGS := -std=c++0x -I$(3RDPDIR)
3RDPOBJS := $(3RDPDIR)/mongoose/mongoose.o \
$(3RDPDIR)/cJSON/cjson.o
OBJS := requestinfo.o \
replyer.o \
remoteobjectserver.o \
remrep.o

%.o : %.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< 

all: $(OBJS)
	$(CXX) $(CXXFLAGS) $(3RDPOBJS) $(OBJS) main.cpp -o $(BINDIR)/$(PROJ) $(LDFLAGS)

clean:
	rm -f $(BINDIR)/$(PROJ) $(OBJS)
