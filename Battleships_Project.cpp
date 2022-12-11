#include <iostream>
#include <stdlib.h>
#include <cmath>
using namespace std;

#define alive_ship_part '+'
#define dead_ship_part 'x'
#define reef '#'
#define canon '!'
#define engine '%'
#define radar '@'

#define visible '*'
#define invisible '?'



struct PLAYER_DATA {
    int max_board_y;
    int min_board_y;
    int max_board_x;
    int min_board_x;
    int alive_ship_parts;
    int beginning_fleet[4];
} player_A, player_B;


struct SHIP_DATA {
    bool is_on_board;
    int placed_y;
    int placed_x;
    char direction;
    int ship_number;
    bool ship_availability;
    char ship_state[5];
    int available_moves;
    bool engine_state;
    bool canon_state;
    bool radar_state;
    int canon_range;
    int available_shots;
    int canon_y;
    int canon_x;
    int engine_y;
    int engine_x;
    int radar_y;
    int radar_x;
}ship_data[2][4][10];

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

enum spy_type
{
    RADAR,
    SPY_PLANE
};

double get_range(int y1, int x1, int y2, int x2)
{
    double result;
    result = sqrt(pow(abs(y1 - y2), 2) + pow(abs(x1 - x2), 2));
    return result;
}

void delete_board(char** Board,int Board_Y)
{
    if (Board != NULL)
    {
        for (int i = 0; i < Board_Y; i++) delete[] Board[i];
        delete[] Board;
        Board = NULL;
    }    
}

void place_reef(char** Board,char** playerA_vision, char** playerB_vision, int y, int x)
{
    Board[y][x] = '#';
    playerA_vision[y][x] = visible;
    playerB_vision[y][x] = visible;
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

void make_board_invisible(char** Board, int Board_Y, int Board_X)
{
    int i, j;
    for (i = 0; i < Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            Board[i][j] = invisible;
        }
    }
}

void end_game(char** Board,char** playerA_vision,char** playerB_vision,int Board_Y)
{
    delete_board(Board, Board_Y);
    delete_board(playerA_vision, Board_Y);
    delete_board(playerB_vision, Board_Y);
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


void make_area_visible(char** player_visibility, int Board_Y, int Board_X, int y, int x,int length_temp, bool is_radar_operational, int type)
{
    int i, j;
    int radar_range;
    if (type == SPY_PLANE)
    {
        radar_range = 1;
        for (i = y - radar_range; i <= y + radar_range; i++)
        {
            for (j = x - radar_range; j <= x + radar_range; j++)
            {
                if (does_position_exist(Board_Y, Board_X, i, j))
                {                    
                    player_visibility[i][j] = visible;
                }
            }
        }
    }
    else
    {
        if (is_radar_operational) radar_range = 5 - length_temp;
        else radar_range = 1;

        for (i = y - radar_range; i <= y + radar_range; i++)
        {
            for (j = x - radar_range; j <= x + radar_range; j++)
            {
                if (does_position_exist(Board_Y, Board_X, i, j))
                {
                    double distance = get_range(y, x, i, j);
                    if (distance <= radar_range)  player_visibility[i][j] = visible;
                }
            }
        }
    }
}

bool dirless_part_prox_validation(char** Board, int Board_Y, int Board_X, int y, int x)
{
    if (does_position_exist(Board_Y, Board_X, y + 1, x))
    {
        if (Board[y + 1][x] == alive_ship_part || Board[y + 1][x] == dead_ship_part || Board[y + 1][x] == canon || Board[y + 1][x] == radar || Board[y + 1][x] == engine) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y, x + 1))
    {
        if (Board[y][x + 1] == alive_ship_part || Board[y][x + 1] == dead_ship_part || Board[y][x + 1] == canon || Board[y][x + 1] == radar || Board[y][x + 1] == engine) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y, x - 1))
    {
        if (Board[y][x - 1] == alive_ship_part || Board[y][x - 1] == dead_ship_part || Board[y][x - 1] == canon || Board[y][x - 1] == radar || Board[y][x - 1] == engine) return false;
    }
    if (does_position_exist(Board_Y, Board_X, y - 1, x))
    {
        if (Board[y - 1][x] == alive_ship_part || Board[y - 1][x] == dead_ship_part || Board[y - 1][x] == canon || Board[y - 1][x] == radar || Board[y - 1][x] == engine) return false;
    }
    return true;
}

