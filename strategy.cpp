class Strategy {
public:
  Strategy(GameState *gamestate) gs(gamestate){}

  int evaluate() {
    if(is_winning_board())
      return MAX;

    if(is_losing_board())
      return MIN;
    // Calculate budget

    advanced_calc();
    return 0;
  }

  bool is_winning_move(int x, int y) {
    return false;
  }

  // Is line contained by x,y blocked going south/east depending on player
  bool is_blocked(int player, int x, int y) {
    return false;
  }

  bool is_connected(int player, int x, int y) {
    return false;
  }

  // Underlying gamestate will have to be updated every round

protected:
  GameState *gs;
  int MIN = 0;
  int MAX = 100000000;
private:
  virtual int advanced_calc() = 0;
};

class StrategyA: public Strategy {
public:
  StrategyA(Gamestate *gamestate) Strategy(gamestate) {}

private:
  int advanced_calc() {
    return 0;
  }
}  
