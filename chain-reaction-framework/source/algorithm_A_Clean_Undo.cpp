
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include "../include/board.h"
#include "../include/player.h"
#include "../include/rules.h"
#include <stdlib.h>

#define ROW 5
#define COL 6

#define BEST_SCORE 999
#define WORST_SCORE -999

#define OUR_WRONG_PLACE -1000
#define ENEMY_WRONG_PLACE 1000

using namespace std;

// enum turn {myTurn =1, enemysTurn = -1};
/******************************************************
 * In your algorithm, you can just use the the funcitons
 * listed by TA to get the board information.(functions 
 * 1. ~ 4. are listed in next block)
 * 
 * The STL library functions is not allowed to use.
******************************************************/

/*************************************************************************
 * 1. int board.get_orbs_num(int row_index, int col_index)
 * 2. int board.get_capacity(int row_index, int col_index)
 * 3. char board.get_cell_color(int row_index, int col_index)
 * 4. void board.print_current_board(int row_index, int col_index, int round)
 * 
 * 1. The function that return the number of orbs in cell(row, col)
 * 2. The function that return the orb capacity of the cell(row, col)
 * 3. The function that return the color fo the cell(row, col)
 * 4. The function that print out the current board statement
*************************************************************************/
char color;
Player predict_red_player(RED);
Player predict_blue_player(BLUE);
float evaluate(Board board);
void gainTree();
bool isNewGame(Board board);
void chooseCorner(int* index);

class gameMove{
    private:
        int _row; int _col;
        int _turn; // 1: your turn, -1: enermy's turn
        Board _boardState;
        float _score;
        float _alpha;//the max score ever recorded (for alpha-beta pruning)
        float _beta; // the min score ever recorded (for alpha-beta pruning)
        int checkAllTheSameColor(Board board); //1: we win, 0 still play, -1 we lose
    public:
        gameMove():_row(-1),_col(-1),_turn(1),_score(0), _alpha(OUR_WRONG_PLACE), _beta(ENEMY_WRONG_PLACE){};
        gameMove(Board board):_boardState(board),_row(-1),_col(-1),_turn(1),_score(0), _alpha(OUR_WRONG_PLACE), _beta(ENEMY_WRONG_PLACE){};
        gameMove(Board board, int row, int col, int turn):_boardState(board),_row(row),_col(col),_turn(turn),_score(0), _alpha(OUR_WRONG_PLACE), _beta(ENEMY_WRONG_PLACE){};
        gameMove** children;
        // get method //
        int getRow(){return _row;};
        int getCol(){return _col;};
        int getTurn(){return _turn;};
        Board getBoard(){return _boardState;};
        int changeTurn(){return -_turn;};      
        float getAlpha(){return _alpha;};
        float getBeta(){return _beta;};
        float getScore(){return _score;};
        // set method//
        void setRow(int row){_row = row;};
        void setCol(int col){_col = col;};
        void setTurn(int turn){_turn = turn;};
        void setScore(float score){_score = score;};
        void setAlpha(float alpha){_alpha = alpha;};
        void setBeta(float beta){_beta = beta;};
        void setBoard(Board board){_boardState = board;};
        //method//
        float evaluate();
        
};
float gameMove::evaluate(){
    //Note: this algo can speed up, by adding a visited flag. 
    //because most of time the board doesn't change too much
    /*
    2. valudate the board (only)
        A. the total num_orbs in the board, if it is yours, then get a point, if not, reduce a point
        B. count the ability to change color: num_orbs/(capacity - 1)
            the ATCC point = 0.1~0.01 ???
            value += color * ATCC_point * ability_to_change_color(ATCC) * capacity
        C. if orbs nums are more than the enermy surround, then get more point 
    ****/
    int i, j;
    float value =0;
    float ATCCP = 0.3; //the given point for ability to change color 
    float ATCC ;
    for (i=0; i <5; i++){
        for (j=0;j<6;j++){
            int orbNum = this->_boardState.get_orbs_num(i,j);
            int cellCapacity = this->_boardState.get_capacity(i,j);
            int sameColor;
            if (this->_boardState.get_cell_color(i,j)=='w'){
                sameColor = 0;
            }
            else if (this->_boardState.get_cell_color(i,j)==color){
                sameColor = 1;
            }
            else{
                sameColor = -1;
            }
            
            ATCC = float(orbNum)/(cellCapacity-1); //ability to change color;
            // if ((orbNum*sameColor)!=0)
                // cout <<  " at " << i << ", " << j << ": sameColor * (orbNum + ATCCP* ATCC * cellCapacity) = " << sameColor <<"* (" << orbNum << "+ " << ATCCP << "* " << ATCC << "*" << cellCapacity << ")= " <<sameColor * (orbNum + ATCCP* ATCC * cellCapacity) << endl;
            
            value += float(sameColor) * (orbNum + ATCCP* ATCC * cellCapacity); // A. B. evaluate the atcc
            
            //evaluate surround
            if (i!=0){// look up
                if(orbNum > this->_boardState.get_orbs_num(i -1,j) && this->_boardState.get_cell_color(i-1,j)!=this->_boardState.get_cell_color(i,j) ){
                    value += sameColor * this->_boardState.get_orbs_num(i -1,j) *ATCCP * ATCC;
                }
            }
            if (i!=5){ //look down
                if(orbNum > this->_boardState.get_orbs_num(i +1,j) && this->_boardState.get_cell_color(i+1,j)!=this->_boardState.get_cell_color(i,j)){
                    value +=sameColor *  this->_boardState.get_orbs_num(i +1,j) *ATCCP * ATCC;
                }
            }
            if(j!=0){ //look left
                if(orbNum > this->_boardState.get_orbs_num(i,j -1) && this->_boardState.get_cell_color(i,j-1)!=this->_boardState.get_cell_color(i,j)){
                    value +=sameColor *  this->_boardState.get_orbs_num(i ,j-1) *ATCCP * ATCC;
                }
            }
            if (j!=6){ //look right
                if(orbNum > this->_boardState.get_orbs_num(i,j+1) && this->_boardState.get_cell_color(i-1,j+1)!=this->_boardState.get_cell_color(i,j)){
                    value +=sameColor *  this->_boardState.get_orbs_num(i ,j+1) *ATCCP * ATCC;
                }
            }

        }
    }
    return value;
}

