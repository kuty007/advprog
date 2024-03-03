.PHONY: all
all: generator primeCounter myPrimeCounter 

generator:  generator.c
	gcc -o randomGenerator generator.c

primeCounter:	primeCounter.c
	gcc -o primeCounter primeCounter.c

myPrimeCounter: myPrimeCounter.c
	gcc -o myPrimeCounter myPrimeCounter.c -lpthread -lm


.PHONY: clean
clean:
	-rm randomGenerator primeCounter myPrimeCounter 2>/dev/null
