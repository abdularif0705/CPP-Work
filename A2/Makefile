
CXXFLAGS=-std=c++20 -Wall -Wextra -Werror -O3

all: a2.exe

clean:
	rm -f a2.exe

run: a2.exe
	@echo -n '$$ '
	./$< input0.dat
	@echo -e -n '$$\n$$ '
	./$< input1.dat
	@echo -e -n '$$\n$$ '
	./$< input2.dat
	@echo -e -n '$$\n$$ '
	./$< input3.dat
	@echo -e -n '$$\n$$ '
	./$< input4.dat
	@echo -e -n '$$\n$$ '
	./$< input5.dat
	@echo -e -n '$$\n$$ '
	./$< input6.dat
	@echo -e -n '$$\n$$ '
	./$< input7.dat
	@echo '$$'

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

