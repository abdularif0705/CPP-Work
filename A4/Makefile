CXX=g++-11
CXXFLAGS=-std=c++20 -Wall -Wextra -Wold-style-cast -pedantic -Werror
LDFLAGS=

all: a4.exe

clean:
	rm -f *.exe *.o

run: a4.exe
	./$< <input1.dat
	./$< <input2.dat
	./$< <input3.dat
	./$< <input4.dat

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

a4.exe: a4.o a4-provided.o
	$(CXX) $(CXXFLAGS) -o $@ $+ $(LDFLAGS)
