ODIR = bin
   side_effect_heir := $(shell mkdir -p $(ODIR))

make:
	g++ -pthread -std=c++11 *.cpp -o bin/buddhabrotter

run:
	bin/buddhabrotter
