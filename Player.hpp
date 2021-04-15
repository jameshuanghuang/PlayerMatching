#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__
#include <vector>

class Player final {
  public:
    Player() : m_id(0), m_rank(1), m_waitTime(0) {}
    Player(int id, int rank, int wait_time)
        : m_id(id), m_rank(rank), m_waitTime(wait_time) {}

    inline void waitOnetick() { m_waitTime++; }
    inline void print() const {
        printf("Player id: %d, rank %d, waited %d\n", getId(), getRank(),
               getWaitTime());
    }
    inline int getRank() const { return m_rank; }
    inline int getId() const { return m_id; }
    inline int getWaitTime() const { return m_waitTime; }

  private:
    int m_id;
    int m_rank;
    int m_waitTime;
};

#endif // #ifndef __PLAYER_HPP__