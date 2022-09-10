.PHONY: all
.PHONY: clean
all: afktest

clean:
	rm -f *.o
	rm -f afktest

afktest: main.o network.o packet.o srp.o mini-gmp.o sha256.o
	$(CXX) -O3 $^ -o $@

main.o: main.cpp network.h packet.h srp.h
	$(CXX) -O3 -c $< -o $@

network.o: network.cpp network.h packet.h
	$(CXX) -O3 -c $< -o $@

packet.o: packet.cpp packet.h
	$(CXX) -O3 -c $< -o $@

srp.o: srp.cpp srp.h mini-gmp.h sha2.h
	$(CXX) -O3 -c $< -o $@

mini-gmp.o: mini-gmp.c mini-gmp.h
	$(CC) -O3 -c $< -o $@

sha256.o: sha256.c sha2.h
	$(CC) -O3 -c $< -o $@
