#include <iostream>
#include <stdlib.h>
using namespace std;


//definicje

#define CAR_size 5
#define BAT_size 4
#define CRU_size 3
#define DES_size 2

#define alive_ship_part '+'
#define dead_ship_part 'x'
#define reef '#'


#define MAX_ships 10


//structs


struct PLAYER_DATA {
    int max_board_y;
    int min_board_y;
    int max_board_x;
    int min_board_x;
    int alive_ship_parts;
} player_A, player_B;


struct SHIP_DATA {
    int placed_y;
    int placed_x;
    char direction;
    int ship_number;
    bool ship_availability;
    char ship_state[5];
    int available_moves;
    bool engine_state;
    bool canon_state;
}ship_data[2][4][10];

//enumy
enum ship
{
    CAR,
    BAT,
    CRU,
    DES
};


enum player
{
    A,
    B
};

//funkcje

void place_reef(char** Board, int y, int x)
{
    Board[y][x] = '#';
}

bool are_all_ships_placed(int ships_quantity[4][2])
{
    int i, j;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (ships_quantity[j][i] != 0) return false;
        }
    }
    return true;
}

char** create_board(int Board_Y, int Board_X)
{
    int i, j;
    char** a = new char* [Board_Y];
    for (i = 0; i < Board_Y; i++) a[i] = new char[Board_X];
    for (i = 0; i < Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++) a[i][j] = ' ';
    }
    return a;
}

void end_game(char** Board, int Board_Y)
{
    for ( int i = 0; i < Board_Y; i++) delete[] Board[i];
    delete[] Board;
    exit(0);
}

bool does_position_exist(int Board_Y, int Board_X, int y, int x)
{
    if (y >= Board_Y) return false;
    if (x >= Board_X) return false;
    if (y < 0) return false;
    if (x < 0) return false;
    return true;
}

bool dirless_part_prox_validation(char** Board, int Board_Y, int Board_X, int y, int x)
{
    if (does_position_exist(Board_Y, Board_X, y + 1, x))
    {
        if (Board[y + 1][x] == alive_ship_part || Board[y + 1][x] == dead_ship_part) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y, x + 1))
    {
        if (Board[y][x + 1] == alive_ship_part || Board[y][x + 1] == dead_ship_part) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y, x - 1))
    {
        if (Board[y][x - 1] == alive_ship_part || Board[y][x - 1] == dead_ship_part) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y - 1, x))
    {
        if (Board[y - 1][x] == alive_ship_part || Board[y - 1][x] == dead_ship_part) return false;
    }
    return true;
}

bool ship_position_validation(int Board_Y, int Board_X, int y, int x, int active_player, int ship_length, char direction)
{
    if (active_player == A)
    {
        if (x<player_A.min_board_x || x>player_A.max_board_x) return false;
        if (y<player_A.min_board_y || y>player_A.max_board_y) return false;
        //dir validation
        switch (direction)
        {
        case 'N':
            if (y + 4 - ship_length > player_A.max_board_y) return false;
            break;
        case 'E':
            if (x - 4 + ship_length < player_A.min_board_x) return false;
            break;
        case 'S':
            if (y - 4 + ship_length < player_A.min_board_y) return false;
            break;
        case 'W':
            if (x + 4 - ship_length > player_A.max_board_x) return false;
            break;
        }
    }
    if (active_player == B)
    {
        if (x<player_B.min_board_x || x>player_B.max_board_x) return false;
        if (y<player_B.min_board_y || y>player_B.max_board_y) return false;
        //dir validation
        switch (direction)
        {
        case 'N':
            if (y + 4 - ship_length > player_B.max_board_y) return false;
            break;
        case 'E':
            if (x - 4 + ship_length < player_B.min_board_x) return false;
            break;
        case 'S':
            if (y - 4 + ship_length < player_B.min_board_y) return false;
            break;
        case 'W':
            if (x + 4 - ship_length > player_B.max_board_x) return false;
            break;
        }
    }
    return true;
}

int get_length_temp(char ship_class[3])
{
    int result=0;
    if (strcmp(ship_class, "CAR") == 0)
    {
        result = CAR;
    }
    if (strcmp(ship_class, "BAT") == 0)
    {
        result = BAT;
    }
    if (strcmp(ship_class, "CRU") == 0)
    {
        result = CRU;
    }
    if (strcmp(ship_class, "DES") == 0)
    {
        result = DES;
    }
    return result;
}


bool is_ship_on_board(int Board_Y, int Board_X, int y, int x)
{
    if (y < 0) return false;
    if (x < 0) return false;
    if (y >= Board_Y) return false;
    if (x >= Board_X) return false;
    return true;
}