bool ship_position_validation(int y, int x, int active_player, int ship_length, char direction)
{
    if (active_player == A)
    {
        if (x<player_A.min_board_x || x>player_A.max_board_x) return false;
        if (y<player_A.min_board_y || y>player_A.max_board_y) return false;        
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


void ship_placement(char** Board, int Board_Y, int Board_X, int y, int x, char direction, int ship_number, char ship_class[3], int player, int ship_quantity[4][2], char command[20], char ship_state[5], char direction_of_move,char** playerA_vision, char** playerB_vision)
{
    int i;
    int tempx = x;
    int tempy = y;
    int length_temp = 0;
    length_temp = get_length_temp(ship_class);
    char player_char = 'I';
    if (player == A) player_char = 'A';
    if (player == B) player_char = 'B';
    if (strcmp(command, "MOVE") != 0)
    {
        if (!ship_position_validation(y, x, player, length_temp, direction) && strcmp(command,"SHIP")!=0)
        {
            printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "NOT IN STARTING POSITION");
            end_game(Board, playerA_vision, playerB_vision, Board_Y);         
        }
        if (ship_data[player][length_temp][ship_number].ship_availability == false)
        {
            if (strcmp(command, "SHIP") == 0)
            {
                printf("INVALID OPERATION \"%s %c %i %i %c %i %s %s\": %s", command, player_char, y, x, direction, ship_number, ship_class, ship_state, "SHIP ALREADY PRESENT");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            if (strcmp(command, "PLACE_SHIP") == 0)
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "SHIP ALREADY PRESENT");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }            
        }

        if (ship_quantity[length_temp][player] == 0)
        {
            if (strcmp(command, "SHIP") == 0)
            {
                printf("INVALID OPERATION \"%s %c %i %i %c %i %s %s\": %s", command, player_char, y, x, direction, ship_number, ship_class, ship_state, "ALL SHIPS OF THE CLASS ALREADY SET");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            if (strcmp(command, "PLACE_SHIP") == 0)
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "ALL SHIPS OF THE CLASS ALREADY SET");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            
        }
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
            if (strcmp(command, "SHIP") == 0)
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s %s\": %s", command, y, x, direction, ship_number, ship_class, ship_state, "PLACING SHIP ON REEF");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            if (strcmp(command, "MOVE") == 0)
            {
                printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "PLACING SHIP ON REEF");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            if(strcmp(command,"PLACE_SHIP")==0)
            {
                printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "PLACING SHIP ON REEF");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
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
        if (strcmp(command,"SHIP")==0)
        {
            printf("INVALID OPERATION \"%s %c %i %i %c %i %s %s\": %s", command, player_char, y, x, direction, ship_number, ship_class, ship_state, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
            end_game(Board, playerA_vision, playerB_vision, Board_Y);
        }
        if (strcmp(command, "MOVE") == 0)
        {
            printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
            end_game(Board, playerA_vision, playerB_vision, Board_Y);
        }
        if (strcmp(command, "PLACE_SHIP") == 0)
        {
            printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "PLACING SHIP TOO CLOSE TO OTHER SHIP");
            end_game(Board, playerA_vision, playerB_vision, Board_Y);
        }            
    }
    ship_data[player][length_temp][ship_number].is_on_board = true;
    int ship_state_check = 0;
    for (i = 0; i < 5 - length_temp; i++)
    {
        if (player == A) playerA_vision[tempy][tempx] = visible;
        if (player == B) playerB_vision[tempy][tempx] = visible;
        if (ship_state[ship_state_check] == '1')
        {
            if (strcmp(command, "MOVE") != 0)
            {
                if (player == A) player_A.alive_ship_parts++;
                if (player == B) player_B.alive_ship_parts++;
            }
            Board[tempy][tempx] = alive_ship_part;
            if (i == 0)
            {
                Board[tempy][tempx] = radar;
                ship_data[player][length_temp][ship_number].radar_state = true;
                ship_data[player][length_temp][ship_number].radar_y = tempy;
                ship_data[player][length_temp][ship_number].radar_x = tempx;
                if (player == A)
                {
                    make_area_visible(playerA_vision, Board_Y, Board_X,tempy,tempx, length_temp, true, RADAR);
                }
                else
                {
                    make_area_visible(playerB_vision, Board_Y, Board_X,tempy,tempx, length_temp, true ,RADAR);
                }
            } 
            if (i == 1)
            {
                Board[tempy][tempx] = canon;
                ship_data[player][length_temp][ship_number].canon_state = true;
                ship_data[player][length_temp][ship_number].canon_y = tempy;
                ship_data[player][length_temp][ship_number].canon_x = tempx;
            }            
            if (i == 5 - length_temp - 1)
            {
                Board[tempy][tempx] = engine;
                ship_data[player][length_temp][ship_number].engine_state = true;
                ship_data[player][length_temp][ship_number].engine_y = tempy;
                ship_data[player][length_temp][ship_number].engine_x = tempx;                
            }            
            if (i == 5 - length_temp - 1 && i == 1)
            {
                Board[tempy][tempx] = engine;
                ship_data[player][length_temp][ship_number].canon_state = true;
                ship_data[player][length_temp][ship_number].engine_state = true;
                ship_data[player][length_temp][ship_number].canon_y = tempy;
                ship_data[player][length_temp][ship_number].canon_x = tempx;
                ship_data[player][length_temp][ship_number].engine_y = tempy;
                ship_data[player][length_temp][ship_number].engine_x = tempx;
            }            
        }
        else
        {
            Board[tempy][tempx] = dead_ship_part;
            if (i == 0)
            {
                if (player == A)
                {
                    make_area_visible(playerA_vision, Board_Y, Board_X,tempy,tempx,length_temp, false,RADAR);
                }
                else
                {
                    make_area_visible(playerB_vision, Board_Y, Board_X,tempy,tempx,length_temp, false,RADAR);
                }
            }            
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


void delete_ship(char** Board, int ship_y, int ship_x, char direction, int length_temp)
{
    int i;
    int tempx = ship_x;
    int tempy = ship_y;
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



void extended_shootout(char** Board, int Board_Y, int Board_X, int aimed_y, int aimed_x, int ship_number, int player, char command[20], char ship_class[3], char** playerA_vision, char** playerB_vision, int ships_quantity[4][2])
{
    int i, j, opposite_player=0;
    if (player == A)opposite_player = B;
    if (player == B)opposite_player = A;

    int length_temp = get_length_temp(ship_class);
    if (!are_all_ships_placed(ships_quantity))
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "NOT ALL SHIPS PLACED");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    if (aimed_x > Board_X || aimed_x<0 || aimed_y<0 || aimed_y > Board_Y)
    {
        printf("INVALID OPERATION \"%s %i %s %i %i\": %s", command, ship_number, ship_class, aimed_y, aimed_x, "FIELD DOES NOT EXIST");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    if (!ship_data[player][length_temp][ship_number].canon_state)
    {
        printf("INVALID OPERATION \"%s %i %s %i %i\": %s", command, ship_number, ship_class, aimed_y, aimed_x, "SHIP CANNOT SHOOT");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    if (ship_data[player][length_temp][ship_number].available_shots == 0)
    {
        printf("INVALID OPERATION \"%s %i %s %i %i\": %s", command, ship_number, ship_class, aimed_y, aimed_x, "TOO MANY SHOOTS");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    int ship_range;
    double range;
    if (strcmp(ship_class, "CAR") == 0)
    {
        ship_range = INT_MAX;
    }
    else
    {
        ship_range = 5 - length_temp;
    }
        range = get_range(ship_data[player][length_temp][ship_number].canon_y, ship_data[player][length_temp][ship_number].canon_x, aimed_y, aimed_x);  
    if (range > ship_range)
    {
        printf("INVALID OPERATION \"%s %i %s %i %i\": %s", command, ship_number, ship_class, aimed_y, aimed_x, "SHOOTING TOO FAR");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    ship_data[player][length_temp][ship_number].available_shots--;
    switch (Board[aimed_y][aimed_x])
    {
        case '!':
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 10; j++)
                {
                    if (ship_data[opposite_player][i][j].canon_y == aimed_y && ship_data[opposite_player][i][j].canon_x == aimed_x)
                    {
                        ship_data[opposite_player][i][j].canon_state = false;                
                        break;
                    }
                }
            }
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (opposite_player == A) player_A.alive_ship_parts--;
            if (opposite_player == B) player_B.alive_ship_parts--;
            break;
        case '%':
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 10; j++)
                {
                    if (ship_data[opposite_player][i][j].engine_y == aimed_y && ship_data[opposite_player][i][j].engine_x == aimed_x)
                    {
                        ship_data[opposite_player][i][j].engine_state = false;                    
                        break;
                    }
                }            
            }
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (opposite_player == A) player_A.alive_ship_parts--;
            if (opposite_player == B) player_B.alive_ship_parts--;
            break;
        case '@':
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j < 10; j++)
                {
                    if (ship_data[opposite_player][i][j].radar_y == aimed_y && ship_data[opposite_player][i][j].radar_x == aimed_x)
                    {
                        ship_data[opposite_player][i][j].radar_state = false;                    
                        break;
                    }
                }            
            }
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (opposite_player == A) player_A.alive_ship_parts--;
            if (opposite_player == B) player_B.alive_ship_parts--;
            break;
        case '+':
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (opposite_player == A) player_A.alive_ship_parts--;
            if (opposite_player == B) player_B.alive_ship_parts--;
            break;
        case ' ':
            break;
        case 'x':
            break;
        case '#':
            break;
    }
}

