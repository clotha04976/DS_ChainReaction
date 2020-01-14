#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include "../include/board.h"
#include "../include/player.h"
#include "../include/rules.h"
#define ROW 5
#define COL 6
#define INF 999
#define ILLEGAL_SCORE 1000
using namespace std;
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
Board predictBoard;
int boardValue[ROW][COL];
char color;
Player predict_red_player(RED);
Player predict_blue_player(BLUE);
void getBoard(Board board);
float evaluate(Board board, int nextTurn);
float evaluate2(Board board);
void gainTree();
bool isNewGame(Board board);
// int alpha, beta;
class gameMove{
    private:
        int _row;
        int _col;
        int _turn; // 1: your turn, -1: enermy's turn
        float _score;
        float _alpha  ;
        float _beta ;
    public:
        gameMove():_row(-1),_col(-1),_turn(1),_score(0), _alpha(-(INF+1)), _beta(INF+1){};
        gameMove(int row, int col, int turn):_row(row),_col(col),_turn(turn),_score(0), _alpha(-(INF+1)), _beta(INF+1){};
        gameMove** children;
        int getRow(){return _row;};
        int getCol(){return _col;};
        int getTurn(){return _turn;};
        float getAlpha(){return _alpha;};
        float getBeta(){return _beta;};
        float getScore(){return _score;};
        void setRow(int row){_row = row;};
        void setCol(int col){_col = col;};
        void setTurn(int turn){_turn = turn;};
        void setScore(float score){_score = score;};
        void setAlpha(float alpha){_alpha = alpha;};
        void setBeta(float beta){_beta = beta;};
        void evaluateScore(Board board, int nextTurn){_score = evaluate(board, nextTurn);};
        void changeTurn(){_turn = -_turn;};              
};
int checkAllTheSameColor(Board board);//1: we win, 0 still play, -1 we lose

bool isRoot(gameMove current){
    return (current.getCol() == -1 || current.getRow() == -1);
}
bool renewBoard(Board* board, gameMove currentMove, int turn){
    bool isLegal = false;
    if (turn == -1){  //Our turn
        if (color == 'r'){ 
            isLegal = board->place_orb(currentMove.getRow(), currentMove.getCol(), &predict_red_player);
        }
        else{
            isLegal =board->place_orb(currentMove.getRow(), currentMove.getCol(), &predict_blue_player);
        }
    }
    else{ //enemy's turn
        if (color =='b'){//our color is blue
            isLegal = board->place_orb(currentMove.getRow(), currentMove.getCol(), &predict_red_player);
        }
        else{
            isLegal = board->place_orb(currentMove.getRow(), currentMove.getCol(), &predict_blue_player);
        }
    }
    return isLegal;
}
bool isLeaf(int depth, int height){
    return depth == height;
}