bool reef_check(char** Board, int y, int x)
{
    if (Board[y][x] == reef) return true;
    return false;
}

void ship_placement(char** Board, int Board_Y, int Board_X, int y, int x, char direction, int ship_number, char ship_class[3], int player, int ship_quantity[4][2], char command[20], char ship_state[5], char direction_of_move)
{
    int i;
    int tempx = x;
    int tempy = y;
    int length_temp = 0;
    length_temp = get_length_temp(ship_class);
    if (strcmp(command, "MOVE") != 0)
    {
        if (!ship_position_validation(Board_Y, Board_X, y, x, player, length_temp, direction))
        {
            printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "NOT IN STARTING POSITION");
            end_game(Board, Board_Y);
        }
    }
    if (ship_data[player][length_temp][ship_number].ship_availability == false)
    {
        printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "SHIP ALREADY PRESENT");
        end_game(Board, Board_Y);
    }
    if (ship_quantity[length_temp][player] == 0)
    {
        printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "ALL SHIPS OF THE CLASS ALREADY SET");
        end_game(Board, Board_Y);
    }
    ship_quantity[length_temp][player]--;
    ship_data[player][length_temp][ship_number].ship_availability = false;
    ship_data[player][length_temp][ship_number].placed_y = tempy;
    ship_data[player][length_temp][ship_number].placed_x = tempx;
    ship_data[player][length_temp][ship_number].direction = direction;

    for (i = 0; i < 5; i++)
    {
        ship_data[player][length_temp][ship_number].ship_state[i] = ship_state[i];
    }
    


    bool no_near_ships = true;
    for (i = 0; i < 5 - length_temp; i++)
    {
        
        if (reef_check(Board,tempy,tempx))
        {
            if (strcmp(command,"SHIP")==0)
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s %s\": %s", command, y, x, direction, ship_number, ship_class, ship_state, "PLACING SHIP ON REEF");
                end_game(Board, Board_Y);
            }
            else
            {
                if (strcmp(command, "MOVE") == 0)
                {
                    printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "PLACING SHIP ON REEF");
                    end_game(Board, Board_Y);
                }
                else
                {
                    printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "PLACING SHIP ON REEF");
                    end_game(Board, Board_Y);
                }                
            }
        }
        
        if (!dirless_part_prox_validation(Board, Board_Y, Board_X,tempy,tempx))
        {
            no_near_ships = false;
            break;
        }
        switch (direction)
        {
        case 'N':
            tempy++;
            break;
        case 'E':
            tempx--;
            break;
        case 'S':
            tempy--;
            break;
        case 'W':
            tempx++;
            break;
        }
    }
    tempy = y;
    tempx = x;
    if (!no_near_ships)
    {
        char player_char='I';
        if (player == A) player_char = 'A';
        if (player == B) player_char = 'B';
        if (strcmp(command,"SHIP")==0)
        {
            printf("INVALID OPERATION \"%s %c %i %i %c %i %s %s\": %s", command, player_char, y, x, direction, ship_number, ship_class, ship_state, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
            end_game(Board, Board_Y);
        }
        else
        {
            if (strcmp(command, "MOVE") == 0)
            {
                printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
                end_game(Board, Board_Y);
            }
            else
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
                end_game(Board, Board_Y);
            }            
        }
    }
    int ship_state_check = 0;
    for (i = 0; i < 5 - length_temp; i++)
    {
        if (ship_state[ship_state_check] == '1')
        {
            if (strcmp(command, "MOVE") != 0)
            {
                if (player == A) player_A.alive_ship_parts++;
                if (player == B) player_B.alive_ship_parts++;
            }
            Board[tempy][tempx] = alive_ship_part;
        }
        else
        {
            Board[tempy][tempx] = dead_ship_part;
        }
        switch (direction)
        {
        case 'N':
            tempy++;
            break;
        case 'E':
            tempx--;
            break;
        case 'S':
            tempy--;
            break;
        case 'W':
            tempx++;
            break;
        }
        ship_state_check++;
    }
}


void delete_ship(char** Board, int ship_y, int ship_x, char direction, char ship_class[3])
{
    int i;
    int tempx = ship_x;
    int tempy = ship_y;
    int length_temp = 0;
    if (strcmp(ship_class, "CAR") == 0)
    {
        length_temp = CAR;
    }
    if (strcmp(ship_class, "BAT") == 0)
    {
        length_temp = BAT;
    }
    if (strcmp(ship_class, "CRU") == 0)
    {
        length_temp = CRU;
    }
    if (strcmp(ship_class, "DES") == 0)
    {
        length_temp = DES;
    }
    for (i = 0; i < 5 - length_temp; i++)
    {
        Board[tempy][tempx] = ' ';
        switch (direction)
        {
        case 'N':
            tempy++;
            break;
        case 'E':
            tempx--;
            break;
        case 'S':
            tempy--;
            break;
        case 'W':
            tempx++;
            break;
        }
    }
}

