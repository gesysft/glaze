all: formula side4 slip percent 

formula: formula.cpp tools.h tools.cpp ptable.cpp 
	flex -f ptable.l
	g++ -g -Wall -std=c++11 formula.cpp tools.cpp ptable.cpp -o formula -Wno-deprecated-register -Wno-unused-function

side4: side4.cpp
	g++ -g -Wall -std=c++11 side4.cpp -o side4

slip: slip.cpp
	g++ -g -Wall -std=c++11 slip.cpp -o slip

percent: percent.cpp tools.cpp tools.h
	g++ -g -Wall -std=c++11 -Wall percent.cpp tools.cpp -o percent

clean:
	rm -rf a.out formula 4side slip percent *.o 
