# Random Prime Counter

@ Haim Goldfisher & Asaf Yekutiel

## Description

This program is designed for real-time processing of an endless data stream. It generates a stream of random numbers and efficiently counts the prime numbers in the stream using a custom prime counting algorithm. This is particularly useful for scenarios like security camera data processing where quick intrusion detection is crucial.

## Compilation

To compile the program, use the following command:

```bash
make
```

This assumes you have the make utility installed on your system.

## Naive Solution

To run the program with the naive prime counter implementation, use:

```bash
./randomGenerator 7 1000 | ./myPrimeCounter
```

This command uses a naive implementation for prime counting. Be aware that the naive implementation may not be as efficient for large data streams.
This command generates 1000 random numbers with random seed 7 and feeds them into the prime counter. Adjust the parameters as needed.

## Prime Counting Strategy

The `myPrimeCounter` utilizes a parallelized approach to efficiently count prime numbers within an endless data stream. The strategy involves breaking down the data stream into chunks, each assigned to a separate thread for simultaneous processing. This parallelization aims to leverage the capabilities of modern multi-core processors for faster prime detection.

### Miller-Rabin Primality Test

The core of the algorithm relies on the Miller-Rabin primality test, which is a probabilistic algorithm used to determine the primality of numbers. It repeatedly tests randomly chosen bases against the number to be checked for primality. The number of iterations (`iter`) determines the confidence level of the primality result.

### Parallel Processing

The data stream is divided into chunks, and each thread independently processes its assigned chunk using the Miller-Rabin primality test. To minimize contention, atomic operations are used for synchronizing access to shared variables, such as the start and end indices of each thread's chunk.

### Modular Arithmetic Functions

The algorithm employs modular arithmetic functions, including modular multiplication (`multiply_mod`) and modular exponentiation (`modular_exponentiation`). These functions contribute to the efficiency of the Miller-Rabin test and ensure that computations remain within manageable bounds, crucial for handling large numbers in real-time scenarios.

### Dynamic Thread Allocation

The program dynamically allocates threads based on the predefined `NUM_THREADS` constant. Each thread maintains its local counter for prime numbers, and the main thread aggregates the results from all threads to obtain the total count of prime numbers in the data stream.

### Real-Time Considerations

The design of `myPrimeCounter` prioritizes real-time considerations, making it suitable for scenarios like security camera data processing. The use of parallelization and efficient algorithms aims to minimize latency and provide timely results for prime number detection in the continuous data stream.

## Running the Program

To run the program, use the following command. For example:

```bash
./randomGenerator 10 10000000 | ./myPrimeCounter
```

This command generates 10M random numbers with random seed 10 and feeds them into the prime counter. Adjust the parameters as needed.

## Running with Time Measurement

To measure the execution time, you can use the time command. For example:

```bash
time ./randomGenerator 10 10000000 | ./myPrimeCounter

```

This will provide information about the real, user, and system time taken by the program.

## Performance Comparison

To showcase the superior performance of `myPrimeCounter` over the naive implementation (`primeCounter`), a performance comparison was conducted using a dataset of 10 million random numbers. The results reveal that `myPrimeCounter` runs approximately 10 times faster than `primeCounter` for this specific scenario.

### Execution Time Comparison

The following commands were used for the comparison:

```bash
time ./randomGenerator 10 10000000 | ./primeCounter
```

```bash
time ./randomGenerator 10 10000000 | ./myPrimeCounter
```

The runtime measurements demonstrate a substantial improvement in the execution time of 'myPrimeCounter' compared to 'primeCounter'.

### Visual Confirmation

To visually substantiate the performance difference, refer to the attached image below:

![Memory Usage](https://github.com/haimgoldfisher/Adv_Prog/blob/main/Ex2/X10_than_primeCounter.jpg?raw=true)

## Memory Usage Verification

To ensure efficiency in memory consumption, myPrimeCounter was tested for an infinite stream of numbers. The program consistently maintains a memory footprint below 2MB, making it well-suited for scenarios with continuous data streams.

### Memory Measurement

The following command was used to assess memory usage:

```bash
valgrind --tool=massif ./randomGenerator 10 10000000 | ./myPrimeCounter
```

### Visual Confirmation

The System Monitor screenshot below provides visual proof of `myPrimeCounter` maintaining memory usage below the 2MB threshold:

![Memory Usage](https://github.com/haimgoldfisher/Adv_Prog/blob/main/Ex2/no_more_than_2MB_RAM.png?raw=true)

## Dependencies

This program relies on the 'make' utility for compilation. It also relies on the 'valgrind' utility for memory consumption check.
