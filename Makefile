build:
	@echo "building TUP"
	@clang++ -W -Wall -Wextra -Wformat -Wno-vla -pedantic -O3 -std=c++17 main.cpp -o TUP

test: build
	./TUP instances/umps4.txt 2 3 1000

clean:
	@rm TUP
