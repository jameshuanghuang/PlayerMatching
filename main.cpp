#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "Player.hpp"
#include "Team.hpp"

// [todo] need to test different num threads,
#define NUM_THREADS 2
#define NUM_PLAYERS_PER_TEAM 3
#define ROOM_SIZE (NUM_PLAYERS_PER_TEAM * 2)
std::mutex              mut_;
std::condition_variable cv;
bool                    isEnd = false;

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
        data[i].setProperties(id, rank, 0);
    }
}

struct ProducerThreadData {
    int                  roomSize;
    std::vector<Player>* pWaitingRoom;
    std::vector<Player>* pData;
};

void ProducerFunc(ProducerThreadData* producerThreadData) {
    printf("[Producer] %s() start\n", __FUNCTION__);
    const int                  roomSize     = producerThreadData->roomSize;
    std::vector<Player>*       pWaitingRoom = producerThreadData->pWaitingRoom;
    const std::vector<Player>* pData        = producerThreadData->pData;

    assert(pWaitingRoom);
    assert(pData);

    for (int i = 0; i < pData->size(); i++) {
        std::unique_lock<std::mutex> lk(mut_);
        cv.wait(lk, [&]() { return pWaitingRoom->size() < roomSize; });
        for (auto& player : *pWaitingRoom) {
            player.waitOnetick();
        }
        pWaitingRoom->push_back((*pData)[i]);
        printf("[Producer] Player %d joined the waiting room.\n",
               (*pData)[i].getId());
        printf("[Producer] Notify One\n");
        cv.notify_one();
    }
    isEnd = true;
    printf("[Producer] %s() start\n", __FUNCTION__);
}

struct ConsumerThreadData {
    std::vector<Player>* pWaitingRoom;
};

void ConsumerFunc(ConsumerThreadData* consumerThreadData) {
    printf("[Consumer] %s() start\n", __FUNCTION__);
    std::vector<Player>* pWaitingRoom = consumerThreadData->pWaitingRoom;

    assert(pWaitingRoom);

    while (!isEnd) {
        {
            std::unique_lock<std::mutex> lk(mut_);
            cv.wait(lk, [&]() {
                return pWaitingRoom->size() >= NUM_PLAYERS_PER_TEAM * 2;
            });
            assert(pWaitingRoom->size() >= NUM_PLAYERS_PER_TEAM * 2);
            if (pWaitingRoom->size() >= NUM_PLAYERS_PER_TEAM * 2) {
                // [todo] before sort, add previous remaining player to team
                sort(pWaitingRoom->begin(), pWaitingRoom->end(),
                     [](const Player& p1, const Player& p2) {
                         return p1.getRank() > p2.getRank();
                     });

                Team teams[2] = {};
                for (int i = 0; i < NUM_PLAYERS_PER_TEAM * 2; i++) {
                    const auto& player = (*pWaitingRoom)[i];
                    const auto  which  = (i % 4 == 1 || i % 4 == 2) ? 1 : 0;

                    // [todo] Remove redundent copy
                    teams[which].addPlayer(player);
                }

                const int match_differnece =
                    Team::calMatchDifference(teams[0], teams[1]);

                printf("[Consumer] Match diff = %d =====\n", match_differnece);
                for (const auto& team : teams) {
                    printf("[Consumer] Team rank = %d =====\n", team.getRank());
                    for (const auto& player : team.getPlayers()) {
                        printf("[Consumer] %d %d %d\n", player.getId(),
                               player.getRank(), player.getWaitTime());
                    }
                }
                printf("\n");

                pWaitingRoom->erase(pWaitingRoom->begin(),
                                    pWaitingRoom->begin() +
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
         [](Player& p1, Player& p2) { return p1.getId() < p2.getId(); });

    printf("%d\n", num);
    for (int i = 0; i < num; i++) {
        printf("%d %d\n", data[i].getId(), data[i].getRank());
    }

    printf("End of read data =======\n");

    const int           roomSize = ROOM_SIZE;
    std::vector<Player> waitingRoom;
    waitingRoom.reserve(roomSize);

    ProducerThreadData ptd{
        .roomSize = roomSize, .pWaitingRoom = &waitingRoom, .pData = &data};
    ConsumerThreadData ctd{.pWaitingRoom = &waitingRoom};

    isEnd = false;
    std::thread th_producer(ProducerFunc, &ptd);
    std::thread th_consumer(ConsumerFunc, &ctd);

    th_producer.join();
    th_consumer.join();

    return 0;
}
