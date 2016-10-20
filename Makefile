CXX := g++
#CXX := /home/dwarf/Documents/cpp_projects/compiler/bin/g++4.8
LD := ld
CFLAGS := -std=c++11 -g -Dunix -D_REENTRANT -DSDL_MAIN_HANDLED -DGLEW_STATIC -Wall
CINCLUDE := -Iinclude -Iinclude/warp -Isrc
LDFLAGS := -Wl,-rpath=./ -Llib -lwarp -lopenal -lvorbisfile -lvorbis -lpng -lGL -lpthread -lSDL2 -lGLEW `freetype-config --libs` #-lprofiler
CPP_FILES := $(wildcard src/*.cpp) $(wildcard src/util/*.cpp)
#OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(CPP_FILES))
TEST_FILES := $(patsubst src/test/%.cpp,obj/test/%.o, $(wildcard src/test/*.cpp))
H_FILES := $(wildcard src/*.h) $(wildcard src/util/*.h)

compile: debug/ant

test: debug/test
	cd debug; ./test

debug/ant: $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

debug/test: $(filter-out obj/main.o,$(OBJ_FILES)) $(TEST_FILES) 
	$(CXX) -o $@ $^ $(LDFLAGS) -lgtest -lgtest_main -lpthread


obj/%.o: src/%.cpp $(H_FILES)
	$(CXX) $(CFLAGS) $(CINCLUDE) -c -o $@ $<

info:
	@echo $(OBJ_FILES)
	@echo $(CPP_FILES)

clean:
	rm -r -f obj/*
	rm -f debug/ant
	mkdir obj/test


