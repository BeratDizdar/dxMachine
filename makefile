LIB = -ld3d9 -lole32 -lxaudio2_8
CXX = g++
CXXFLAGS = -O2 -fno-rtti -fno-exceptions -fno-threadsafe-statics
SRC_FILES = $(wildcard src/*.cpp) $(wildcard src/depen/*.cpp)
OBJ_FILES = $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES))

test: compile
	$(CXX) *.cpp obj/*.o obj/depen/*.o -o main.exe $(LIB)
	./main.exe

compile: dirs lib/dxMachine.dll

lib/dxMachine.dll: $(OBJ_FILES)
	g++ -shared -o $@ $^ $(LIB)

obj/%.o: src/%.cpp
	g++ $(CXXFLAGS) -c $< -o $@

dirs:
	mkdir -p obj/depen
	mkdir -p lib

clean:
	rm -rf obj
	rm -f lib/dxMachine.dll

# fxc .\main.hlsl /nologo /T vs_2_0 /Fh main_vs.h /Vn g_MainVS
# fxc .\main.hlsl /nologo /T vs_2_0 /Fo main.pso