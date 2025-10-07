#include <bits/stdc++.h>
using namespace std;
enum class Color { WHITE, BLACK };
struct Position{
    int x,y;
    Position(int x, int y) : x(x), y(y) {}
    bool isValid(){
        return x>=0 && x<8 && y>=0 && y<8;
    }
    bool operator==(const Position &p) const {
        return x == p.x && y == p.y;
    }
};
struct Move{
    Position from;
    Position to;
    bool isCapture = false;
    bool isPromotion = false;
    char promoteTo; // 'Q', 'R', 'B', 'N'
    bool operator<(const Move& other) const {
        if (from.x != other.from.x) return from.x < other.from.x;
        if (from.y != other.from.y) return from.y < other.from.y;
        if (to.x != other.to.x) return to.x < other.to.x;
        if (to.y != other.to.y) return to.y < other.to.y;
        return promoteTo < other.promoteTo;
    }
};
class Board;
class Piece{
    protected:
    Color color;
    public:
    Piece(Color c) : color(c) {}
    Color getColor(){ return color;}
    virtual char Symbol() = 0;
    virtual set<Move> legalMoves(Board &board, Position pos) = 0;
};
class King : public Piece{
    public:
    King(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'K' : 'k'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};

class Queen : public Piece{
    public:
    Queen(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'Q' : 'q'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};
//hathi
class Rook : public Piece{
    public:
    Rook(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'R' : 'r'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};
//trishool
class Bishop : public Piece{
    public:
    Bishop(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'B' : 'b'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};
//ghoda
class Knight : public Piece{
    public:
    Knight(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'N' : 'n'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};


class Pawn : public Piece{
    public:
    Pawn(Color c) : Piece(c){}
    char Symbol() { return color == Color::WHITE ? 'P' : 'p'; }
    set<Move> legalMoves(Board &board, Position pos) override;
};

class Board{
    vector<vector<Piece*>> board;
    public:
    Board(){
        board.resize(8,vector<Piece*>(8,NULL));
        // Initialize pieces
        for(int i=0;i<8;i++){
            board[1][i] = new Pawn(Color::WHITE);
            board[6][i] = new Pawn(Color::BLACK);
        }
        board[0][0] = board[0][7] = new Rook(Color::WHITE); 
        board[7][0] = board[7][7] = new Rook(Color::BLACK);
        board[0][1] = board[0][6] = new Knight(Color::WHITE);
        board[7][1] = board[7][6] = new Knight(Color::BLACK);
        board[0][2] = board[0][5] = new Bishop(Color::WHITE);
        board[7][2] = board[7][5] = new Bishop(Color::BLACK);
        board[0][3] = new Queen(Color::WHITE);
        board[7][3] = new Queen(Color::BLACK);
        board[0][4] = new King(Color::WHITE);
        board[7][4] = new King(Color::BLACK);
    }
    ~Board(){
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                if(board[i][j]) delete board[i][j];
            }
        }
    }
    bool isValid(Position pos){
        return pos.x>=0 && pos.x<8 && pos.y>=0 && pos.y<8;
    }
    Piece* getPiece(Position pos){
        if(isValid(pos)) return board[pos.x][pos.y];
        return NULL;
    }
    void PrintBoard(){
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                if(board[i][j]) cout<<board[i][j]->Symbol()<<" ";
                else cout<<". ";
            }
            cout<<endl;
        }
    }
    // return codes: 0 = invalid move, 1 = moved successfully, 2 = moved and king captured (game over)
    int movePiece(Piece* piece, Position from, Position to){
        if(isValid(from) && isValid(to) && board[from.x][from.y] == piece 
        && piece->legalMoves(*this, from).count(Move{from, to})){
            Piece* dest = board[to.x][to.y];
            bool kingCaptured = false;
            if(dest){
                // detect if captured piece is a King
                King* k = dynamic_cast<King*>(dest);
                if(k) kingCaptured = true;
                // delete captured piece to avoid leak
                delete dest;
            }
            board[to.x][to.y] = piece;
            board[from.x][from.y] = NULL;
            return kingCaptured ? 2 : 1;
        }else{
            cout<<"Invalid Move"<<endl;
            cout<<"choose from the following moves:"<<endl;
            for(auto &m : piece->legalMoves(*this, from)){
                cout<<"("<<m.from.x<<","<<m.from.y<<") -> ("<<m.to.x<<","<<m.to.y<<")"<<endl;
            }
            return 0;
        }
    }
};

set<Move> King::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int directions[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto &dir : directions){
            int nx = pos.x + dir[0];
            int ny = pos.y + dir[1];
            Position newPos{nx, ny};
            if(newPos.isValid()){
                Piece* p = board.getPiece(newPos);
                if(!p || p->getColor() != color){
                    Move m{pos, newPos};
                    if(p) m.isCapture = true;
                    moves.insert(m);
                }
            }
        }
        return moves;
    }

