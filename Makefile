CXX := g++
#CXX := /home/dwarf/Documents/cpp_projects/compiler/bin/g++4.8
AR := ar
LD := ld
CFLAGS := -std=c++11 -g -Dunix -D_REENTRANT -DSDL_MAIN_HANDLED -DGLEW_STATIC -Wall -fpic
CINCLUDE := -Iinclude -Iinclude/freetype -Isrc/rendering -Isrc/networking -Isrc/general -Isrc/gui -Isrc
LDFLAGS := -lopenal -lvorbisfile -lvorbis -lpng -lGL -lpthread -lSDL2 -lGLEW `freetype-config --libs` #-lprofiler
CPP_FILES := $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
#OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(CPP_FILES))
H_FILES := $(wildcard src/*/*.h)


slib: folders bin/libwarp.so

bin/libwarp.so: $(OBJ_FILES)
	$(CXX) -shared -Wl,-soname,libwarp.so -o $@ $^ $(LDFLAGS)

bin/libwarp.a: $(OBJ_FILES)
	$(AR) -rcs $@ $^ 

obj/%.o: src/%.cpp $(H_FILES)
	$(CXX) $(CFLAGS) $(CINCLUDE) -c -o $@ $<

folders : | bin obj obj/rendering obj/networking obj/general obj/gui

bin:
	mkdir bin

obj:
	mkdir obj

obj/rendering:
	mkdir obj/rendering

obj/gui:
	mkdir obj/gui

obj/networking:
	mkdir obj/networking

obj/general:
	mkdir obj/general

info:
	@echo $(OBJ_FILES)
	@echo $(CPP_FILES)

clean:
	rm -r -f obj/*	
	rm -r -f bin/libwarp.so
	rm -r -f bin/libwarp.a
