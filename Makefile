all: formula  

formula: formula.cpp tools.h tools.cpp ptable.cpp ptable.l
	flex -f ptable.l
	g++ -Wall -std=c++11 formula.cpp tools.cpp ptable.cpp -o formula -Wno-deprecated-register -Wno-unused-function

clean:
	rm -rf formula 
