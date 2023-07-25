CXX = g++
# dir of sfml on my system
INCPATH = /usr/local/include/
# dir of sfml dylibs on my system
LIBPATH = /usr/local/lib/
LIBS = sfml-window sfml-system sfml-graphics
CXXFLAGS = -Wextra -Wall -Wno-missing-braces -mavx -march=native -std=c++11 -O3 -I$(INCPATH)
LINKER = -L$(LIBPATH) $(addprefix -l,$(LIBS))

cs3388-a3: main.o triangle.o vector.o geom.o
	$(CXX) $(CXXFLAGS) $(LINKER) $^ -o $@

triangle.o: triangle.cpp triangle.hpp
vector.o: vector.cpp vector.hpp
geom.o: geom.cpp geom.hpp
main.o: main.cpp matrix.hpp vector.hpp geom.hpp Bresenham.hpp mesh.hpp light.hpp

clean:
	rm -r cs3388-a3 *.dSYM *.o

.PHONY: clean
