CXX = g++
# dir of sfml on my system
INCPATH = /usr/local/include/
# dir of sfml dylibs on my system
LIBPATH = /usr/local/lib/
LIBS = sfml-window sfml-system sfml-graphics
CXXFLAGS = -Wall -Os -std=c++11 -I$(INCPATH) -L$(LIBPATH) $(addprefix -l,$(LIBS))

cs3382-a1: main.cpp
	$(CXX) $(CXXFLAGS) $? -o $@

clean:
	rm cs3382-a1

.PHONY: clean