void childrenIniti(gameMove* currentMove){
    currentMove->children = new gameMove*[ROW];
    for (int i = 0 ; i <ROW; i++){
        currentMove->children[i] = new gameMove[COL];
        for (int j =0; j < COL ; j++){
            currentMove->children[i][j].setRow(i);
            currentMove->children[i][j].setCol(j);
        }
    }
}
// Returns the optimal value a maximizer can obtain. 
// depth is current depth in game tree. 
// nodeIndex is index of current node in scores[]. 
// isMax is true if current move is 
// of maximizer, else false 
// scores[] stores leaves of Game tree. 
// h is maximum height of Game tree 
gameMove minmax(gameMove& currentMove, bool isMax, Board board, int turn, int h = 2,int depth = 0) {//turn = -1 is after enemy's turn 
    if (!isRoot(currentMove)){
        bool isLegal = renewBoard(&board, currentMove, turn);
        if (!isLegal){ //put the wrong place
            if (isMax) currentMove.setScore(ILLEGAL_SCORE);
            else       currentMove.setScore(-ILLEGAL_SCORE);
            return currentMove;
        }
        else{  // is End of Game ?
            int EndOfGame = checkAllTheSameColor(board);
            if (EndOfGame == 1 || EndOfGame == -1){ 
                currentMove.setScore(INF * EndOfGame);
                return currentMove;
            }
        }
    }
    // Terminating condition. leaf node is reached 
    if (isLeaf(depth, h)){
        currentMove.evaluateScore(board, -turn);
        return currentMove;
    }
    // initializing the current move
    childrenIniti(&currentMove);
    //  If current move is maximizer, find the maximum attainable value 
    gameMove current;
    if (isMax){
        float max = -INF-1;
        for (int i = 0; i < ROW; i++){
            for(int j=0; j< COL; j++){
                currentMove.children[i][j].setAlpha(currentMove.getAlpha() );
                currentMove.children[i][j].setBeta(currentMove.getBeta() );
                gameMove currentPredict = minmax(currentMove.children[i][j], false, board, -turn, h, depth+1);
                // if (depth ==0)  printf("%4.2f  ", currentPredict.getScore());
                if (max <  currentPredict.getScore()){
                    current = currentMove.children[i][j];
                    current.setScore(currentPredict.getScore());
                    max = current.getScore();
                    if (currentMove.getAlpha() < currentPredict.getScore()){
                        currentMove.setAlpha( currentPredict.getScore()) ;
                    }
                    if(currentMove.getAlpha() > currentPredict.getBeta()){
                        return current;
                    }
                }
            }
            // if (depth ==0)  cout << endl;
        }
    }
    else{
        float min = INF+1;
        for (int i = 0; i < ROW; i++){
            for(int j=0; j< COL; j++){
                gameMove currentPredict =minmax(currentMove.children[i][j], true, board,-turn, h,depth+1);
                if (min > currentPredict.getScore()){
                    current = currentMove.children[i][j];
                    current.setScore(currentPredict.getScore());
                    min = current.getScore();
                    if (currentMove.getBeta() > currentPredict.getScore()){
                        currentMove.setBeta( currentPredict.getScore()) ;
                    }
                    if(currentMove.getAlpha() > currentPredict.getBeta()){
                        return current;
                    }
                }
            }
        }
    }
    return current;
}

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
float evaluateAround(Board board, int nextTurn, int row, int col){
    int adjustX, adjustY, xBound, yBound;
    adjustX = 1;
    adjustY = 1;
    xBound = 1;
    yBound = 1;
    float val =0.0;
    char centerColor = board.get_cell_color(row,col);
    int centerNum = board.get_orbs_num(row, col);
    int centerCapacity = board.get_capacity(row, col);
    int centerNumToExplosure = centerCapacity - centerNum;
    bool isCenterColorNextTrun;
    if (centerColor == color){
        if(nextTurn == 1){
            isCenterColorNextTrun = false;
        }
        else{
            isCenterColorNextTrun = true;
        }
    }
    else {
        if(nextTurn == 1){
            isCenterColorNextTrun = true;
        }
        else{
            isCenterColorNextTrun = false;
        }
    }
    for (int v = -yBound; v != yBound + adjustY; ++v) {
        for (int u = -xBound; u != xBound + adjustX; ++u) {
            if (col + u >= 0 && col + u < COL && row + v >= 0 && row + v < ROW) {
                if(board.get_cell_color(row+v, col+u) =='w') {
                    continue;
                }
                else{
                    char currentColor = board.get_cell_color(row+v, col+u);
                    float currentNum = board.get_orbs_num(row+v, col+u);
                    float currentCapacity = board.get_capacity(row+v, col+u);
                    float currentNumToExplosure = currentCapacity - currentNum;
                    if (currentColor == centerColor){
                        if (0 == v+u){ //center
                            continue;
                        }
                        else if(0 == (v+u)%2) {//coner
                            val += 0.01;
                        }
                        else{ //edge
                            val -= 0.01;
                        }
                    }
                    else{
                        if(0 == (v+u)%2) {//coner
                            val -= 0.01;
                        }
                        else{ //edge
                            if(currentNumToExplosure < centerNumToExplosure){
                                val -= (float)centerNum * 0.05* currentNumToExplosure/(currentCapacity-1);
                            }
                            else if(currentNumToExplosure == centerNumToExplosure){
                                if (isCenterColorNextTrun){
                                    val += (float)currentNum *0.05 * centerNumToExplosure/(centerCapacity-1);
                                }
                                else{
                                    val -= (float)centerNum *0.05 * currentNumToExplosure/(currentCapacity-1);
                                }
                            }
                            else{
                                val += (float)currentNum * 0.05 * centerNumToExplosure/(centerCapacity-1);
                            }
                        }
                    }
                }
            }
        }
    }
    if (centerColor == color)
        return val; 
    else
        return -val;
}

float evaluate(Board board, int nextTurn){ //O(n)
    int i, j;
    float value =0;
    float ATCCP = 0.3;
    float ATCC ; //the given point for ability to change color 
    for (i=0; i <5; i++){
        for (j=0;j<6;j++){
            int orbNum = board.get_orbs_num(i,j);
            int cellCapacity = board.get_capacity(i,j);
            int sameColor;
            if (board.get_cell_color(i,j)=='w'){
                continue;
            }
            else if (board.get_cell_color(i,j)==color){
                sameColor = 1;
            }
            else{
                sameColor = -1;
            }
            ATCC = float(orbNum)/(cellCapacity-1); //ability to change color; corner:+1, edge:1/2, center: 1/3
            value += float(sameColor) * (orbNum + ATCCP* ATCC * cellCapacity); // A. B. evaluate the atcc
            //  orbNum + 0.3 * (corner = 2, edge =3/2, center= 4/3) = (corner: 1.6, edge 1.45, center 1.4)
            //evaluate Around
            value +=  evaluateAround(board, nextTurn, i, j);;
        }
    }
    return value;
}

void placeCorner(int index[]){
    srand(time(NULL));  
    index[0] = (ROW-1)* (rand() % 2);
    index[1] = (COL-1)* (rand() % 2);
}

void algorithm_A(Board board, Player player, int index[]){
    int h=4;
    color = player.get_color();
    int i; int j;
    if (isNewGame(board)){   
        placeCorner(index);
    }
    else{
        gameMove Next;
        Next = minmax(Next, true, board, 1, h);
        index[0] = Next.getRow();
        index[1] = Next.getCol();
    }
}