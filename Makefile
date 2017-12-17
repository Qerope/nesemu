CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra
SRC = src/main.cpp

nesemu: $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)
