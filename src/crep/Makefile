PROJ := crep

CC := gcc
CXX := g++

3RDPDIR := ../../3rdp
LDFLAGS := -pthread -ldl -lm
CXXFLAGS := -std=c++0x -I$(3RDPDIR)
OBJS := $(3RDPDIR)/mongoose/mongoose.o \
$(3RDPDIR)/cJSON/cjson.o
BINDIR := ../../bin

all:$(CREPO)
	$(CXX) $(CXXFLAGS) $(OBJS) main.cpp -o $(BINDIR)/$(PROJ) $(LDFLAGS)

clean:
	rm -f $(BINDIR)/$(PROJ)
