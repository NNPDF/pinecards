CXX = c++

all:
	$(CXX) -c applcheck.cpp $(shell pkg-config lhapdf --cflags) $(shell applgrid-config --cxxflags)
	$(CXX) applcheck.o $(shell pkg-config lhapdf --cflags --libs) $(shell applgrid-config --cxxflags --ldflags) -o applcheck