void reset_shots(int player)
{
    int i, j;
    int max_shots = 5;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if(ship_data[player][i][j].canon_state) ship_data[player][i][j].available_shots = max_shots;
        }
        max_shots--;
    }
}

void shootout(char** Board, int Board_Y, int Board_X, int aimed_y, int aimed_x, int ships_quantity[4][2], int aimed_player, char command[20], char** playerA_vision, char** playerB_vision)
{
    if (aimed_x > Board_X || aimed_x<0 || aimed_y<0 || aimed_y > Board_Y)
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "FIELD DOES NOT EXIST"); 
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }
    if (are_all_ships_placed(ships_quantity))
    {
        if (Board[aimed_y][aimed_x] != dead_ship_part && Board[aimed_y][aimed_x]!= reef && Board[aimed_y][aimed_x] != ' ')
        {
            Board[aimed_y][aimed_x] = dead_ship_part;
            if (aimed_player == A) player_A.alive_ship_parts--;
            if (aimed_player == B) player_B.alive_ship_parts--;            
        }
    }
    else
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "NOT ALL SHIPS PLACED");
        end_game(Board, playerA_vision, playerB_vision, Board_Y);
    }

}

void refresh_ships_after_shootout(char** Board, int shot_player)
{
    int i=shot_player, j, k, l;
    for (j = 0; j < 4; j++)
    {
        for (k = 0; k < 10; k++)
        {
            if (ship_data[i][j][k].is_on_board)
            {
                int tempy = ship_data[i][j][k].placed_y;
                int tempx = ship_data[i][j][k].placed_x;
                int control = 0;
                for (l = 0; l < 5 - j; l++)
                {
                    if (Board[tempy][tempx] == dead_ship_part) ship_data[i][j][k].ship_state[control] = '0';
                    else ship_data[i][j][k].ship_state[control] = '1';
                    control++;                    
                    switch (ship_data[i][j][k].direction)
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
        }
    }
}

void basic_print(char** Board, int Board_Y, int Board_X)
{
    int i, j;
    for (i = 0; i <Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            if (Board[i][j] == canon || Board[i][j] == engine || Board[i][j] == radar) cout << '+';
            else cout << Board[i][j];            
        }
        cout << endl;
    }
    cout << "PARTS REMAINING:: " << "A : " << player_A.alive_ship_parts << " B : " << player_B.alive_ship_parts << endl;
}

void print_ship_class(int length_temp)
{
    switch (length_temp)
    {
    case CAR:
        cout << "CAR";
        break;
    case BAT:
        cout << "BAT";
        break;
    case CRU:
        cout << "CRU";
        break;
    case DES:
        cout << "DES";
        break;
    }
}

int get_int_length(int number)
{
    int digits = 0;
    int i;
    for (i = 1; i < 10; i++)
    {
        if (number == pow(10, i))
        {
            digits = -1;
            break;
        }
    }
    while (number != 0) {        
        number /= 10;
        digits++;
    }
    return digits;
}

void print_certain_digit(int x, int digit_index, int number_length)
{
    int digit_temp = 1;
    int power = number_length-1;
    int temp = int(pow(10, power));    
    do {
        int digit = x / temp;
        if (digit_temp == digit_index) cout << digit;
        x = x % temp;
        temp /= 10;        
        digit_temp++;
    } while (temp != 0);
}

void extended_print(char** Board, int Board_Y, int Board_X)
{
    int i, j,k=0,m=0;
    int y_length=get_int_length(Board_Y), x_length=get_int_length(Board_X);    
    int o = 1, l = 1;
    for (i = 0; i < Board_Y + x_length; i++)
    {
        for (j = 0; j < Board_X + y_length; j++)
        {
            if (i < x_length && j < y_length) cout << ' ';
            if (i<x_length && j>y_length - 1)
            {
                print_certain_digit(j-y_length,o,x_length);
            }
            if (i>x_length-1 && j<y_length)
            {
                print_certain_digit(i-x_length, l, y_length);
                l++;
            }
            if (j > y_length-1 && i > x_length-1)
            {
                cout << Board[k][m];
                m++;
            }
        }
        o++;
        l = 1;
        if (i > x_length-1)
        {
            k++;
            m = 0;
        }
        cout << endl;
    }
    cout << "PARTS REMAINING:: " << "A : " << player_A.alive_ship_parts << " B : " << player_B.alive_ship_parts << endl;
}

void basic_player_print(char** player_vision,char** Board, int Board_Y, int Board_X)
{
    int i, j;
     for (i = 0; i < Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            if (player_vision[i][j] == visible)
            {
                if (Board[i][j] == canon || Board[i][j] == engine || Board[i][j] == radar) cout << '+';
                else cout << Board[i][j];
            }
            else cout << invisible;
        }
        cout << endl;
    }
}

