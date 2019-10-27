compile:
	g++ main.cpp -std=c++14 -o main.out

.PHONY:run
run: compile
	./main.out