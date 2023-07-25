CXX = g++
# dir of sfml on my system
INCPATH = /usr/local/include/
# dir of sfml dylibs on my system
LIBPATH = /usr/local/lib/
LIBS = sfml-window sfml-system sfml-graphics
CXXFLAGS = -Wextra -Wall -Wno-missing-braces -mavx -march=native -std=c++17 -O3 -I$(INCPATH) -ferror-limit=3
LINKER = -L$(LIBPATH) $(addprefix -l,$(LIBS))

cs3388-a4: main.o vector.o plane.o cone.o sphere.o
	$(CXX) $(CXXFLAGS) $(LINKER) $^ -o $@

sphere.o: surface.hpp sphere.hpp sphere.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
plane.o: surface.hpp plane.hpp plane.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
cone.o: surface.hpp cone.hpp cone.cpp matrix_utils.hpp material.hpp matrix.hpp vector.hpp
vector.o: vector.cpp vector.hpp
main.o: main.cpp matrix.hpp vector.hpp light.hpp matrix_utils.hpp surface.hpp plane.hpp cone.hpp material.hpp sphere.hpp

clean:
	rm -r cs3388-a4 *.dSYM *.o

.PHONY: clean
