
CXXFLAGS=-std=c++20 -Wall -Wextra -Werror -fconcepts-diagnostics-depth=10 -fsanitize=address -O3 -march=native

TARGETS=test_levenshtein.exe test_mutate.exe test_crossover.exe

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

run: $(TARGETS)
	@for prog in $(TARGETS) ; do \
		echo "$$ ./$$prog" ; \
		./$$prog ; \
	done

%.exe: %.cpp *.hpp
	$(CXX) $(CXXFLAGS) -o $@ $<
