#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "../include/algorithm.h"
#include "../include/gameTree.h"

using namespace std;

void algorithm_B(Board board, Player player, int index[]){
    // algorithm_B is randomMove version bot
    srand(time(NULL)*time(NULL));
    int row, col;
    char color = player.get_color();
    
    while(1){
        row = rand() % 5;
        col = rand() % 6;
        if(board.get_cell_color(row, col) == color || board.get_cell_color(row, col) == 'w') break;
    }

    index[0] = row;
    index[1] = col;
}

void algorithm_C(Board board, Player player, int index[]){
    // algorithm_C is noLook version bot
    int row=rand()%5, col=rand()%6;
    char color = player.get_color();

    // Scan through the board to find a cell that has min (capacity-orbNum)
    int orbLack=4;
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)==color){
                if( (board.get_capacity(i,j)-board.get_orbs_num(i,j))<=orbLack ){
                    row=i;
                    col=j;
                }
            }
        }
    }
    index[0]=row;
    index[1]=col;
}

void algorithm_D(Board board, Player player, int index[]){
    // algorithm_D is heithoff version bot
    gameTree gt(board,&player);
    TreeNode *orbNode = gt.chooseOrb(EASY_BOARD_EVALUATOR);
    index[0] = orbNode->getX();
    index[1] = orbNode->getY();
}

void algorithm_E(Board board, Player player, int index[]){
    // algorithm_E is heithoff version bot
    gameTree gt(board,&player);
    TreeNode *orbNode = gt.chooseOrb(HARD_BOARD_EVALUATOR);
    index[0] = orbNode->getX();
    index[1] = orbNode->getY();
}
