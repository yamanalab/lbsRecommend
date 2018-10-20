CC = g++

HELIB = /usr/local/src/HElib/src

CFLAGS = -O2 -std=c++11 -Wall #-Wshadow
GMP = -lgmp
LDLIBS = -L/usr/local/lib -L/usr/local/include -lntl $(GMP) -lm -lrt -lstdc++
CFLAGS += -I/usr/local/include -I$(HELIB) -pthread -DFHE_THREADS -DFHE_DCRT_THREADS -DFHE_BOOT_THREADS


%: %.cpp
	$(CC) $(CFLAGS) -o $@.out $< $(HELIB)/fhe.a $(LDLIBS)

clean:
	rm *.out


