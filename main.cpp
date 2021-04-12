#include <cstdio>
#include <iostream>
#include <thread>
#include <vector>

struct Player {
    int id;
    int rank;
};

struct ThreadData {
    FILE*                fin;
    std::vector<Player>& data;
};

void th_readfile(ThreadData threadata) {
    FILE*               fin  = threadata.fin;
    std::vector<Player> data = threadata.data;
    int                 num  = data.size();
    for (int i = 0; i < num; i++) {
        fscanf(fin, "%d %d", &data[i].id, &data[i].rank);
    }
}

int main(int argc, char* argv[]) {
    FILE* fin = nullptr;
    fin       = fopen("in.txt", "rt");
    int num   = 0;

    fscanf(fin, "%d", &num);

    std::vector<Player> data(num);

    ThreadData threadata = {fin, data};

    std::thread first(th_readfile, threadata);
    first.join();

    fclose(fin);

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].id, data[i].rank);
    }

    return 0;
}