class minMaxTree{
    private:
        gameMove* root;
        Player* player;
        Player* enemy;
        minMaxTree** children;
    public:
        minMaxTree(Board board, Player *player, int turn);
        minMaxTree();
        void init(Board board, Player *player, int turn);
        void evaluateButtomUp();
        void chooseBest(int* index, int predictDepth);
        void buildNextRound(gameMove* currentMove);
        void buildNextRound();
};

minMaxTree::minMaxTree(Board board, Player *player, int turn=-1){
    this->root = new gameMove(board, -1 ,-1, turn);
    this->player = player;
    char colorOfPlayer = player->get_color();
    char colorOfEnemy = 'v'; // virtual player
    bool leaveForLoop = false;
    //get enemy's color
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)!='w' && board.get_cell_color(i,j)!=colorOfPlayer){
                colorOfEnemy = board.get_cell_color(i,j);
                leaveForLoop = true;
                break;
            }
        }
        if (leaveForLoop)
            break;
    }
    this->enemy = new Player(colorOfEnemy);
}

void minMaxTree::init(Board board, Player *player, int turn = -1){
    this->root = new gameMove(board, -1 ,-1, turn);
    this->player = player;
    char colorOfPlayer = player->get_color();
    char colorOfEnemy = 'v'; // virtual player
    bool leaveForLoop = false;
    //get enemy's color
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)!='w' && board.get_cell_color(i,j)!=colorOfPlayer){
                colorOfEnemy = board.get_cell_color(i,j);
                leaveForLoop = true;
                break;
            }
        }
        if (leaveForLoop)
            break;
    }
    this->enemy = new Player(colorOfEnemy);
}

void minMaxTree::buildNextRound(gameMove* current){
    current->children = new gameMove*[ROW];
    for (int i = 0 ; i <ROW; i++){
        current->children[i] = new gameMove[COL];
        for (int j =0; j < COL ; j++){
            bool isLegal = false;
            Board board = current->getBoard();
            if (current->getTurn() ==-1){  
                isLegal = board.place_orb(i, j, this->player); //Our turn
            }else{
                isLegal = board.place_orb(i, j, this->enemy);
            }
            
            if (isLegal){
                current->children[i][j].setRow(i);
                current->children[i][j].setCol(j);
                current->children[i][j].setTurn(current->changeTurn());
                current->children[i][j].setBoard(board);
            }
            else{
                if (current->getTurn() == -1){
                    current->children[i][j].setScore(OUR_WRONG_PLACE);
                }else{
                    current->children[i][j].setScore(ENEMY_WRONG_PLACE);
                }
                
            }
        }
    }
}

