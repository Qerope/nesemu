CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra
SRC = src/main.cpp
SRC += src/cartridge.cpp
SRC += src/cpu.cpp
TEST_SRC = tests/test_cpu.cpp src/cpu.cpp src/cartridge.cpp

nesemu: $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

test: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -Isrc -o test_cpu $(TEST_SRC) && ./test_cpu
