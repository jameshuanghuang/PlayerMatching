#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// [todo] need to test different num threads,
#define NUM_THREADS 2
#define NUM_PLAYERS_PER_TEAM 3
#define ROOM_SIZE (NUM_PLAYERS_PER_TEAM * 2)
std::mutex              mut_;
std::condition_variable cv;
bool                    isEnd = false;

struct Player {
    int id;
    int rank;
    int wait_time;
};

struct Team {
    std::vector<Player> member;
    int                 rank;
};

struct ReadFileThreadData {
    FILE*                fin;
    std::vector<Player>& data;
    int                  startIdx;
    int                  endIdx;
};

void ReadFileFunc(ReadFileThreadData* readFileThreadata) {
    FILE*                fin  = readFileThreadata->fin;
    std::vector<Player>& data = readFileThreadata->data;
    int                  num  = data.size();
    for (int i = readFileThreadata->startIdx; i <= readFileThreadata->endIdx;
         i++) {
        int id   = -1;
        int rank = -1;

        mut_.lock();
        fscanf(fin, "%d %d", &id, &rank);
        mut_.unlock();

        data[i].id        = id;
        data[i].rank      = rank;
        data[i].wait_time = 0;
    }
}

struct ProducerThreadData {
    int                  room_size;
    std::vector<Player>& waiting_room;
    std::vector<Player>& data;
};

void ProducerFunc(ProducerThreadData* producerThreadData) {
    printf("[Producer] %s() start\n", __FUNCTION__);
    const auto  room_size    = producerThreadData->room_size;
    auto&       waiting_room = producerThreadData->waiting_room;
    const auto& data         = producerThreadData->data;
    for (int i = 0; i < data.size(); i++) {
        std::unique_lock<std::mutex> lk(mut_);
        cv.wait(lk, [&]() { return waiting_room.size() < room_size; });
        for (auto& player : waiting_room) {
            player.wait_time++;
        }
        waiting_room.push_back(data[i]);
        printf("[Producer] Player %d joined the waiting room.\n", data[i].id);
        printf("[Producer] Notify One\n");
        cv.notify_one();
    }
    isEnd = true;
    printf("[Producer] %s() start\n", __FUNCTION__);
}

struct ConsumerThreadData {
    std::vector<Player>& waiting_room;
};

void ConsumerFunc(ConsumerThreadData* consumerThreadData) {
    printf("[Consumer] %s() start\n", __FUNCTION__);
    auto& waiting_room = consumerThreadData->waiting_room;

    while (!isEnd) {
        {
            std::unique_lock<std::mutex> lk(mut_);
            cv.wait(lk, [&]() {
                return waiting_room.size() >= NUM_PLAYERS_PER_TEAM * 2;
            });
            assert(waiting_room.size() >= NUM_PLAYERS_PER_TEAM * 2);
            if (waiting_room.size() >= NUM_PLAYERS_PER_TEAM * 2) {
                sort(waiting_room.begin(), waiting_room.end(),
                     [](Player p1, Player p2) { return p1.rank > p2.rank; });

                Team teams[2] = {};
                for (int i = 0; i < NUM_PLAYERS_PER_TEAM * 2; i++) {
                    const auto& player = waiting_room[i];
                    const auto  which  = (i % 4 == 1 || i % 4 == 2) ? 1 : 0;

                    // [todo] Remove redundent copy
                    teams[which].member.push_back(player);
                    teams[which].rank += player.rank;
                }

                // [todo] Wrap into a function
                const int match_differnece = abs(teams[0].rank - teams[1].rank);

                printf("[Consumer] Match %d =====\n", match_differnece);
                for (const auto& team : teams) {
                    printf("[Consumer] Team %d =====\n", team.rank);
                    for (const auto& player : team.member) {
                        printf("[Consumer] %d %d %d\n", player.id, player.rank,
                               player.wait_time);
                    }
                }
                printf("\n");

                waiting_room.erase(waiting_room.begin(),
                                   waiting_room.begin() +
                                       NUM_PLAYERS_PER_TEAM * 2);
            }
        }
        printf("[Consumer] Notify One\n");
        cv.notify_one();
    }
    printf("[Consumer] %s() end\n", __FUNCTION__);
}

int main(int argc, char* argv[]) {
    FILE* fin = nullptr;
    fin       = fopen("in.txt", "rt");
    int num   = 0;

    fscanf(fin, "%d", &num);

    std::vector<Player> data(num);

    std::vector<std::thread>        threads;
    std::vector<ReadFileThreadData> readFileThreadDataArray;
    /*
     0 32   33*0   33*1  0   33*i  33*(i+1)
    33 65   33*1   33*2  1
    66 98   33*2   33*3  2
    */
    for (int i = 0; i < NUM_THREADS; i++) {
        int count    = num / NUM_THREADS;
        int startIdx = count * i;
        int endIdx   = count * (i + 1) - 1;
        // Assign the rest tasks to the last thread
        if (i == NUM_THREADS - 1) {
            endIdx = num - 1;
        }
        readFileThreadDataArray.push_back(ReadFileThreadData{
            .fin = fin, .data = data, .startIdx = startIdx, .endIdx = endIdx});
        printf("thread %d : [%d, %d]\n", i, startIdx, endIdx);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        threads.push_back(
            std::thread(ReadFileFunc, &readFileThreadDataArray[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    fclose(fin);

    sort(data.begin(), data.end(),
         [](Player& p1, Player& p2) { return p1.id < p2.id; });

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].id, data[i].rank);
    }

    printf("End of read data =======\n");

    const int           room_size = ROOM_SIZE;
    std::vector<Player> waiting_room;
    waiting_room.reserve(room_size);

    ProducerThreadData ptd{
        .room_size = room_size, .waiting_room = waiting_room, .data = data};
    ConsumerThreadData ctd{.waiting_room = waiting_room};

    isEnd = false;
    std::thread th_producer(ProducerFunc, &ptd);
    std::thread th_consumer(ConsumerFunc, &ctd);

    th_producer.join();
    th_consumer.join();

    return 0;
}
