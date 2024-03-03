#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_THREADS 5
#define CHUNK_SIZE 2000000

struct ThreadArgs {
    atomic_int start; // starting index for the thread
    atomic_int end;  // ending index for the thread
    int* local_counter; // local counter for prime numbers found by the thread
};

long long multiply_mod(long long a, long long b, long long mod) // modular multiplication
{
    long long result = 0;
    long long y = a % mod;
    while (b > 0) 
    {
        if (b % 2 == 1)
            result = (result + y) % mod;
        y = (y * 2) % mod;
        b /= 2;
    }
    return result % mod;
}

long long modular_exponentiation(long long base, long long exponent, long long mod) // modular exponentiation
{
    long long result = 1;
    long long y = base;
    while (exponent > 0) 
    {
        if (exponent % 2 == 1)
            result = (result * y) % mod;
        y = (y * y) % mod;
        exponent /= 2;
    }
    return result % mod;
}

/*
Miller-Rabin Primality Test:

Input: p (number to be tested for primality), iter (number of iterations)

1. If p < 2 or (p != 2 and p % 2 == 0), return 0 (not prime).
2. Factorize p-1 as 2^s * d, where s is the largest power of 2 dividing p-1 and d is odd.
3. Repeat iter times:
   a. Choose a random base 'a' in the range [1, p - 1].
   b. Compute mod = a^d % p.
   c. While d is not p-1 and mod is not 1 and mod is not p-1:
        i. Square mod and double d.
   d. If mod is not p-1 and d is even, return 0 (not prime).

4. Return 1 (likely prime) after iter iterations with a probability of at least (1 - 0.5^iter) for correctness.

Running Time: O(iter*log(p)) = O(log(p))

*/

int miller_rabin_test(long long p, int iter) 
{
    if (p < 2 || (p != 2 && p % 2 == 0)) // check if p is less than 2 or even (excluding 2)
        return 0;
    long long s = p - 1;
    while (s % 2 == 0) 
    {
        s /= 2; // divide s by 2 until it becomes an odd number
    }
    for (int i = 0; i < iter; i++)
    {
        // choose a random base 'a' in the range [1, p - 1]
        long long a = rand() % (p - 1) + 1;

        long long temp = s;
        long long mod = modular_exponentiation(a, temp, p);

        // perform modular exponentiation to compute a^(s) % p
        while (temp != p - 1 && mod != 1 && mod != p - 1) 
        {
            // square the result and update the exponent
            mod = multiply_mod(mod, mod, p);
            temp *= 2;
        }
        // check cond for primality
        if (mod != p - 1 && temp % 2 == 0)
            return 0;
    }
    return 1; // p is likely prime
}

_Noreturn void* process_range(void* args) {
    struct ThreadArgs* threadArgs = (struct ThreadArgs*)args;
    int* local_counter = (threadArgs->local_counter);

    int num;
    int start, end;

    while (1) 
    {
        start = atomic_load(&(threadArgs->start));
        end = atomic_load(&(threadArgs->end));
        for (int i = start; i <= end; i++) {
            if (scanf("%d", &num) != EOF) 
            {
                if (miller_rabin_test(num, 4)) // (1 − 0.0625) = 0.9375% 
                    (*local_counter)++;
            } 
            else 
            {
                pthread_exit(NULL); // end of input or error condition
            }
        }

        // printf("Thread %ld processed range [%d, %d], local_counter = %d\n", pthread_self(), start, end, *local_counter);

        // update start and end values for the next iteration
        atomic_store(&(threadArgs->start), start + NUM_THREADS * CHUNK_SIZE);
        atomic_store(&(threadArgs->end), end + NUM_THREADS * CHUNK_SIZE);
    }

    pthread_exit(NULL);
}

int main() 
{
    int total_primes = 0;
    pthread_t threads[NUM_THREADS];
    struct ThreadArgs threadArgs[NUM_THREADS];

    int total_numbers;

    for (int i = 0; i < NUM_THREADS; i++) 
    {
        // init start and end values for each thread
        atomic_init(&(threadArgs[i].start), i * CHUNK_SIZE + 1);
        atomic_init(&(threadArgs[i].end), (i + 1) * CHUNK_SIZE);

        // allocate memory for local counter
        threadArgs[i].local_counter = (int*)malloc(sizeof(int));
        *(threadArgs[i].local_counter) = 0;

        // create threads and pass args
        pthread_create(&threads[i], NULL, process_range, &threadArgs[i]);
    }

    // wait for threads to finish and accumulate results
    for (int i = 0; i < NUM_THREADS; i++) 
    {
        pthread_join(threads[i], NULL);
        total_primes += *(threadArgs[i].local_counter);

        // free allocated memory for local counter
        free(threadArgs[i].local_counter);
    }

    // print the result
    printf("%d total primes.\n", total_primes);

    return 0;
}