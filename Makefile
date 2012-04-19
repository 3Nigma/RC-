CXX := g++
CXXFLAGS := -I./3rdp
CREPO := 

all: crep

3rdpdeps:
	cd 3rdp; make

crep: 3rdpdeps
	cd src/crep; make

clean:
	cd 3rdp; make clean
	cd src/crep; make clean
	cd bin; rm -f *