void ship_move_reset(int player)
{
    int i,j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (i != CAR) ship_data[player][i][j].available_moves = 3;
            else ship_data[player][i][j].available_moves = 2;
        }
    }
}

void print_ship_state(int length_temp, int player, int ship_number)
{
    int i;
    for (i = 0; i < 5 - length_temp; i++)
    {
        cout << ship_data[player][length_temp][ship_number].ship_state[i];
    }
}

void print_save(char** Board,int Board_Y, int Board_X, int active_player, bool if_extended)
{
    int i,j;
    char player;
    if (active_player == A) player = 'A';
    else player = 'B';
    cout << "[state]" << endl;
    cout << "BOARD_SIZE " << Board_Y << ' ' << Board_X << endl;
    cout << "NEXT_PLAYER " << player << endl;
    cout << "INIT_POSITION " << 'A' << ' ' << player_A.min_board_y << ' ' << player_A.min_board_x << ' ' << player_A.max_board_y << ' ' << player_A.max_board_x << endl;
    cout << "SET_FLEET " << 'A'  << ' ' <<  player_A.beginning_fleet[CAR] << ' ' <<  player_A.beginning_fleet[BAT] << ' ' << player_A.beginning_fleet[CRU] << ' ' << player_A.beginning_fleet[DES] << endl;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (ship_data[A][i][j].is_on_board)
            {                
                cout << "SHIP " << 'A' << ' ' << ship_data[A][i][j].placed_y << ' ' << ship_data[A][i][j].placed_x << ' ' << ship_data[A][i][j].direction << ' ' << ship_data[A][i][j].ship_number << ' ';
                print_ship_class(i);
                cout << ' ';
                print_ship_state(i, A, j);
                cout << endl;
            }
        }
    }
    cout << "INIT_POSITION " << 'B' << ' ' << player_B.min_board_y << ' ' << player_B.min_board_x << ' ' << player_B.max_board_y << ' ' << player_B.max_board_x << endl;
    cout << "SET_FLEET " << 'B' << ' ' << player_B.beginning_fleet[CAR] << ' ' << player_B.beginning_fleet[BAT] << ' ' << player_B.beginning_fleet[CRU] << ' ' << player_B.beginning_fleet[DES] << endl;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 10; j++)
        {
            if (ship_data[B][i][j].is_on_board)
            {                
                cout << "SHIP "<< 'B' << ' ' << ship_data[B][i][j].placed_y << ' ' << ship_data[B][i][j].placed_x << ' ' << ship_data[B][i][j].direction << ' ' << ship_data[B][i][j].ship_number << ' ';
                print_ship_class(i);
                cout << ' ';
                print_ship_state(i, B, j);
                cout << endl;
            }
        }
    }
    for (i = 0; i < Board_Y; i++)
    {
        for (j = 0; j < Board_X; j++)
        {
            if (Board[i][j] == reef)
            {
                cout << "REEF " << i << ' ' << j << endl;
            }
        }
    }
    if (if_extended) cout << "EXTENDED_SHIPS" << endl;
    cout << "[state]" << endl;
}