void minMaxTree::buildNextRound(){
    Board board = this->root->getBoard();
    this->children = new minMaxTree*[ROW];
    for (int i = 0 ; i <ROW; i++){
        this->children[i] = new minMaxTree[COL];
        for (int j =0; j < COL ; j++){
            bool isLegal = false;
            Board board = this->root->getBoard();
            if (this->root->getTurn() ==-1){  
                isLegal = board.place_orb(i, j, this->player); //Our turn
            }else{
                isLegal = board.place_orb(i, j, this->enemy);
            }
            
            if (isLegal){
                this->children[i][j].init(board, enemy);
            }
            else{
                if (this->getTurn() == -1){
                    this->children[i][j].setScore(OUR_WRONG_PLACE);
                }else{
                    this->children[i][j].setScore(ENEMY_WRONG_PLACE);
                }
                
            }
        }
    }
}
bool isLeaf(int height){
    if (height == 0)
        return true;
    else
        return false;
}

void minMaxTree::chooseBest(minMaxTree* current, int* index, int height = 4){
    if(isLeaf(height)){
        
    }else{
        buildNextRound(current);


    }
    
}

// Returns the optimal value a maximizer can obtain. 
// depth is current depth in game tree. 
// nodeIndex is index of current node in scores[]. 
// isMax is true if current move is 
// of maximizer, else false 
// scores[] stores leaves of Game tree. 
// h is maximum height of Game tree 
gameMove minimax(gameMove& currentMove, bool isMax, Board board, int turn, int h = 2,int depth = 0 ) {//turn = -1 is after enemy's turn 
    
    //renew the board
    if (!(currentMove.getCol() == -1 || currentMove.getRow() == -1)){
        bool flag = false;
        // Player currentPlayer();
        if (turn == -1){  //Our turn
            if (color == 'r'){ 
                flag = board.place_orb(currentMove.getRow(), currentMove.getCol(), &predict_red_player);
            }
            else{
                flag =board.place_orb(currentMove.getRow(), currentMove.getCol(), &predict_blue_player);
            }
        }
        else{ //enemy's turn
            if (color =='b'){//our color is blue
                flag = board.place_orb(currentMove.getRow(), currentMove.getCol(), &predict_red_player);
            }
            else{
                flag = board.place_orb(currentMove.getRow(), currentMove.getCol(), &predict_blue_player);
            }
        }
        if (!flag){ //put the wrong place
            if (isMax){
                currentMove.setScore(ENEMY_WRONG_PLACE);
            } 
            else{
                currentMove.setScore(OUR_WRONG_PLACE);
            }
            return currentMove;
        }
        else{  //WIN or Lose the game
            int EndOfGame = checkAllTheSameColor(board);
            if(EndOfGame!=0){ 
                currentMove.setScore(BEST_SCORE * EndOfGame);
                return currentMove;
            }
        }
        
    }
    // Terminating condition. i.e leaf node is reached 
    if (depth == h) {
        currentMove.evaluate();
        // board.print_current_board(currentMove.getRow(), currentMove.getCol(), 0);
        // cout << "If next :" << currentMove.getRow() << ", " << currentMove.getCol() <<" Score = " << currentMove.getScore() << endl;
        // system("pause");
        return currentMove;
    }
    // initializing the current move
    currentMove.children = new gameMove*[ROW];
    for (int i = 0 ; i <ROW; i++){
        currentMove.children[i] = new gameMove[COL];
        for (int j =0; j < COL ; j++){
            currentMove.children[i][j].setRow(i);
            currentMove.children[i][j].setCol(j);
            // currentMove.children[i][j].setTurn(-turn);
        }
    }
    //  If current move is maximizer, find the maximum attainable value 
    if (isMax){
        float max = OUR_WRONG_PLACE;
        bool willLose = true;
        gameMove current;
        for (int i = 0; i < ROW; i++){
            for(int j=0; j< COL; j++){
                currentMove.children[i][j].setAlpha( currentMove.getAlpha());
                currentMove.children[i][j].setBeta( currentMove.getBeta());
                gameMove currentPredict =minimax(currentMove.children[i][j], false, board, -turn, h, depth+1);
                if (max <  currentPredict.getScore()){
                    current = currentMove.children[i][j];
                    current.setScore(currentPredict.getScore());
                    max = current.getScore();
                    willLose = false;
                    if (currentMove.getAlpha() < currentPredict.getScore()){
                        currentMove.setAlpha( currentPredict.getScore()) ;
                    }
                    if(currentMove.getAlpha() > currentPredict.getBeta()){
                        //alpha-beta pruning
                        return current;
                    }
                }
            }
        }
        return current; 
    } 
    else{
        float min = ENEMY_WRONG_PLACE;
        gameMove current;
        // cout << "layer = " << depth << endl;
        for (int i = 0; i < ROW; i++){
            for(int j=0; j< COL; j++){
                gameMove currentPredict =minimax(currentMove.children[i][j], true, board,-turn, h,depth+1);
                // cout << currentPredict.getScore() << " ";
                // cout << min << "> " << currentPredict.getScore() << "?" << endl;
                
                if (min > currentPredict.getScore()){
                    current = currentMove.children[i][j];
                    current.setScore(currentPredict.getScore());
                    min = current.getScore();

                    if (currentMove.getBeta() > currentPredict.getScore()){
                        currentMove.setBeta( currentPredict.getScore()) ;
                    }
                    if(currentMove.getAlpha() > currentPredict.getBeta()){
                        //alpha-beta pruning
                        return current;
                    }
                    
                }
            }
            // cout <<endl;
        }
        // cout << "min : " << current.getRow() << ", " << current.getCol() << endl;
        return current;
    }
}