void move_ship(int player, int ship_number, char ship_class[3], char direction_of_move)
{
    int length_temp=get_length_temp(ship_class);
    ship_data[player][length_temp][ship_number].ship_availability = true;
    switch (direction_of_move)
    {
    case 'F':
        switch (ship_data[player][length_temp][ship_number].direction)
        {
        case 'N':
            ship_data[player][length_temp][ship_number].placed_y--;
            break;
        case 'E':
            ship_data[player][length_temp][ship_number].placed_x++;;
            break;
        case 'S':
            ship_data[player][length_temp][ship_number].placed_y++;
            break;
        case 'W':
            ship_data[player][length_temp][ship_number].placed_x--;
            break;
        }
        break;
    case 'L':
        switch (ship_data[player][length_temp][ship_number].direction)
        {
        case 'N':
            ship_data[player][length_temp][ship_number].direction = 'W';
            ship_data[player][length_temp][ship_number].placed_y--;
            ship_data[player][length_temp][ship_number].placed_x= ship_data[player][length_temp][ship_number].placed_x - 4 + length_temp;
            break;
        case 'E':
            ship_data[player][length_temp][ship_number].direction = 'N';
            ship_data[player][length_temp][ship_number].placed_x++;
            ship_data[player][length_temp][ship_number].placed_y= ship_data[player][length_temp][ship_number].placed_y - 4 + length_temp;
            break;
        case 'S':
            ship_data[player][length_temp][ship_number].direction = 'E';
            ship_data[player][length_temp][ship_number].placed_y++;
            ship_data[player][length_temp][ship_number].placed_x= ship_data[player][length_temp][ship_number].placed_x + 4 - length_temp;
            break;
        case 'W':
            ship_data[player][length_temp][ship_number].direction = 'S';
            ship_data[player][length_temp][ship_number].placed_x--;
            ship_data[player][length_temp][ship_number].placed_y= ship_data[player][length_temp][ship_number].placed_y + 4 - length_temp;

            break;
        }
        break;
    case 'R':
        switch (ship_data[player][length_temp][ship_number].direction)
        {
        case 'N':
            ship_data[player][length_temp][ship_number].direction = 'E';
            ship_data[player][length_temp][ship_number].placed_y--;
            ship_data[player][length_temp][ship_number].placed_x= ship_data[player][length_temp][ship_number].placed_x +4-length_temp;
            break;
        case 'E':
            ship_data[player][length_temp][ship_number].direction = 'S';
            ship_data[player][length_temp][ship_number].placed_x++;
            ship_data[player][length_temp][ship_number].placed_y= ship_data[player][length_temp][ship_number].placed_y + 4 - length_temp;
            break;
        case 'S':
            ship_data[player][length_temp][ship_number].direction = 'W';
            ship_data[player][length_temp][ship_number].placed_y++;
            ship_data[player][length_temp][ship_number].placed_x= ship_data[player][length_temp][ship_number].placed_x -4+length_temp;
            break;
        case 'W':
            ship_data[player][length_temp][ship_number].direction = 'N';
            ship_data[player][length_temp][ship_number].placed_x--;
            ship_data[player][length_temp][ship_number].placed_y= ship_data[player][length_temp][ship_number].placed_y-4+length_temp;
            break;
        }
        break;
    }
    ship_data[player][length_temp][ship_number].available_moves--;
}

void shootout(char** Board, int Board_Y, int Board_X, int aimed_y, int aimed_x, int ships_quantity[4][2], int aimed_player, char command[20])
{
    if (aimed_x > Board_X || aimed_x<0 || aimed_y<0 || aimed_y > Board_Y)
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "FIELD DOES NOT EXIST"); 
        end_game(Board, Board_Y);
    }
    if (are_all_ships_placed(ships_quantity))
    {
        if (Board[aimed_y][aimed_x] == alive_ship_part)
        {
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (aimed_player == A) player_A.alive_ship_parts--;
            if (aimed_player == B) player_B.alive_ship_parts--;            
        }
    }
    else
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "NOT ALL SHIPS PLACED");
        end_game(Board, Board_Y);
    }

}

