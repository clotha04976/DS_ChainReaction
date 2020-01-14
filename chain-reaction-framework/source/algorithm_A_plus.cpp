
#ifndef _ALGO_A_
#define _ALGO_A_

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
#define INF 999
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
float evaluate(Board board);
void gainTree();
bool checkNewGame(Board board);
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
        void evaluateScore(Board board){_score = evaluate(board);};
        void changeTurn(){_turn = -_turn;};      
          
};
int checkAllTheSameColor(Board board);//1: we win, 0 still play, -1 we lose
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
                currentMove.setScore(INF+1);
            } 
            else{
                currentMove.setScore(-INF-1);
            }
            return currentMove;
        }
        else{  //WIN or Lose the game
            int EndOfGame = checkAllTheSameColor(board);
            if(EndOfGame!=0){ 
                currentMove.setScore(INF * EndOfGame);
                return currentMove;
            }
        }
        
    }
    // Terminating condition. i.e leaf node is reached 
    if (depth == h) {
        currentMove.evaluateScore(board);
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
        float max = -INF-1;
        bool willLose = true;
        gameMove current;
        // cout << "layer = " << depth << endl;
        for (int i = 0; i < ROW; i++){
            for(int j=0; j< COL; j++){
                currentMove.children[i][j].setAlpha( currentMove.getAlpha());
                currentMove.children[i][j].setBeta( currentMove.getBeta());
                gameMove currentPredict =minimax(currentMove.children[i][j], false, board, -turn, h, depth+1);
                // if (depth ==0){
                //     printf("%4.2f  ", currentPredict.getScore());
                // }
                
                if (max <  currentPredict.getScore()){
                    current = currentMove.children[i][j];
                    current.setScore(currentPredict.getScore());
                    max = current.getScore();
                    willLose = false;
                    if (currentMove.getAlpha() < currentPredict.getScore()){
                        currentMove.setAlpha( currentPredict.getScore()) ;
                    }
                    if(currentMove.getAlpha() > currentPredict.getBeta()){
                        return current;
                    }
                }
            }
            // if (depth ==0){
            //     cout << endl;
            // }
        }
        // if(willLose){
        //     cout << "What ever the color:" << color <<" choose, it will lose. So sad." << endl;
        //     h = 1;
        //     goto predictAgain;
        // }
        // cout << "Max : " << current.getRow() << ", " << current.getCol() << endl;
        return current; 
    }
    // Else (If current move is Minimizer), find the minimum attainable value 
    else{
        float min = INF+1;
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
                        return current;
                    }
                    
                }
            }
            // cout <<endl;
        }
        // cout << "min : " << current.getRow() << ", " << current.getCol() << endl;
        return current;
    }
        // return min(minimax(depth+1, nodeIndex*2, true, scores, h), minimax(depth+1, nodeIndex*2 + 1, true, scores, h)); 
}

/*********************************************************************/

bool checkNewGame(Board board){
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
void getBoardValue(Board board){
    /**
    1. get the board information -> each orbs in the board and record it
    **/
    int i, j;
    for (i=0; i <5; i++){
        for (j=0;j<6;j++){
            if (color == board.get_cell_color(i,j))
                boardValue[i][j] = board.get_orbs_num(i, j);
            else
                boardValue[i][j] = board.get_orbs_num(i, j)* -1;
        }
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
                if(orbNum > board.get_orbs_num(i -1,j)){
                    value += sameColor * board.get_orbs_num(i -1,j) *ATCCP * ATCC;
                }
            }
            if (i!=5){ //look down
                if(orbNum > board.get_orbs_num(i +1,j)){
                    value +=sameColor *  board.get_orbs_num(i +1,j) *ATCCP * ATCC;
                }
            }
            if(j!=0){ //look left
                if(orbNum > board.get_orbs_num(i,j -1)){
                    value +=sameColor *  board.get_orbs_num(i ,j-1) *ATCCP * ATCC;
                }
            }
            if (j!=6){ //look right
                if(orbNum > board.get_orbs_num(i,j+1)){
                    value +=sameColor *  board.get_orbs_num(i ,j+1) *ATCCP * ATCC;
                }
            }

        }
    }
    return value;
}

#endif

void algorithm_A_plus(Board board, Player player, int index[]){
    int h = 4;
    // cout << board.get_capacity(0, 0) << endl;
    // cout << board.get_orbs_num(0, 0) << endl;
    // cout << board.get_cell_color(0, 0) << endl;
    // board.print_current_board(0, 0, 0);

    //////////// Random Algorithm ////////////
    // Here is the random algorithm for your reference, you can delete or comment it.
    srand(time(NULL));
    int row, col;
    float score;
    color = player.get_color();
    // cout << player.get_color() << endl;
    int i; int j;
    // Board boardForPredict = board;

    if (checkNewGame(board)){
        row = 5* rand() % 2;
        col = 6* rand() % 2;
    }
    else{
        gameMove Next;
        Next = minimax(Next, true, board, 1, h);
        row = Next.getRow();
        col = Next.getCol();
        score = Next.getScore();
    }

    
    
    index[0] = row;
    index[1] = col;
    
    // cout << "next: " << row <<", " << col << " Score = " << score << endl;
    //system("pause");
}
// */