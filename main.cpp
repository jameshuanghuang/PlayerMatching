#include <cstdio>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// [todo] need to test different num threads, 
#define NUM_THREADS 2 

std::mutex mut_;

struct Player {
    int id;
    int rank;
};

struct ThreadData {
    FILE*                fin;
    std::vector<Player>& data;
    int                  startIdx;
    int                  endIdx;
};

void th_readfile(ThreadData threadata) {
    FILE*                fin  = threadata.fin;
    std::vector<Player>& data = threadata.data;
    int                  num  = data.size();
    for (int i = threadata.startIdx; i <= threadata.endIdx; i++) {
        int id   = -1;
        int rank = -1;

        mut_.lock();
        fscanf(fin, "%d %d", &id, &rank);
        mut_.unlock();

        data[i].id   = id;
        data[i].rank = rank;
    }
}

int main(int argc, char* argv[]) {
    FILE* fin = nullptr;
    fin       = fopen("in.txt", "rt");
    int num   = 0;

    fscanf(fin, "%d", &num);

    std::vector<Player> data(num);

    std::vector<std::thread> threads;
    /*
     0 32   33*0   33*1  0   33*i  33*(i+1)
    33 65   33*1   33*2  1
    66 98   33*2   33*3  2
    */
    for (int i = 0; i < NUM_THREADS; i++) {
        int count      = num / NUM_THREADS;
        int startIndex = count * i;
        int endIndex   = count * (i + 1) - 1;
        // Assign the rest tasks to the last thread
        if (i == NUM_THREADS - 1) {
            endIndex = num - 1;
        }
        ThreadData threadata = {fin, data, startIndex, endIndex};
        threads.push_back(std::thread(th_readfile, threadata));
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].join();
    }

    fclose(fin);

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].id, data[i].rank);
    }

    return 0;
}
