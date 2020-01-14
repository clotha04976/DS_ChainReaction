#include<iostream>
#include <time.h>
#include "../include/gameTree.h"

/*-------------------------Implement class TreeNode functions---------------------------*/
TreeNode::TreeNode(Board board){
    this->boardState=board;
    nextNode = NULL;
    nextRoundNode = NULL;
    index[0]=0; index[1]=0;
}

TreeNode::TreeNode(Board board, int x, int y){
    this->boardState=board;
    nextNode = NULL;
    nextRoundNode = NULL;
    this->index[0]=x;
    this->index[1]=y;
}

void TreeNode::setNextNode(TreeNode *Node){
    this->nextNode=Node;
}

void TreeNode::setNextRoundNode(TreeNode *Node){
    this->nextRoundNode=Node;
}

TreeNode* TreeNode::getNextNode(){
    return this->nextNode;
}

TreeNode* TreeNode::getNextRoundNode(){
    return this->nextRoundNode;
}

TreeNode* TreeNode::getTail(){
    TreeNode* temp=NULL;
    TreeNode* tempNext=this;
    while(tempNext!=NULL){
        temp = tempNext;
        tempNext = tempNext->getNextNode();
    }
    return temp;
}

Board TreeNode::getBoardState(){
    return this->boardState;
}

int TreeNode::getX(){
    return this->index[0];
}

int TreeNode::getY(){
    return this->index[1];
}

/*------------------------Implement class gameTree functions----------------------------*/
gameTree::gameTree(Board board, Player *player){
    this->root = new TreeNode(board);
    this->player = player;
    char colorPlayer = player->get_color();
    char colorOpponent = 'v'; // virtual player
    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(board.get_cell_color(i,j)!='w' && board.get_cell_color(i,j)!=colorPlayer)
                colorOpponent = board.get_cell_color(i,j);
        }
    }
    this->opponent = new Player(colorOpponent);
}

TreeNode* gameTree::chooseOrb(int difficulty){
    this->insertNextRound(this->root, *this->player);
    TreeNode *firstLevelNode = this->root->getNextRoundNode();
    TreeNode *bestNode = this->root->getNextRoundNode();
    float bestScore = 0;
    float tempScore = 0;
    long numRoundNode = 0;
    
    for(firstLevelNode;firstLevelNode==NULL;firstLevelNode=firstLevelNode->getNextNode()){
        // Find out which one is the best to put the orb
        tempScore = this->BoardEvaluator(firstLevelNode, difficulty);
        // Second level    
        numRoundNode = insertNextRound(firstLevelNode, *this->opponent);
        TreeNode *secondLevelNode = firstLevelNode->getNextRoundNode();
        for(secondLevelNode;secondLevelNode==NULL;secondLevelNode=secondLevelNode->getNextNode()){
            tempScore += 0.8*this->BoardEvaluator(secondLevelNode, difficulty)/numRoundNode;
            numRoundNode = numRoundNode*insertNextRound(secondLevelNode, *this->player);
            TreeNode *thirdLevelNode = secondLevelNode->getNextRoundNode();
            
            for(thirdLevelNode;thirdLevelNode==NULL;thirdLevelNode=thirdLevelNode->getNextNode()){
                tempScore += 0.6*this->BoardEvaluator(thirdLevelNode, difficulty)/numRoundNode;
                numRoundNode = numRoundNode*insertNextRound(thirdLevelNode, *this->opponent);
                TreeNode *fourthLevelNode = thirdLevelNode->getNextRoundNode();
                for(fourthLevelNode;fourthLevelNode==NULL;fourthLevelNode=fourthLevelNode->getNextNode()){
                    tempScore += 0.4*this->BoardEvaluator(fourthLevelNode, difficulty)/numRoundNode;
                }
            }
            
        }
        if(tempScore>bestScore){
            bestScore = tempScore;
            bestNode = firstLevelNode;
        }
    }
    return bestNode;
}

