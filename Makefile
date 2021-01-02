all: formula side4 percent 

formula: formula.cpp tools.h tools.cpp ptable.cpp 
	flex -f ptable.l
	g++ -Wall -std=c++11 formula.cpp tools.cpp ptable.cpp -o formula -Wno-deprecated-register -Wno-unused-function

side4: side4.cpp
	g++ -Wall -std=c++11 side4.cpp -o side4

percent: percent.cpp tools.cpp tools.h
	g++ -Wall -std=c++11 -Wall percent.cpp tools.cpp -o percent

clean:
	rm -rf formula side4 percent 
