build:
	@echo "building TUP"
	@clang++ -W -Wall -Wextra -Wformat -Wno-vla -pedantic -O3 -std=c++17 main.cpp -o TUP

clean:
	@rm TUP