int gameTree::insertNextRound(TreeNode *Node, Player currentTurn){
    // Insert all the cases of next round in gameTree
    // Input : currentTurn indicates which player is going to put the orb in this turn
    int numNewState=0;
    char color = currentTurn.get_color();
    int headX, headY;
    bool headFind=false;
    Board boardState = Node->getBoardState();
    
    // Find first next round node using random
    while(1){
        headX = rand()%5;
        headY = rand()%6;
        if(boardState.get_cell_color(headX,headY)==color || boardState.get_cell_color(headX,headY)=='w'){
            numNewState++;
            boardState.place_orb(headX,headY,&currentTurn);
            Board newBoardState = boardState;
            TreeNode *newNode = new TreeNode(newBoardState,headX,headY);
            Node->setNextRoundNode(newNode);
            break;
        }
    }

    for(int i=0;i<ROW;i++){
        for(int j=0;j<COL;j++){
            if(boardState.get_cell_color(i,j)==color || boardState.get_cell_color(i,j)=='w'){
                numNewState++;
                boardState.place_orb(i,j,&currentTurn);
                Board newBoardState = boardState;
                TreeNode *newNode = new TreeNode(newBoardState,i,j);
                TreeNode *tail = Node->getNextRoundNode()->getTail();
                tail->setNextNode(newNode);
                
            }
        }
    }  
    return numNewState;
}

float gameTree::BoardEvaluator(TreeNode *Node, int difficulty){
    // This is the simple board evaluator
    Board board = Node->getBoardState();
    char colorPlayer = player->get_color();
    char colorOpponent = opponent->get_color();
    float orbPlayer = 0, orbOpponent = 0;
    float orbScore = 0;
    if(difficulty == EASY_BOARD_EVALUATOR){
        for(int i=0;i<ROW;i++){
            for(int j=0;i<COL;j++){
                // 只考慮座標(i,j)的orb數量
                if(board.get_cell_color(i,j)==colorPlayer){
                    orbPlayer+=board.get_orbs_num(i,j);
                }else if(board.get_cell_color(i,j)==colorOpponent){
                    orbOpponent+=board.get_orbs_num(i,j);
                }
            }
        }
        orbScore = orbPlayer - orbOpponent;
    }else if(difficulty == HARD_BOARD_EVALUATOR){
        for(int i=0;i<ROW;i++){
            for(int j=0;j<COL;j++){
                // 考慮座標(i,j)的orb數量以及四個方向的對手orb數量
                if(board.get_cell_color(i,j)==colorPlayer){
                    orbPlayer+=board.get_orbs_num(i,j);
                }else if(board.get_cell_color(i,j)==colorOpponent){
                    orbOpponent+=board.get_orbs_num(i,j);
                }
                
                // 計算四個方向的opponent orb數量(越多代表這個cell越值得引爆)()
                if(index_range_illegal(i+EAST,j)==true){
                    if(board.get_cell_color(i+EAST,j)==colorOpponent && board.get_orbs_num(i+EAST,j)<board.get_orbs_num(i,j))
                        orbScore += 0.25*(board.get_orbs_num(i+EAST,j));
                }
                if(index_range_illegal(i-WEST,j)==true){
                    if(board.get_cell_color(i-WEST,j)==colorOpponent && board.get_orbs_num(i-WEST,j)<board.get_orbs_num(i,j))
                        orbScore += 0.25*(board.get_orbs_num(i-WEST,j));
                }
                if(index_range_illegal(i,j+NORTH)==true){
                    if(board.get_cell_color(i,j+NORTH)==colorOpponent && board.get_orbs_num(i,j+NORTH)<board.get_orbs_num(i,j))
                        orbScore += 0.25*(board.get_orbs_num(i,j+NORTH));
                }
                if(index_range_illegal(i,j-SOUTH)==true){
                    if(board.get_cell_color(i,j-SOUTH)==colorOpponent && board.get_orbs_num(i,j-SOUTH)<board.get_orbs_num(i,j))
                        orbScore += 0.25*(board.get_orbs_num(i-SOUTH,j));
                }
            }
        }
        orbScore += (orbPlayer-orbOpponent);
    }
    return orbScore;
}