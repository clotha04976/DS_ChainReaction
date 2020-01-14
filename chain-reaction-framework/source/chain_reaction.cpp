#include <iostream>
#include "../include/board.h"
#include "../include/player.h"
#include "../include/rules.h"
#include "../include/algorithm.h"

using namespace std;

int play(bool change = false){ // return 2/-2: illegal(red/blue), 1/-1 win/lose(red/blue)
    Board board;
    Player red_player(RED);
    Player blue_player(BLUE);

    bool first_two_step = true;
    int round = 1;
    int index[2];
    if(change)
        goto changeMove;
    while(1){
        //////////// Red Player operations ////////////
        algorithm_A(board, red_player, index);
        board.place_orb(index[0], index[1], &red_player);

        if(rules_violation(red_player)) return 2;

        board.print_current_board(index[0], index[1], round);
        round++;
        
        if(board.win_the_game(red_player) && !first_two_step){
            std::cout << "Red Player won the game !!!" << endl;
            return 1;
        }
        // system("pause");
        //////////// Blue Player operations ////////////
    changeMove:
        algorithm_E(board, blue_player, index);
        board.place_orb(index[0], index[1], &blue_player);

        if(rules_violation(blue_player)) return -2;
        
        board.print_current_board(index[0], index[1], round);
        round++;

        if(board.win_the_game(blue_player) && !first_two_step){
            std::cout << "Blue Player won the game !!!" << endl;
            return -1;
        }
        // system("pause");
        first_two_step = false;
    }

    return 0;
}
int main(){
    int round = 10;
    int red_win =0 , blue_win =0 , red_illegal =0 , blue_illegal =0;
    int playResult;
    for (int i = 0 ; i < round ; i++){
        bool change = false;
        if (i >= round /2)
            change =true;
        playResult = play(change);
        switch (playResult)
        {
        case 2:
            red_illegal++;
            break;
        case 1:
            red_win++;
            break;
        case -1:
            blue_win++;
            break;
        case -2:
            blue_illegal++;
            break;
        default:
            break;
        }
    }
    std::cout << "Red win: " << red_win << "/ " <<round <<endl;
    std::cout << "Red illegal: " << red_illegal << "/ " <<round <<endl;
    std::cout << "Blue win: " << blue_win << "/ " <<round <<endl;
    std::cout << "Blue illegal: " << blue_illegal << "/ " <<round <<endl;
    return 0;
} 