void basic_print(char** Board, int Board_Y, int Board_X)
{
    int i, j;
    for (i = 0; i <Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            cout << Board[i][j];
        }
        cout << endl;
    }
    cout << "PARTS REMAINING:: " << "A : " << player_A.alive_ship_parts << " B : " << player_B.alive_ship_parts << endl;
}

void ship_move_reset(int player)
{
    int i,j,k;
    for (k = 0; k < 2; k++)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 10; j++)
            {
                if (i != CAR) ship_data[k][i][j].available_moves = 3;
                else ship_data[k][i][j].available_moves = 2;
            }
        }
    }
}

int main()
{
    int i, j,Board_Y=21,Board_X=10;
    //default board
    char** Board = create_board(Board_Y, Board_X);
    int ships_quantity[4][2];
    bool available_ship_index[2][4][10];
    int active_player = A;
    bool can_win = false;

    //default player rectangles
    player_A.min_board_y = 0;
    player_A.min_board_x = 0;
    player_A.max_board_y = Board_Y/2-1;
    player_A.max_board_x = Board_X-1;
    player_B.min_board_y = Board_Y/2+1;
    player_B.min_board_x = 0;
    player_B.max_board_y = Board_Y-1;
    player_B.max_board_x = Board_X-1;


    //zeroing ship parts
    player_A.alive_ship_parts = 0;
    player_B.alive_ship_parts = 0;

    //zerowanie tablic planszy i indeksow
    for (i = 0; i < Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            Board[i][j] = ' ';
        }
    }
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 4; j++)
        {
            for (int n = 0; n < 10; n++)
            {
                available_ship_index[i][j][n] = false;
            }
        }
    }
    //default fleet
    
    for (i = 0; i < 2; i++)
    {
        ships_quantity[CAR][i] = 1;
        ships_quantity[BAT][i] = 2;
        ships_quantity[CRU][i] = 3;
        ships_quantity[DES][i] = 4;
    }
    //default indexing and available ship moves
    int k;
    for (k = 0; k < 2; k++)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 10; j++)
            {
                ship_data[k][i][j].ship_availability = true;
            }
        }
    }

    ship_move_reset(A);
    ship_move_reset(B);
    
    char command_group[20];

    while (cin >> command_group)
    {
        if (strcmp(command_group, "[state]")==0)
        {
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "SHIP") == 0)
                {
                    char direction_of_move = 'X';
                    char player;
                    cin >> player;
                    int player_number=A;
                    if (player == 'A') player_number = A;
                    if (player == 'B') player_number = B;
                    int y, x, ship_number;
                    char ship_class[256],direction,ship_state[5];
                    cin >> y >> x >> direction >> ship_number >> ship_class >> ship_state;
                    ship_placement(Board, Board_Y, Board_X, y, x, direction, ship_number, ship_class, player_number, ships_quantity, command,ship_state,direction_of_move);
                }
                if (strcmp(command, "REEF") == 0)
                {
                    int y,x;
                    cin >> y >> x;
                    if (x > Board_X || y > Board_Y || x < 0 || y < 0)
                    {
                        printf("INVALID OPERATION \"%s %i %i\": %s", command,y,x, "REEF IS NOT PLACED ON BOARD");
                        end_game(Board, Board_Y);
                    }
                    else
                    {
                        place_reef(Board, y, x);
                    }
                }
                if (strcmp(command, "BOARD_SIZE") == 0)
                {
                    for (i = 0; i < Board_Y; i++) delete[] Board[i];
                    delete[] Board;
                    cin >> Board_Y >> Board_X;          
                    Board = create_board(Board_Y, Board_X);
                }
                if (strcmp(command, "INIT_POSITION") == 0)
                {
                    char player;
                    cin >> player;
                    if (player == 'A')
                    {
                        cin >> player_A.min_board_y >> player_A.min_board_x >> player_A.max_board_y >> player_A.max_board_x;
                    }
                    else
                    {
                        cin >> player_B.min_board_y >> player_B.min_board_x >> player_B.max_board_y >> player_B.max_board_x;
                    }
                }
                if (strcmp(command, "SET_FLEET")==0)
                {
                    char X;
                    cin >> X;
                    if (X == 'A')
                    {
                        cin >> ships_quantity[CAR][A] >> ships_quantity[BAT][A] >> ships_quantity[CRU][A] >> ships_quantity[DES][A];
                    }
                    else
                    {
                        cin >> ships_quantity[CAR][B] >> ships_quantity[BAT][B] >> ships_quantity[CRU][B] >> ships_quantity[DES][B];
                    }
                }
                if (strcmp(command, "PRINT")==0)
                {
                    int control_number;
                    cin >> control_number;
                    if (control_number == 0)
                    {
                        basic_print(Board, Board_Y,Board_X);
                    }
                }
                if (strcmp(command, "NEXT_PLAYER")==0)
                {
                    char player;
                    cin >> player;
                    if (player == 'A') active_player = A;
                    if (player == 'B') active_player = B;

                }
                if (strcmp(command,"[state]") == 0)
                {
                    break;
                }
            }
        }
        if (strcmp(command_group, "[playerA]")==0)
        {      
            if (active_player == B)
            {
                printf("INVALID OPERATION \"%s \": %s", command_group, "THE OTHER PLAYER EXPECTED");
                end_game(Board, Board_Y);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "PLACE_SHIP")==0)
                {
                    char direction_of_move = 'X';
                    int x, y, ship_number;
                    char ship_class[256], direction,ship_state[5];
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    for (i = 0; i < 5; i++) ship_state[i] = '1';                    
                    ship_placement(Board,Board_Y,Board_X, y, x, direction, ship_number, ship_class , active_player, ships_quantity,command, ship_state,direction_of_move);
                }
                if (strcmp(command, "SHOOT")==0)
                {
                    can_win = true;
                    int x, y;
                    cin >> y >> x;
                    shootout(Board,Board_Y,Board_Y, y, x, ships_quantity, B, command);
                }
                if (strcmp(command, "MOVE") == 0)
                {                    
                    int ship_number;
                    char ship_class[3], direction_of_move;
                    cin >> ship_number >> ship_class >> direction_of_move;
                    int length_temp = get_length_temp(ship_class);
                    if (ship_data[active_player][length_temp][ship_number].available_moves == 0)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class,direction_of_move, "SHIP MOVED ALREADY");
                        end_game(Board, Board_Y);
                    }
                    if (!is_ship_on_board(Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x))
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP WENT FROM BOARD");
                        end_game(Board, Board_Y);
                    }
                    delete_ship(Board, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_class);
                    ships_quantity[length_temp][active_player]++;
                    move_ship(active_player, ship_number, ship_class, direction_of_move);
                    ship_placement(Board, Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y,
                        ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_number, ship_class, active_player, ships_quantity,
                        command, ship_data[active_player][length_temp][ship_number].ship_state,direction_of_move);                                                                
                }
                if (strcmp(command,"[playerA]") == 0)
                {
                    ship_move_reset(A);
                    if (player_B.alive_ship_parts == 0 && can_win)
                    {
                        cout << "A won" << endl;
                        end_game(Board, Board_Y);
                    }
                    active_player = B;
                    break;
                }
            }
        }
        if (strcmp(command_group, "[playerB]")==0)
        {
            if (active_player == A)
            {
                printf("INVALID OPERATION \"%s \": %s", command_group, "THE OTHER PLAYER EXPECTED");
                end_game(Board, Board_Y);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "PLACE_SHIP") == 0)
                {
                    char direction_of_move = 'X';
                    int x, y, ship_number;
                    char ship_class[256], direction, ship_state[5];
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    for (i = 0; i < 5; i++) ship_state[i] = '1';
                    ship_placement(Board, Board_Y, Board_X, y, x, direction, ship_number, ship_class, active_player, ships_quantity, command, ship_state,direction_of_move);
                }
                if (strcmp(command, "SHOOT") == 0)
                {
                    can_win = true;
                    int x, y;
                    cin >> y >> x;
                    shootout(Board,Board_Y,Board_X, y, x, ships_quantity, A, command);
                }
                if (strcmp(command, "MOVE") == 0)
                {
                    int ship_number;
                    char ship_class[3], direction_of_move;
                    cin >> ship_number >> ship_class >> direction_of_move;
                    int length_temp = get_length_temp(ship_class);
                    if (ship_data[active_player][length_temp][ship_number].available_moves == 0)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP MOVED ALREADY");
                        end_game(Board, Board_Y);
                    }
                    delete_ship(Board, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_class);
                    ships_quantity[length_temp][active_player]++;
                    move_ship(active_player, ship_number, ship_class, direction_of_move);
                    if (!is_ship_on_board(Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x))
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP WENT FROM BOARD");
                        end_game(Board, Board_Y);
                    }
                    ship_placement(Board, Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y,
                        ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_number, ship_class, active_player, ships_quantity,
                        command, ship_data[active_player][length_temp][ship_number].ship_state, direction_of_move);
                }
                if (strcmp(command, "[playerB]") == 0)
                {
                    ship_move_reset(B);
                    if (player_A.alive_ship_parts == 0 && can_win)
                    {
                        cout << "B won" << endl;
                        end_game(Board, Board_Y);
                    }
                    active_player = A;
                    break;
                }
            }
        }
    }
    return 0;
}