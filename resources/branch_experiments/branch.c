#include <algorithm>
#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>

int main(int args, char **argv)
{
    if (args != 3) {
        printf("Usage: branch <array size> <sort|nosort>\n");
        return 0;
    }

    // generate data
    const size_t arraySize = atoi(argv[1]);
    bool sorted = strcmp(argv[2], "sort") == 0;
    std::vector<int> data(arraySize);

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand() % 256;

    // optionally sort the data
    if (sorted) {
        std::sort(data.begin(), data.end());
    }

    long long sum = 0;

    for (unsigned i = 0; i < 10000; ++i)
    {
        for (unsigned c = 0; c < arraySize; ++c)
        {
            if (data[c] >= 128)
                sum += data[c];
        }
    }
    printf("sum = %ld\n", sum);
}

