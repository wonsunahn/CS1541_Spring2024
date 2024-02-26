#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>

size_t arraySize;
bool sorted;
std::vector<int> data;

void *create(void *unused) {
    // generate data
    data.resize(arraySize);

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand() % 256;

    // optionally sort the data
    if (sorted) {
        std::sort(data.begin(), data.end());
    }
}

void *run(void *unused) {
    long long sum = 0;
    for (unsigned i = 0; i < 10000; ++i)
    {
        for (unsigned c = 0; c < arraySize; ++c)
        {
            if (data[c] >= 128)
                sum += data[c];
        }
    }
    printf("sum = %lld\n", sum);
}

int main(int args, char **argv)
{
    if (args != 3) {
        printf("Usage: branch <array size> <sort|nosort>\n");
        return 0;
    }
    arraySize = atoi(argv[1]);
    sorted = strcmp(argv[2], "sort") == 0;

    pthread_t tCreate, tRun;
    pthread_create(&tCreate, NULL, create, NULL);
    pthread_join(tCreate, NULL);
    pthread_create(&tRun, NULL, run, NULL);
    pthread_join(tRun, NULL);

    return 0;
}

