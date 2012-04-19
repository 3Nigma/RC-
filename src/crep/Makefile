CC := gcc
CXX := g++
3RDPDIR := ../../3rdp
LDFLAGS := -pthread -ldl -lm
CXXFLAGS := -I$(3RDPDIR)
OBJS := $(3RDPDIR)/mongoose/mongoose.o \
$(3RDPDIR)/cJSON/cjson.o
BINDIR := ../../bin

all:$(CREPO)
	$(CXX) $(CXXFLAGS) $(OBJS) main.cpp -o $(BINDIR)/crep $(LDFLAGS)

clean:
	rm -f $(OBJS)
