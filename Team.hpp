#ifndef __TEAM_HPP__
#define __TEAM_HPP__
#include <vector>

#include "Player.hpp"

class Team final {
  public:
    Team() : m_players(std::vector<Player>()), m_rank(0) {}
    Team(std::vector<Player> players, int rank)
        : m_players(players), m_rank(rank) {}

    // [todo] Check potential shallow copy of vector
    inline int getRank() const { return m_rank; }
    inline int getTeamError() const { return m_teamError; }

    // [todo] Avoid copy
    void addPlayer(Player player);
    void updateTeamError();
    void showTeamPlayer();

  private:
    std::vector<Player> m_players;
    int                 m_rank;
    int                 m_avgRank;
    int                 m_teamError;
};

#endif // #ifndef __TEAM_HPP__