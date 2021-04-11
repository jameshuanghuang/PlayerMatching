#include <cstdlib>
#include <iostream>
int main(int argc, char* argv[]) {
    if (argc <= 1) {
        std::cout << "Usage:\n\t" << argv[0] << " <num>\n";
        return -1;
    }
    int num = atoi(argv[1]);
    std::cout << num << "\n";
    srand(100);
    for (int i = 0; i < num; i++) {
        // [todo]: Implement a Player class
        int userID   = i;
        int userRank = rand() % 200 + 900;
        std::cout << userID << " " << userRank << "\n";
    }
    return 0;
}