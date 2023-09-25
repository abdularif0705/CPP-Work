
CXXFLAGS=-std=c++20 -Wall -Wextra -Werror

all: a6.exe

clean:
	rm -f *.exe

run: a6.exe
	@echo "================================================================="
	./a6.exe || (echo "Non-zero shell exit status: $$?" ; true)
	@echo "================================================================="
	./a6.exe arg1 arg2 || (echo "Non-zero shell exit status: $$?" ; true)
	@echo "================================================================="
	./a6.exe arg1 arg2 arg3 || (echo "Non-zero shell exit status: $$?" ; true)
	@echo "================================================================="
	for f in input*.dat ; do \
		echo "$$ ./a6.exe $$f" ; \
		./a6.exe $$f || (echo "Non-zero shell exit status: $$?" ; true); \
	done
	@echo "================================================================="

%.exe: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
