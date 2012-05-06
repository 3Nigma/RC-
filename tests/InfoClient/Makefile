PROJ := infoclient

CC := gcc
CXX := g++

3RDPDIR := ../../3rdp
BINDIR := ../../bin
LDFLAGS := -g -pthread -ldl -lm -lcurl -lboost_regex
CXXFLAGS := -std=c++0x -I$(3RDPDIR)
3RDPOBJS := $(3RDPDIR)/mongoose/mongoose.o \
$(3RDPDIR)/cJSON/cjson.o
OBJS := rcos_InfoServer.o

%.o : %.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< 

all : rcos $(OBJS)
	$(CXX) $(CXXFLAGS) $(3RDPOBJS) $(OBJS) main.cpp -o $(PROJ) $(LDFLAGS)

rcos : main.cpp
	$(BINDIR)/rcc -c ./main.cpp .

clean:
	rm -f $(PROJ) $(OBJS) *~ rcos_*
