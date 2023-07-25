CXX = g++
# dir of sfml on my system
INCPATH = /usr/local/include/
# dir of sfml dylibs on my system
LIBPATH = /usr/local/lib/
LIBS = sfml-window sfml-system sfml-graphics
CXXFLAGS = -Wextra -Wall -Wno-missing-braces -mavx -march=native -std=c++11 -O3 -I$(INCPATH) -L$(LIBPATH) $(addprefix -l,$(LIBS))

cs3382-a2: main.cpp matrix.hpp vector.hpp geom.hpp Bresenham.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -r cs3382-a2 *.dSYM

.PHONY: clean
