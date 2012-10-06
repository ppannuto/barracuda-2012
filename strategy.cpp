class Strategy {
public:
  Strategy(GameState *gamestate) gs(gamestate){}

  int bid() {
    choose_square();
    
    // Calculate budget
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
private:
  virtual int choose_square() = 0;
};

class StrategyA: public Strategy {
public:
  StrategyA(Gamestate *gamestate) Strategy(gamestate) {}

private:
  int choose_square() {
    return 0;
  }
}  