set<Move> Queen::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int directions[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto &dir : directions){
            for(int step=1;step<8;step++){
                int nx = pos.x + dir[0]*step;
                int ny = pos.y + dir[1]*step;
                Position newPos{nx, ny};
                if(!newPos.isValid()) break;
                Piece* p = board.getPiece(newPos);
                if(!p){
                    Move m{pos, newPos};
                    moves.insert(m);
                } else {
                    if(p->getColor() != color){
                        Move m{pos, newPos};
                        m.isCapture = true;
                        moves.insert(m);
                    }
                    break;
                }
            }
        }
        return moves;
    }

set<Move> Rook::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int directions[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
        for(auto &dir : directions){
            for(int step=1;step<8;step++){
                int nx = pos.x + dir[0]*step;
                int ny = pos.y + dir[1]*step;
                Position newPos{nx, ny};
                if(!newPos.isValid()) break;
                Piece* p = board.getPiece(newPos);
                if(!p){
                    Move m{pos, newPos};
                    moves.insert(m);
                } else {
                    if(p->getColor() != color){
                        Move m{pos, newPos};
                        m.isCapture = true;
                        moves.insert(m);
                    }
                    break;
                }
            }
        }
        return moves;
    }

set<Move> Bishop::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int directions[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
        for(auto &dir : directions){
            for(int step=1;step<8;step++){
                int nx = pos.x + dir[0]*step;
                int ny = pos.y + dir[1]*step;
                Position newPos{nx, ny};
                if(!newPos.isValid()) break;
                Piece* p = board.getPiece(newPos);
                if(!p){
                    Move m{pos, newPos};
                    moves.insert(m);
                } else {
                    if(p->getColor() != color){
                        Move m{pos, newPos};
                        m.isCapture = true;
                        moves.insert(m);
                    }
                    break;
                }
            }
        }
        return moves;
    }

set<Move> Knight::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int directions[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
        for(auto &dir : directions){
            int nx = pos.x + dir[0];
            int ny = pos.y + dir[1];
            Position newPos{nx, ny};
            if(newPos.isValid()){
                Piece* p = board.getPiece(newPos);
                if(!p || p->getColor() != color){
                    Move m{pos, newPos};
                    if(p) m.isCapture = true;
                    moves.insert(m);
                }
            }
        }
        return moves;
    }

set<Move> Pawn::legalMoves(Board &board, Position pos){
        set<Move> moves;
        int dir = (color == Color::WHITE) ? 1 : -1;
        // Move forward
        Position oneStep{pos.x + dir, pos.y};
        if(oneStep.isValid() && !board.getPiece(oneStep)){
            Move m{pos, oneStep};
            if((color == Color::WHITE && oneStep.x == 7) || (color == Color::BLACK && oneStep.x == 0)){
                m.isPromotion = true;
                m.promoteTo = 'Q';
            }
            moves.insert(m);
            // First move can be two steps
            if((color == Color::WHITE && pos.x == 1) || (color == Color::BLACK && pos.x == 6)){
                Position twoStep{pos.x + 2*dir, pos.y};
                if(twoStep.isValid() && !board.getPiece(twoStep)){
                    Move m2{pos, twoStep};
                    moves.insert(m2);
                }
            }           
        }
        // Captures
        int captureDirs[2] = {-1, 1};
        for(int cd : captureDirs){
            Position capPos{pos.x + dir, pos.y + cd};
            if(capPos.isValid()){
                Piece* p = board.getPiece(capPos);
                if(p && p->getColor() != color){
                    Move m{pos, capPos};
                    m.isCapture = true;
                    moves.insert(m);
                }
            }
        }
        return moves;        
    }
    
class Game{
    Board board;
    Color turn;
public:
Game():board(), turn(Color::WHITE) {}
void resetGame(){
    board = Board();
    turn = Color::WHITE;
}
void switchTurn(){
    turn = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
}
void playGame(){
    while(1){
        board.PrintBoard();
        cout<<(turn == Color::WHITE ? "White" : "Black")<<"'s turn. Enter move (from_x from_y to_x to_y): ";
        int fx, fy, tx, ty;
        cin>>fx>>fy>>tx>>ty;
        Position from{fx, fy};
        Position to{tx, ty};
        Piece* piece = board.getPiece(from);
        if(!piece){
            cout<<"No piece at the source position."<<endl;
            continue;
        }

        if(piece->getColor() != turn){
            cout<<"It's not your turn."<<endl;
            continue;
        }
        int result = board.movePiece(piece, from, to);
        if(result == 1){
            switchTurn();
        } else if(result == 2){
            // king captured -> current player wins
            board.PrintBoard();
            cout<<(turn == Color::WHITE ? "White" : "Black")<<" wins! (King captured)"<<endl;
            break;
        }
    }
}
};
int main() {
    std::cout << "Lets Play Chess!\n";
    // Game logic will go here
    Game game;
    game.playGame();
    game.resetGame();
    return 0;
}