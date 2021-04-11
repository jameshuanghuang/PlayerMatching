#include <cstdio>
#include <iostream>
#include <vector>

struct Player {
    int id;
    int rank;
};

void th_readfile(FILE* fin, int num, std::vector<Player>& data) {
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

    th_readfile(fin, num, data);

    fclose(fin);

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].id, data[i].rank);
    }

    return 0;
}