/*********************************************************************/

bool isNewGame(Board board){
    for(int i = 0 ; i < ROW ; i ++)
        for(int j = 0 ; j < COL; j++){
            if (board.get_orbs_num(i, j)!=0)
                return false;
        }
    return true;
}
int checkAllTheSameColor(Board board){
    char player_color = color;
    bool win = true;
    bool lose = true;
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(board.get_cell_color(i,j) == 'w') continue;
            else if(board.get_cell_color(i,j) == player_color){
                lose = false;
            }
            else{
                win = false;
            }
        }
    }
    if(win){
        return 1;
    }
    else if(lose){
        return -1;
    }
    else{
        return 0;
    }
}
float evaluate(Board board){ //O(n)
    //Note: this algo can speed up, by adding a visited flag. 
    //because most of time the board doesn't change too much/
    /****
    2. valudate the board (only)
        A. the total num_orbs in the board, if it is yours, then get a point, if not, reduce a point
        B. count the ability to change color: num_orbs/(capacity - 1)
            the ATCC point = 0.1~0.01 ???
            value += color * ATCC_point * ability_to_change_color(ATCC) * capacity
        C. if orbs nums are more than the enermy surround, then get more point 
    ****/
    int i, j;
    float value =0;
    float ATCCP = 0.3; //the given point for ability to change color 
    float ATCC ;
    for (i=0; i <5; i++){
        for (j=0;j<6;j++){
            int orbNum = board.get_orbs_num(i,j);
            int cellCapacity = board.get_capacity(i,j);
            int sameColor;
            if (board.get_cell_color(i,j)=='w'){
                sameColor = 0;
            }
            else if (board.get_cell_color(i,j)==color){
                sameColor = 1;
            }
            else{
                sameColor = -1;
            }
            
            ATCC = float(orbNum)/(cellCapacity-1); //ability to change color;
            // if ((orbNum*sameColor)!=0)
                // cout <<  " at " << i << ", " << j << ": sameColor * (orbNum + ATCCP* ATCC * cellCapacity) = " << sameColor <<"* (" << orbNum << "+ " << ATCCP << "* " << ATCC << "*" << cellCapacity << ")= " <<sameColor * (orbNum + ATCCP* ATCC * cellCapacity) << endl;
            
            value += float(sameColor) * (orbNum + ATCCP* ATCC * cellCapacity); // A. B. evaluate the atcc
            
            //evaluate surround
            if (i!=0){// look up
                if(orbNum > board.get_orbs_num(i -1,j) && board.get_cell_color(i-1,j)!=board.get_cell_color(i,j) ){
                    value += sameColor * board.get_orbs_num(i -1,j) *ATCCP * ATCC;
                }
            }
            if (i!=5){ //look down
                if(orbNum > board.get_orbs_num(i +1,j) && board.get_cell_color(i+1,j)!=board.get_cell_color(i,j)){
                    value +=sameColor *  board.get_orbs_num(i +1,j) *ATCCP * ATCC;
                }
            }
            if(j!=0){ //look left
                if(orbNum > board.get_orbs_num(i,j -1) && board.get_cell_color(i,j-1)!=board.get_cell_color(i,j)){
                    value +=sameColor *  board.get_orbs_num(i ,j-1) *ATCCP * ATCC;
                }
            }
            if (j!=6){ //look right
                if(orbNum > board.get_orbs_num(i,j+1) && board.get_cell_color(i-1,j+1)!=board.get_cell_color(i,j)){
                    value +=sameColor *  board.get_orbs_num(i ,j+1) *ATCCP * ATCC;
                }
            }

        }
    }
    return value;
}

void algorithm_A(Board board, Player player, int index[]){
    // color = player.get_color();
    if (isNewGame(board)){
        chooseCorner(index);
    }else{
        int h = 4;
        int row, col;
        float score;
        minMaxTree game(board, player);
        game.chooseBest(index[]);

        index[0] = row;
        index[1] = col;
    }
}

void chooseCorner(int* index){
    srand(time(NULL));
    int row, col;
    row = 4*(rand()%2);
    col = 5*(rand()%2);
    index[0] = row;
    index[1] = col;
}