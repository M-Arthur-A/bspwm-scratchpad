CXX = clang
NAME = spawner
EXE_PATH = ./bin
EXE = -o $(EXE_PATH)/$(NAME)
SOURCES = ./$(NAME).cpp
# INCLUDES = -I/usr/include/python3.10/ -I/usr/include/pybind11/
CXXFLAGS_RELEASE  += -w -O3 -Wall
CXXFLAGS_DEBUG += -g -w -O3 -Wall
LIBS = -lxcb -lstdc++

$(NAME)_release: ./$(NAME).cpp
	$(CXX) $(SOURCES) $(EXE) $(CXXFLAGS_RELEASE) $(LIBS)

# end
