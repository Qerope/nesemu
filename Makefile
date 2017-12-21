CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra
SRC = src/main.cpp
SRC += src/cartridge.cpp
SRC += src/cpu.cpp

nesemu: $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)
