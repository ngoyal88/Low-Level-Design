#include <bits/stdc++.h>
using namespace std;

struct Position {
    int x, y;
};
enum struct Mark {EMPTY = ' ', X = 'X', O = 'O'};

//--------BOARD CLASS--------
class Board{
    vector<vector<Mark>> board;
    public:
    Board(){
        board.resize(3,vector<Mark>(3,Mark::EMPTY));
    }
    Mark get(Position pos){
        return board[pos.x][pos.y];
    }
    bool isEmpty(Position pos){
        return board[pos.x][pos.y] == Mark::EMPTY;
    }
    bool isValid(Position pos){
        return pos.x>=0 && pos.x<3 && pos.y>=0 && pos.y<3;
    }
    bool isFull(){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                if(board[i][j] == Mark::EMPTY) return false;
            }
        }
    return true;
    }
    bool place(Position pos, Mark mark){
        if(isValid(pos) && isEmpty(pos)){
            board[pos.x][pos.y] = mark;
            return true;
        }
        return false;
    }
    void printBoard(){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                cout<<char(board[i][j]);
                if(j<2) cout<<"|";
            }
            cout<<"\n";
            if(i<2) cout<<"-----\n";
        }
    }

};
//========Refree========
class Refree{
    public:
    bool checkWin(Board &board){
        for(int i=0;i<3;i++){
            //row check
            if(board.get({i, 0}) != Mark::EMPTY && board.get({i, 0}) == board.get({i, 1}) && board.get({i, 1}) == board.get({i, 2})){
                return true;
            }
            //column check
            if(board.get({0, i}) != Mark::EMPTY && board.get({0, i}) == board.get({1, i}) && board.get({1, i}) == board.get({2, i})){
                return true;
            }
        }
        //diagonal check
        if(board.get({0, 0}) != Mark::EMPTY && board.get({0, 0}) == board.get({1, 1}) && board.get({1, 1}) == board.get({2, 2})){
            return true;
        }
        if(board.get({0, 2}) != Mark::EMPTY && board.get({0, 2}) == board.get({1, 1}) && board.get({1, 1}) == board.get({2, 0})){
            return true;
        }
        return false;
    }
};
//--------Player CLASS--------
class Player{
    protected:
    string name;
    Mark mark;
    public:
    Player(string name, Mark mark){
        this->name = name;
        this->mark = mark;
    }
    string getName(){
        return name;
    }
    Mark getMark(){
        return mark;
    }
    virtual Position getMove(Board &board) = 0;
};
//--------HUMAN PLAYER ---------
class HumanPlayer : public Player{
    public:
    HumanPlayer(string name, Mark mark) : Player(name, mark) {}

    Position getMove(Board &board){
        while(true){
        int x, y;
        cout<<"Enter your move (row and column): ";
        cin>>x>>y;
        Position p{x, y};
        if(!board.isValid(p)) cout<<"Invalid move. Try again."<<endl;
        if(!board.isEmpty(p)) cout<<"Position already taken. Try again."<<endl;
        return p;
     }
 }
};

//--------AI PLAYER ---------
class AIPlayer : public Player{
    public:
    AIPlayer(string name, Mark mark) : Player(name, mark) {}

    Position getMove(Board &board){
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                Position p{i,j};
                if(board.isEmpty(p)) return p;
            }
        }
        return Position{-1,-1};
    }
};
//--------GAME CLASS--------
class Game{
    private:
    Board board;
    Refree refree;
    Player *player1;
    Player *player2;
    Player *currentPlayer;
    public:
    Game(HumanPlayer *p1,AIPlayer *p2){
        player1 = p1;
        player2 = p2;
        currentPlayer = player1;
    }
    void SwitchPlayer(){
        currentPlayer = (currentPlayer == player1) ? player2 : player1;
    }
    void playGame(){
        int moves = 0;
        while(moves<9){
            board.printBoard();
            Position move = currentPlayer->getMove(board);
            board.place(move, currentPlayer->getMark());
            moves++;
            if(refree.checkWin(board)){
                board.printBoard();
                cout<<currentPlayer->getName()<<" wins!"<<endl;
                return;
            }
            if(board.isFull()){
                board.printBoard();
                cout<<"It's a draw!"<<endl;
                return;
            }
            cout<<"===== "<<currentPlayer->getName()<<"'s turn ====="<<endl;
            SwitchPlayer();
        }
    }
    void resetGame(){
        board = Board();
        currentPlayer = player1;
    }

};
int main() {
    std::cout << "Welcome to Tic-Tac-Toe!\n";
    // Game logic will go here
    HumanPlayer p1("Player 1", Mark::X);
    AIPlayer p2("Computer", Mark::O);
    Game game(&p1, &p2);
    game.playGame();
    game.resetGame();
    return 0;
}