int main()
{
    int i, j,Board_Y=21,Board_X=10;    
    char** Board = create_board(Board_Y, Board_X);
    char** playerA_vision = create_board(Board_Y, Board_X);    
    make_board_invisible(playerA_vision, Board_Y, Board_X);
    char** playerB_vision = create_board(Board_Y, Board_X);   
    make_board_invisible(playerB_vision, Board_Y, Board_X);
    int ships_quantity[4][2];
    bool available_ship_index[2][4][10];
    int active_player = A;
    bool can_win = false;
    bool extended_switch = false;
    
    player_A.min_board_y = 0;
    player_A.min_board_x = 0;
    player_A.max_board_y = Board_Y/2-1;
    player_A.max_board_x = Board_X-1;
    player_B.min_board_y = Board_Y/2+1;
    player_B.min_board_x = 0;
    player_B.max_board_y = Board_Y-1;
    player_B.max_board_x = Board_X-1;


    
    player_A.alive_ship_parts = 0;
    player_B.alive_ship_parts = 0;

    
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
    
    
    for (i = 0; i < 2; i++)
    {
        ships_quantity[CAR][i] = 1;
        ships_quantity[BAT][i] = 2;
        ships_quantity[CRU][i] = 3;
        ships_quantity[DES][i] = 4;
    }

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
                    ship_placement(Board, Board_Y, Board_X, y, x, direction, ship_number, ship_class, player_number, ships_quantity, command,ship_state,direction_of_move,playerA_vision,playerB_vision);
                }
                if (strcmp(command, "EXTENDED_SHIPS") == 0)
                {
                    extended_switch = true;
                }
                if (strcmp(command, "SAVE") == 0)
                {
                    print_save(Board, Board_Y, Board_X, active_player, extended_switch);
                }
                if (strcmp(command, "REEF") == 0)
                {
                    int y,x;
                    cin >> y >> x;
                    if (x > Board_X || y > Board_Y || x < 0 || y < 0)
                    {
                        printf("INVALID OPERATION \"%s %i %i\": %s", command,y,x, "REEF IS NOT PLACED ON BOARD");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    else
                    {
                        place_reef(Board,playerA_vision,playerB_vision, y, x);
                    }
                }
                if (strcmp(command, "BOARD_SIZE") == 0)
                {
                    delete_board(Board, Board_Y);
                    delete_board(playerA_vision, Board_Y);
                    delete_board(playerB_vision, Board_Y);
                    cin >> Board_Y >> Board_X;                    
                    Board = create_board(Board_Y, Board_X);
                    playerA_vision = create_board(Board_Y, Board_X);
                    playerB_vision = create_board(Board_Y, Board_X);
                    make_board_invisible(playerA_vision, Board_Y, Board_X);
                    make_board_invisible(playerB_vision, Board_Y, Board_X);
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
                        for (i = 0; i < 4; i++)
                        {
                            player_A.beginning_fleet[i] = ships_quantity[i][A];
                        }
                    }
                    else
                    {
                        cin >> ships_quantity[CAR][B] >> ships_quantity[BAT][B] >> ships_quantity[CRU][B] >> ships_quantity[DES][B];
                        for (i = 0; i < 4; i++)
                        {
                            player_B.beginning_fleet[i] = ships_quantity[i][B];
                        }
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
                    if (control_number == 1)
                    {
                        extended_print(Board, Board_Y, Board_X);
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
            reset_shots(A);
            if (active_player == B)
            {
                printf("INVALID OPERATION \"%s \": %s", command_group, "THE OTHER PLAYER EXPECTED");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "SPY")==0)
                {
                    int y, x,carrier_number;
                    cin >> carrier_number >> y >> x;
                    if (!ship_data[active_player][CAR][carrier_number].canon_state)
                    {
                        printf("INVALID OPERATION \"%s %i %i %i\": %s", command, carrier_number, y, x, "CANNOT SEND PLANE");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    if (ship_data[active_player][CAR][carrier_number].available_shots==0)
                    {
                        printf("INVALID OPERATION \"%s %i %i %i\": %s", command, carrier_number, y, x, "ALL PLANES SENT");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    make_area_visible(playerA_vision, Board_Y, Board_X, y, x, CAR, false, SPY_PLANE);
                    ship_data[active_player][CAR][carrier_number].available_shots--;
                }
                if (strcmp(command, "PRINT") == 0)
                {
                    int control_number;
                    cin >> control_number;
                    if (control_number == 0)
                    {
                        basic_player_print(playerA_vision,Board, Board_Y, Board_X);
                    }
                }
                if (strcmp(command, "PLACE_SHIP")==0)
                {
                    char direction_of_move = 'X';
                    int x, y, ship_number;
                    char ship_class[256], direction,ship_state[5];
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    for (i = 0; i < 5; i++) ship_state[i] = '1';                    
                    ship_placement(Board,Board_Y,Board_X, y, x, direction, ship_number, ship_class , active_player, ships_quantity,command, ship_state,direction_of_move,playerA_vision,playerB_vision);
                }
                if (strcmp(command, "SHOOT")==0)
                {
                    can_win = true;
                    if (extended_switch)
                    {
                        int x, y;
                        int ship_number;
                        char ship_class[3];
                        cin >> ship_number >> ship_class >> y >> x;
                        extended_shootout(Board, Board_Y,Board_X, y, x, ship_number, A, command, ship_class,playerA_vision,playerB_vision,ships_quantity);
                    }
                    else
                    {
                        int x, y;
                        cin >> y >> x;
                        shootout(Board, Board_Y, Board_X, y, x, ships_quantity, B, command,playerA_vision,playerB_vision);
                    }
                    refresh_ships_after_shootout(Board, B);
                }
                if (strcmp(command, "MOVE") == 0)
                {                    
                    int ship_number;
                    char ship_class[3], direction_of_move;
                    cin >> ship_number >> ship_class >> direction_of_move;
                    int length_temp = get_length_temp(ship_class);
                    if (!ship_data[active_player][length_temp][ship_number].engine_state)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP CANNOT MOVE");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    if (ship_data[active_player][length_temp][ship_number].available_moves == 0)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class,direction_of_move, "SHIP MOVED ALREADY");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    if (!is_ship_on_board(Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x))
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP WENT FROM BOARD");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    delete_ship(Board, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, length_temp);
                    ships_quantity[length_temp][active_player]++;
                    move_ship(active_player, ship_number, ship_class, direction_of_move);
                    ship_placement(Board, Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y,
                        ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_number, ship_class, active_player, ships_quantity,
                        command, ship_data[active_player][length_temp][ship_number].ship_state,direction_of_move,playerA_vision,playerB_vision);                                                                
                }
                if (strcmp(command,"[playerA]") == 0)
                {
                    ship_move_reset(A);
                    if (player_B.alive_ship_parts == 0 && can_win)
                    {
                        cout << "A won" << endl;
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    active_player = B;
                    break;
                }
            }
        }
        if (strcmp(command_group, "[playerB]")==0)
        {
            reset_shots(B);
            if (active_player == A)
            {
                printf("INVALID OPERATION \"%s \": %s", command_group, "THE OTHER PLAYER EXPECTED");
                end_game(Board, playerA_vision, playerB_vision, Board_Y);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "SPY")==0)
                {
                    int y, x, carrier_number;
                    cin >> carrier_number >> y >> x;
                    if (!ship_data[active_player][CAR][carrier_number].canon_state)
                    {
                        printf("INVALID OPERATION \"%s %i %i %i\": %s", command, carrier_number, y, x, "CANNOT SEND PLANE");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    if (ship_data[active_player][CAR][carrier_number].available_shots == 0)
                    {
                        printf("INVALID OPERATION \"%s %i %i %i\": %s", command, carrier_number, y, x, "ALL PLANES SENT");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    make_area_visible(playerB_vision, Board_Y, Board_X, y, x, CAR, false, SPY_PLANE);
                    ship_data[active_player][CAR][carrier_number].available_shots--;
                }
                if (strcmp(command, "PRINT") == 0)
                {
                    int control_number;
                    cin >> control_number;
                    if (control_number == 0)
                    {
                        basic_player_print(playerB_vision,Board, Board_Y, Board_X);
                    }
                }
                if (strcmp(command, "PLACE_SHIP") == 0)
                {
                    char direction_of_move = 'X';
                    int x, y, ship_number;
                    char ship_class[256], direction, ship_state[5];
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    for (i = 0; i < 5; i++) ship_state[i] = '1';
                    ship_placement(Board, Board_Y, Board_X, y, x, direction, ship_number, ship_class, active_player, ships_quantity, command, ship_state,direction_of_move,playerA_vision,playerB_vision);
                }
                if (strcmp(command, "SHOOT") == 0)
                {
                    can_win = true;
                    int x, y;
                    if (extended_switch)
                    {
                        int ship_number;
                        char ship_class[3];
                        cin >> ship_number >> ship_class >> y >> x;
                        extended_shootout(Board, Board_Y,Board_X, y, x, ship_number, B, command, ship_class,playerA_vision,playerB_vision,ships_quantity);
                    }
                    else
                    {
                        cin >> y >> x;
                        shootout(Board, Board_Y, Board_X, y, x, ships_quantity, A, command,playerA_vision,playerB_vision);
                    }
                    refresh_ships_after_shootout(Board, A);
                }
                if (strcmp(command, "MOVE") == 0)
                {
                    int ship_number;
                    char ship_class[3], direction_of_move;
                    cin >> ship_number >> ship_class >> direction_of_move;
                    int length_temp = get_length_temp(ship_class);
                    if (!ship_data[active_player][length_temp][ship_number].engine_state)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP CANNOT MOVE");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    if (ship_data[active_player][length_temp][ship_number].available_moves == 0)
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP MOVED ALREADY");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    delete_ship(Board, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, length_temp);
                    ships_quantity[length_temp][active_player]++;
                    move_ship(active_player, ship_number, ship_class, direction_of_move);
                    if (!is_ship_on_board(Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y, ship_data[active_player][length_temp][ship_number].placed_x))
                    {
                        printf("INVALID OPERATION \"%s %i %s %c\": %s", command, ship_number, ship_class, direction_of_move, "SHIP WENT FROM BOARD");
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    ship_placement(Board, Board_Y, Board_X, ship_data[active_player][length_temp][ship_number].placed_y,
                        ship_data[active_player][length_temp][ship_number].placed_x, ship_data[active_player][length_temp][ship_number].direction, ship_number, ship_class, active_player, ships_quantity,
                        command, ship_data[active_player][length_temp][ship_number].ship_state, direction_of_move,playerA_vision,playerB_vision);
                }
                if (strcmp(command, "[playerB]") == 0)
                {
                    ship_move_reset(B);
                    if (player_A.alive_ship_parts == 0 && can_win)
                    {
                        cout << "B won" << endl;
                        end_game(Board, playerA_vision, playerB_vision, Board_Y);
                    }
                    active_player = A;
                    break;
                }
            }
        }
    }
    return 0;
}