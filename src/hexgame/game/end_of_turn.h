#ifndef END_OF_TURN
#define END_OF_TURN

class Game;
class Unit;

class EndOfTurn {
public:
    EndOfTurn(Game *game): game(game) { }
    void apply();

private:
    void restore_moves(Unit& unit);
    void restore_health(Unit& unit);

private:
    Game *game;
};

#endif
