#include <iostream>
#include <vector>

struct Player {
    int id;
    int rank;
};

int main(int argc, char* argv[]) {
    FILE* fin = nullptr;
    fin       = fopen("in.txt", "rt");
    int num   = 0;

    fscanf(fin, "%d", &num);

    std::vector<Player> data(num);

    for (int i = 0; i < num; i++) {
        fscanf(fin, "%d %d", &data[i].id, &data[i].rank);
    }

    fclose(fin);

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].id, data[i].rank);
    }
    return 0;
}