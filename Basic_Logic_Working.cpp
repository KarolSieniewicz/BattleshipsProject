#include <iostream>
#include <stdlib.h>
using namespace std;

//definicje
#define Board_X 10
#define Board_Y 21

#define CAR_size 5
#define BAT_size 4
#define CRU_size 3
#define DES_size 2

#define alive_ship_part '+'
#define dead_ship_part 'x'

#define MAX_ships 10


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

void ships_availability(bool available_ship_index[2][4][20], int player) {
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 20; j++)
        {
            available_ship_index[player][i][j] = true;
        }
    }
}


bool ship_position_validation(int x, int y, int active_player, int ship_length, char direction)
{
    if (x<0 || x>Board_X) return false;
    if (active_player == A)
    {
        if (y<0 || y>Board_Y / 2 - 1) return false;
        //dir validation
        switch (direction)
        {
        case 'N':
            if (y + 4 - ship_length > Board_Y/2-1) return false;
            break;
        case 'E':
            if (x - 4 + ship_length < 0) return false;
            break;
        case 'S':
            if (y - 4 + ship_length < 0) return false;
            break;
        case 'W':
            if (x + 4 - ship_length > Board_X) return false;
            break;
        }
    }
    if (active_player == B)
    {
        if (y<0 || y>Board_Y - 1) return false;
        switch (direction)
        {
        case 'N':
            if (y + 4 - ship_length > Board_Y) return false;
            break;
        case 'E':
            if (x - 4 + ship_length < 0) return false;
            break;
        case 'S':
            if (y - 4 + ship_length < Board_Y/2+1) return false;
            break;
        case 'W':
            if (x + 4 - ship_length > Board_X) return false;
            break;
        }
    }
    return true;
}


void ship_placement(char ships[Board_Y][Board_X], int y, int x, char direction, int ship_number, char ship_class[3], int player, int alive_ship_parts[2], bool available_ship_index[2][4][20], int ship_quantity[4][2], char command[20])
{
    int i;
    int tempx = x;
    int tempy = y;
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
    if (!ship_position_validation(x,y,player,length_temp,direction))
    {
        printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s",command, y, x, direction, ship_number, ship_class,"NOT IN STARTING POSITION");
        exit(0);
    }
    if (available_ship_index[player][length_temp][ship_number]==false)
    {
        printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "SHIP ALREADY PRESENT");
        exit(0);
    }
    if (ship_quantity[length_temp][player] == 0)
    {
        printf("INVALID OPERATION \"%s %i %i %c %i %s\": %s", command, y, x, direction, ship_number, ship_class, "ALL SHIPS OF THE CLASS ALREADY SET");
        exit(0);
    }
    ship_quantity[length_temp][player]--;
    available_ship_index[player][length_temp][ship_number] = false;
    for (i = 0; i < 5 - length_temp; i++)
    {
        alive_ship_parts[player]++;
        ships[tempy][tempx] = alive_ship_part;
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

void shootout(char Board[Board_Y][Board_X], int aimed_y, int aimed_x, int ships_quantity[4][2], int alive_ship_parts[2], int aimed_player, char command[20])
{
    if (aimed_x > Board_X || aimed_x<0 || aimed_y<0 || aimed_y > Board_Y)
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "FIELD DOES NOT EXIST"); 
        exit(0);
    }
    if (are_all_ships_placed(ships_quantity))
    {
        if (Board[aimed_y][aimed_x] == alive_ship_part)
        {
            Board[aimed_y][aimed_x] = dead_ship_part;
            alive_ship_parts[aimed_player]--;
        }
    }
    else
    {
        printf("INVALID OPERATION \"%s %i %i\": %s", command, aimed_y, aimed_x, "NOT ALL SHIPS PLACED");
        exit(0);
    }

}

void basic_print(char Board[Board_Y][Board_X], int alive_parts_A, int alive_parts_B)
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
    cout << "PARTS REMAINING:: " << "A : " << alive_parts_A << " B : " << alive_parts_B << endl;
}


int main()
{
    int i, j;
    char Board[Board_Y][Board_X];
    int ships_quantity[4][2], alive_ship_parts[2];
    bool available_ship_index[2][4][20];
    int active_player = A;
    bool can_win = false;

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
            for (int n = 0; n < 20; n++)
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
        alive_ship_parts[i] = 0;
    }
    //default indeksy
    ships_availability(available_ship_index, A);
    ships_availability(available_ship_index, B);
    char command_group[20];

    while (cin >> command_group)
    {
        if (strcmp(command_group, "[state]")==0)
        {
            while (true)
            {
                char command[20];
                cin >> command;
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
                        basic_print(Board, alive_ship_parts[A], alive_ship_parts[B]);
                    }
                }
                if (strcmp(command, "NEXT_PLAYER")==0)
                {
                    cin >> active_player;
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
                exit(0);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "PLACE_SHIP")==0)
                {
                    int x, y, ship_number;
                    char ship_class[256], direction;
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    ship_placement(Board, y, x, direction, ship_number, ship_class , active_player, alive_ship_parts,available_ship_index,ships_quantity,command);
                }
                if (strcmp(command, "SHOOT")==0)
                {
                    can_win = true;
                    int x, y;
                    cin >> y >> x;
                    shootout(Board, y, x, ships_quantity, alive_ship_parts, B, command);
                }
                if (strcmp(command,"[playerA]") == 0)
                {
                    if (alive_ship_parts[B] == 0 && can_win)
                    {
                        cout << "A won" << endl;
                        return 0;
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
                exit(0);
            }
            while (true)
            {
                char command[20];
                cin >> command;
                if (strcmp(command, "PLACE_SHIP") == 0)
                {
                    int x, y, ship_number;
                    char ship_class[256], direction;
                    cin >> y >> x >> direction >> ship_number >> ship_class;
                    ship_placement(Board, y, x, direction, ship_number, ship_class, active_player, alive_ship_parts, available_ship_index, ships_quantity,command);
                }
                if (strcmp(command, "SHOOT") == 0)
                {
                    can_win = true;
                    int x, y;
                    cin >> y >> x;
                    shootout(Board, y, x, ships_quantity, alive_ship_parts, A, command);
                }
                if (strcmp(command, "[playerB]") == 0)
                {
                    if (alive_ship_parts[A] == 0 && can_win)
                    {
                        cout << "B won" << endl;
                        return 0;
                    }
                    active_player = A;
                    break;
                }
            }
        }
    }
    return 0;
}