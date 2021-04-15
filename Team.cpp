#include <vector>

#include "Team.hpp"
#include "Player.hpp"

// [todo] Avoid copy
void Team::addPlayer(Player player) {
    m_players.push_back(player);
    m_rank += player.getRank();
    m_avgRank = m_rank / m_players.size();
    updateTeamError();
}

void Team::updateTeamError() {
    m_teamError = 0;
    for (const auto& player : m_players) {
        m_teamError += abs(player.getRank() - m_avgRank);
    }
}

void Team::showTeamPlayer() {
    printf("Team error %d, Team players:", getTeamError());
    for (const auto& player : m_players) {
        printf(" %d", player.getId());
    }
    printf("\n");
}

#if defined(__UT_PLAYER__) && (__UT_PLAYER__ == 1)
int main(int argc, char* argv[]) {
    Player p1(1, 33, 2); // id, rank, waiting_time
    Player p2(2, 22, 3);
    Player p3(3, 11, 4);
    Player p4(4, 44, 5);
    Player p5(5, 77, 1);
    Player p6(6, 66, 0);
    p6.waitOnetick();
    p6.print();
    Team                teams[2] = {};
    std::vector<Player> simluateTeams;
    simluateTeams.push_back(p1);
    simluateTeams.push_back(p2);
    simluateTeams.push_back(p3);
    simluateTeams.push_back(p4);
    simluateTeams.push_back(p5);
    simluateTeams.push_back(p6);

    for (int i = 0; i < 6; i++) {
        const int which = (i % 4 == 1 || i % 4 == 2) ? 1 : 0;
        teams[which].addPlayer(simluateTeams[i]);
    }
    teams[0].showTeamPlayer();
    teams[1].showTeamPlayer();

    return 0;
}
#endif // #if defined(__UT_PLAYER__) && (__UT_PLAYER__ == 1)
