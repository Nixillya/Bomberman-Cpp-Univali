#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>

using namespace std;

#define freeBlock 0
#define solidBlock 1
#define fragileBlock 2
#define bombBlock 3
#define explosionBlock 4

struct XY{
    int X=-1; 
    int Y=-1;
};

struct PositionType{
    XY Pos;
};

struct BombType{
    XY Pos;
    int Time=0;
    int Range;
    int Type;
};

struct EnemyType{
    XY Pos;
    int Move;
};

struct PlayerType{
    XY Pos;
    int MaxBombs=1;
    int MaxRange=1;
    int Lifes=1;
    int ActualBomb=0;
    int Invincible=2;
    int Item=0;
};

struct TimerType{
    int Sec;
    int Min;
    int Hour;
};

const int mapSizeY = 15;
const int mapSizeX = 25;

int VA(int number){
    if(number<0){
        number*=-1;
    }
    return number;
}

void wait(int milisecunds){
    int waits = clock();
    while(clock()-waits<milisecunds) {
        if (kbhit()) {
            getch();
        }
    }
}

bool explode(int map[mapSizeY][mapSizeX], BombType bomb, int y, int x,int &playerPoints,int i=0){
    if(i>=1){
        y += (y!=0) ? y/VA(y) : 0;
        x += (x!=0) ? x/VA(x) : 0;
    }
    if (map[bomb.Pos.Y+y][bomb.Pos.X+x] == solidBlock){
        return false;
    }else{
        i++;
        if(bomb.Range!=1000){
            if(map[bomb.Pos.Y+y][bomb.Pos.X+x] == fragileBlock || i>=bomb.Range){
                map[bomb.Pos.Y+y][bomb.Pos.X+x] = explosionBlock;
                return false;
            }
        }else{
            if(map[bomb.Pos.Y+y][bomb.Pos.X+x] == fragileBlock){
                map[bomb.Pos.Y+y][bomb.Pos.X+x] = explosionBlock;
            }
        }
        map[bomb.Pos.Y+y][bomb.Pos.X+x] = explosionBlock;
        explode(map,bomb,y,x,playerPoints,i);
        return false;
    }
}

void new_line(string x, string y, string z,int size){
    cout<<x;
    for(int i = 0; i<size; i++){
        cout<<y;
    }
    cout<<z<<endl;
}

void render_details(TimerType timer,PlayerType player,int playerTotalMoves, int playerTotalBombs, int playerPoints){
    cout << "\e[2;"<<mapSizeX+15<<"H";
    if(timer.Sec<10){
        cout << 0;
    }
    cout << timer.Sec;
    cout << "\e[2;"<<mapSizeX+12<<"H";
    if(timer.Min<10){
        cout << 0;
    }
    cout << timer.Min;
    cout << "\e[2;"<<mapSizeX+9<<"H";
    if(timer.Hour<10){
        cout << 0;
    }
    cout << timer.Hour;

    cout<<"\e[2;"<<mapSizeX+20<<"H";
    if(playerPoints<1000000){
        cout<<0;
    }
    if(playerPoints<100000){
        cout<<0;
    }
    if(playerPoints<10000){
        cout<<0;
    }
    if(playerPoints<1000){
        cout<<0;
    }
    if(playerPoints<100){
        cout<<0;
    }
    if(playerPoints<10){
        cout<<0;
    }
    cout<<playerPoints;

    cout<<"\e[3;"<<mapSizeX+4<<"H";
    cout<<"      "<<playerTotalMoves<<"---"<<playerTotalBombs<<" ";

    cout<<"\e[6;"<<mapSizeX+4<<"H";
    cout<<"◉:";
    if(player.MaxBombs<10){
        cout<<"0"<<player.MaxBombs;
    }else{
        cout<<player.MaxBombs;
    }
    cout<<" ◈:";
    if(player.MaxRange<10){
        cout<<"0"<<player.MaxRange;
    }else{
        cout<<player.MaxRange;
    }
    cout<<" ☤:";
    if(player.Lifes<10){
        cout<<"0"<<player.Lifes;
    }else{
        cout<<player.Lifes;
    }

    cout<<"\e[6;"<<mapSizeX+19<<"H";
    if(player.Item==0){
        cout<<" ";
    }
    if(player.Item==1){
        cout<<"◊";
    }
    if(player.Item==2){
        cout<<"⬘";
    }
    if(player.Item==3){
        cout<<"Ω";
    }
}

void found_danger(int danger[4],int y,int x){
    if(y<0){
        danger[0] = 119;
    }
    if(y>0){
        danger[1] = 115;
    }
    if(x<0){
        danger[2] = 97;
    }
    if(x>0){
        danger[3] = 100;
    }
}

bool theres_bomb(BombType bombs[], int maximumBombs){
    for(int bomb=0; bomb<maximumBombs; bomb++){
        if(bombs[bomb].Pos.Y!=-1 && bombs[bomb].Pos.X!=-1){
            return true;
        }
    }
    return false;
}

int count_bombs(BombType bombs[], int maximumBombs){
    int totalbombs = 0;
    for(int bomb=0; bomb<maximumBombs; bomb++){
        if(bombs[bomb].Pos.Y!=-1 && bombs[bomb].Pos.X!=-1){
            totalbombs++;    
        }
    }
    return totalbombs;
}

int game(int difficulty, int players,TimerType &timer, int &phase, int &playerTotalMoves, int &playerTotalBombs, int &playerPoints){

//------------------------> VARIAVEIS GERAIS >------------------------//

    int enemysQuantity;
    if(difficulty==1){
        enemysQuantity = 3*phase;
    }
    if(difficulty==2){
        enemysQuantity = 5*phase;
    }
    if(difficulty==3){
        enemysQuantity = 7*phase;
    }
    if(phase==3){
        enemysQuantity=1;
    }
    int moveEnemy = clock();
    int freezeEnemys = 0;
    EnemyType enemys[enemysQuantity];

    int fragileWallQuantity = 50;
    PositionType boxs[fragileWallQuantity];
    PositionType portal;

    PlayerType player;
    int cpuMoves = clock();

    int maximumBombs = 25;
    BombType bombs[maximumBombs];
    BombType explosion[maximumBombs]; 

    int timerClock = clock();

    int map[mapSizeY][mapSizeX];

//------------------------< VARIAVEIS GERAIS <------------------------//

//------------------------> GERAÇÃO DO MAPA >------------------------//
    srand(time(0));
    for (int y = 0; y < mapSizeY; y++) {
        for (int x = 0; x  <mapSizeX; x++) {
            if (x == 0 || y == 0) {
               map[y][x] = solidBlock;
            } else if (x == mapSizeX - 1 || y == mapSizeY - 1) {
               map[y][x] = solidBlock;
            } else {
               map[y][x] = freeBlock;
            }
        }
    }
    if(phase!=3){
        for (int y = 0; y < mapSizeY; y += 2){
            for (int x = 0; x < mapSizeX; x += 2){
                if(map[y][x]==freeBlock){
                    if(rand()%10!=0){
                        map[y][x] = solidBlock;
                    }else{
                        map[y][x] = fragileBlock;
                    }
                }
            }
        }
        for (int i=0; i < fragileWallQuantity; i++){
            PositionType fragil;
            while(true){
                fragil.Pos.Y = rand()%mapSizeY;
                fragil.Pos.X = rand()%mapSizeX;
                if (map[fragil.Pos.Y][fragil.Pos.X] == freeBlock){
                    bool success = false;
                    for(int y=-1;y<=1;y++){
                        for(int x=-1;x<=1;x++){
                            if(y==0 || x==0){
                                if(map[fragil.Pos.Y+y][fragil.Pos.X+x] == fragileBlock || map[fragil.Pos.Y+y][fragil.Pos.X+x] == solidBlock){
                                    success = true;
                                }
                            }
                        }
                    }
                    if(success){
                        map[fragil.Pos.Y][fragil.Pos.X] = fragileBlock;
                        if(rand()%4==0){
                            boxs[i].Pos.Y=fragil.Pos.Y;
                            boxs[i].Pos.X=fragil.Pos.X;
                        }
                        break;
                    }
                }
            }
        }
        while(true){ // Gera a posição do Portal
            bool success = false;
            portal.Pos.Y = rand()%mapSizeY,
            portal.Pos.X = rand()%mapSizeX;
            if(map[portal.Pos.Y][portal.Pos.X] == fragileBlock){ // Verifica se a posição do player é uma area livre, se for, ele faz uma verificação para não gerar o player perto de paredes frageis
                success = true;
                for(int box=0;box<fragileWallQuantity;box++){
                    if(portal.Pos.Y == boxs[box].Pos.Y && portal.Pos.X == boxs[box].Pos.X){
                        success = false;
                    }
                }
                if(success){
                    break;
                }
            }
        }
    }
    while(true){ // Gera a posição do jogador
        bool success = false;
        player.Pos.Y = rand()%mapSizeY,
        player.Pos.X = rand()%mapSizeX;
        if (map[player.Pos.Y][player.Pos.X] == freeBlock){ // Verifica se a posição do player é uma area livre, se for, ele faz uma verificação para não gerar o player perto de paredes frageis
            success = true;
            for (int y = -1; y < 2; y++){
                for (int x = -1; x < 2; x++){ // Passa por uma matriz 3x3 em volta do player
                    if(y==0 || x==0){ // Verifica se tem uma parede fragil em volta do player, se tiver, ele gera outra posição
                        if (map[player.Pos.Y+y][player.Pos.X+x] == fragileBlock){
                            success = false;
                        }
                    }
                }
            }
            if(success){
                break;
            }
        }
    }
    for (int  i = 0; i < enemysQuantity; i++){ // Gera as posições dos enemys
        EnemyType enemy;   // Posição do inimigo
        while(true){ // Gera uma posição aleatoria para o inimigo e faz as verificações para não gerar o inimigo emcima do player, de outros enemys ou em blocos proibidos
            bool success = false;
            enemy.Pos.Y = rand()%mapSizeY;
            enemy.Pos.X = rand()%mapSizeX;
            enemy.Move = 0;
            for (int otherEnemy = 0; otherEnemy < enemysQuantity; otherEnemy++){
                if (enemy.Pos.X == enemys[otherEnemy].Pos.X && enemy.Pos.Y == enemys[otherEnemy].Pos.Y){
                    enemy.Pos.Y = player.Pos.Y;
                    enemy.Pos.X = player.Pos.X;
                }
            }
            if(enemy.Pos.X != player.Pos.X && enemy.Pos.Y != player.Pos.Y) {
                if(map[enemy.Pos.Y][enemy.Pos.X] == freeBlock){
                    success = true;
                    for (int y = -1; y < 2; y++){
                        for (int x = -1; x < 2; x++){
                            if(y==0 || x==0){
                                if (map[enemy.Pos.Y+y][enemy.Pos.X+x] == fragileBlock){
                                    success = false;
                                }
                            }
                        }
                    }
                    if(success){
                        enemys[i] = {enemy};
                        break;
                    }
                }
            }
        }
    }
//------------------------< GERAÇÃO DO MAPA <------------------------//

//------------------------> BORDAS LATERAIS >------------------------//
    cout<<"\e[1;"<<mapSizeX+3<<"H";
    new_line("┏","━","┓",23);
    cout<<"\e[2;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",23);
    cout<<"\e[3;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",23);
    cout<<"\e[4;"<<mapSizeX+3<<"H";
    new_line("┗","━","┛",23);
    cout << "\e[2;"<<mapSizeX+3<<"H";
    cout << "┃["<<phase<<"] [00:00:00]";

    cout<<"\e[5;"<<mapSizeX+3<<"H";
    new_line("┏","━","┳",14);
    cout<<"\e[6;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",14);
    cout<<"\e[7;"<<mapSizeX+3<<"H";
    new_line("┗","━","┻",14);
    cout<<"\e[5;"<<mapSizeX+18<<"H";
    new_line("┳","━","┓",1);
    cout<<"\e[6;"<<mapSizeX+18<<"H";
    new_line("┃"," ","┃",1);
    cout<<"\e[7;"<<mapSizeX+18<<"H";
    new_line("┻","━","┛",1);
//------------------------< BORDAS LATERAIS <------------------------//

//---------------------------> RENDERIZAÇÃO DO MAPA >---------------------------//
    while(true){
        if(player.ActualBomb>=maximumBombs){
            player.ActualBomb = 0;
        }
        if(playerPoints<0){
            playerPoints=0;
        }
        cout << "\e[?25l\e[1;1H";
        new_line("┏","━","┓",mapSizeX);
        for (int y = 0; y < mapSizeY; y++) {
            cout<<"┃";
            for (int x = 0; x < mapSizeX; x++) {
                 bool block = true;
                 if(block){
                    for(int enemy=0; enemy<enemysQuantity; enemy++) {
                        if(y==enemys[enemy].Pos.Y && x==enemys[enemy].Pos.X) {
                            block = false;
                            if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X]==explosionBlock) { // Testa se naquela posição tem um bloco de explosão
                                if(freezeEnemys==0){
                                    cout << "\e[31;43m\u25A1"; // INIMIGO MORTO NA EXPLOSÃO
                                }else{
                                    cout << "\e[31;43m\e[38;5;52m\u25A1"; // INIMIGO MORTO NA EXPLOSÃO CONGELADO
                                }

                            }else{
                                if(player.Pos.Y==enemys[enemy].Pos.Y && player.Pos.X==enemys[enemy].Pos.X) {
                                    cout << "\e[31;42m\u25CB"; // PLAYER MORTO NO INIMIGO
                                    break;
                                }else{
                                    if(freezeEnemys==0){
                                        cout << "\e[31;42m\u25A0"; // INIMIGO
                                        break;
                                    }else{
                                        cout << "\e[31;42m\e[38;5;52m\u25A0"; // INIMIGO CONGELADO
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(block){
                    if (player.Pos.Y == y && player.Pos.X == x) {
                        block = false;
                        if(map[player.Pos.Y][player.Pos.X] == explosionBlock){
                            cout << "\e[97;43m\u25CB"; // PLAYER MORTO NA EXPLOSÃO
                        }else{
                            if(player.Invincible==0){
                                if(map[player.Pos.Y][player.Pos.X] == fragileBlock){
                                        cout << "\e[97;42m\e[48;5;245m\u25CF"; // PLAYER
                                        player.Item = 0;
                                }else{
                                    cout << "\e[97;42m\u25CF"; // PLAYER
                                }
                            }else{
                                if(rand()%2){
                                    cout << "\e[0;42m\e[38;5;21m\u25CF";
                                }else{
                                    cout << "\e[0;42m\e[38;5;51m\u25CF";
                                }
                            }
                        }
                    }
                }
                if(block){
                    if(map[y][x] == explosionBlock){
                        cout << "\e[33;43m "; // EXPLOSÃO
                        block = false;
                        continue;
                    }
                }
                if(block){
                    if(portal.Pos.Y==y && portal.Pos.X==x){
                        if(map[y][x] == freeBlock){
                            if(rand()%2){
                                cout << "\e[32;42m\e[38;5;93m\u25CF";
                                continue;
                            }else{
                                cout << "\e[32;42m\e[38;5;207m\u25CF";
                                continue;
                            }
                        }
                    }
                }
                if(block){
                    for(int box=0; box<fragileWallQuantity; box++){
                        if(boxs[box].Pos.Y==y && boxs[box].Pos.X==x){
                            if(map[y][x] == freeBlock){
                                cout<<"\e[32;42m\e[38;5;3m\u25CF";
                                block = false;
                                continue;
                            }
                        }
                    }
                }
                if (block) {
                    if (map[y][x] == freeBlock){
                        cout << "\e[32;42m "; // AREÁ LIVRE
                        continue;
                    }
                    if (map[y][x] == solidBlock){
                        cout << "\e[48;5;243m "; // PAREDE SOLIDA
                        continue;
                    }
                    if (map[y][x] == fragileBlock){
                        cout << "\e[48;5;245m "; // PAREDE FRAGIL
                        continue;
                    }
                    if (map[y][x] == bombBlock){
                        for(int bomb=0; bomb<maximumBombs; bomb++){
                            if(bombs[bomb].Pos.Y==y && bombs[bomb].Pos.X==x){
                                if(rand()%2){
                                    cout << "\e[30;42m\u25C9"; // BOMBA
                                    continue;
                                }else{
                                    cout << "\e[33;42m\u25C9"; // BOMBA
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
            cout << "\e[0m┃\n";
        }
        new_line("┗","━","┛",mapSizeX);
        render_details(timer,player,playerTotalMoves,playerTotalBombs,playerPoints);
//---------------------------< RENDERIZAÇÃO DO MAPA <---------------------------//

//----------------------> SISTEMA DO PLAYER >----------------------//
        if(player.Pos.Y == portal.Pos.Y && player.Pos.X == portal.Pos.X){
            phase++;
            return 1;
        }
        if(player.Invincible==0){
            if (map[player.Pos.Y][player.Pos.X] == explosionBlock){
                player.Lifes--;
                playerPoints-=200;
                if(player.Lifes>=1){
                    moveEnemy = clock();
                    timerClock = clock();
                    player.Item = 0;
                    player.Invincible = 1;
                    continue;
                }else{
                    if(playerPoints<0){
                        playerPoints=0;
                    }
                    render_details(timer,player,playerTotalMoves,playerTotalBombs,playerPoints);
                    return 0;
                }
            }else{
                for(int enemy=0; enemy<enemysQuantity; enemy++){
                    if(player.Pos.Y==enemys[enemy].Pos.Y && player.Pos.X==enemys[enemy].Pos.X){
                        player.Lifes--;
                        playerPoints-=200;
                        if(player.Lifes>=1){
                            moveEnemy = clock();
                            timerClock = clock();
                            player.Item = 0;
                            player.Invincible = 1;
                            continue;
                        }else{
                            if(playerPoints<0){
                                playerPoints=0;
                            }
                            render_details(timer,player,playerTotalMoves,playerTotalBombs,playerPoints);
                            return 0;
                        }
                    }
                    if(player.Item==1){
                        for(int y=-1; y<=1; y++){
                            for(int x=-1; x<=1; x++){
                                if(player.Pos.Y+y==enemys[enemy].Pos.Y && player.Pos.X+x==enemys[enemy].Pos.X){
                                    freezeEnemys = 3;
                                    player.Item = 0;
                                    timerClock = clock();
                                }
                            }
                        }
                    }
                }
            }
        }
        for(int box=0;box<fragileWallQuantity;box++){
            if(player.Pos.Y==boxs[box].Pos.Y && player.Pos.X==boxs[box].Pos.X){
                boxs[box].Pos.Y = -1;
                boxs[box].Pos.X = -1;
                if(rand()%10!=0 || player.Item!=0){
                    int item = rand()%3;
                    if(item==0){
                        player.MaxBombs++;
                    }
                    if(item==1){
                        player.MaxRange++;
                    }
                    if(item==2){
                        player.Lifes++;
                    }
                }else{
                    bool success = false;
                    while(!success){
                        int item = rand()%3+1;
                        player.Item = item;
                        success = true;
                    }
                }
            }
        }

        if (kbhit() || players==1){
            PositionType target;
            target.Pos.Y = 0;
            target.Pos.X = 0;
            int key;

            if(players==2){
                key = getch();
            }
//----------------------> CPU PLAYER >----------------------//
            if(players==1){
                key = 0;
                int danger[4] = {-1,-1,-1,-1};
                if((clock()-cpuMoves) >= 0){
                    cpuMoves = clock();
                    int totalEnemys = 0;
                    bool putBomb = false;
                    for(int y=-1;y<=1;y++){
                        for(int x=-1;x<=1;x++){
                            if((y==0||x==0)){
                                if(player.Item==2 && player.Invincible==0){
                                    if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock && map[player.Pos.Y+y][player.Pos.X+x]!=fragileBlock){
                                        found_danger(danger,y,x);
                                    }
                                }else{
                                    if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock){
                                        found_danger(danger,y,x);
                                    }
                                }

                                for(int enemy=0;enemy<enemysQuantity;enemy++){
                                    if((player.Pos.Y+y == enemys[enemy].Pos.Y && player.Pos.X+x == enemys[enemy].Pos.X)){
                                        found_danger(danger,y,x);
                                        putBomb = true;
                                    }
                                    int Y = 0;
                                    int X = 0;
                                    if(enemys[enemy].Move == 1){
                                        Y--;
                                    }
                                    if(enemys[enemy].Move == 2){
                                        Y++;
                                    }
                                    if(enemys[enemy].Move == 3){
                                        X--;
                                    }
                                    if(enemys[enemy].Move == 4) {
                                        X++;
                                    }
                                    if(((player.Pos.Y+y == enemys[enemy].Pos.Y+Y && player.Pos.X+x == enemys[enemy].Pos.X+X) && freezeEnemys==0)){
                                        putBomb = true;
                                        found_danger(danger,y,x);
                                    }
                                }
                            }
                        }
                    }
                    for(int y=-2;y<=2;y++){
                        for(int x=-2;x<=2;x++){
                            if((player.Pos.Y+y>0 && player.Pos.Y+y<mapSizeY && player.Pos.X+x>0 && player.Pos.X+x<mapSizeX)){
                                if(((y>=-1||x>=-1) && (y<=1||x<=1)) || (y==0||x==0)){
                                    for(int enemy=0;enemy<enemysQuantity;enemy++){
                                        if((player.Pos.Y+y == enemys[enemy].Pos.Y && player.Pos.X+x == enemys[enemy].Pos.X)){
                                            totalEnemys++;
                                        }
                                    }
                                    if(map[player.Pos.Y+y][player.Pos.X+x]==bombBlock){
                                        for(int bomb=0;bomb<maximumBombs;bomb++){
                                            if((bombs[bomb].Pos.Y==player.Pos.Y+y && bombs[bomb].Pos.X==player.Pos.X+x) && (clock()-bombs[bomb].Time)>=850){
                                                found_danger(danger,y,x);
                                            }
                                        }
                                    }
                                }
                                if(((y>=-1&&x>=-1) && (y<=1&&x<=1)) || (y==0||x==0)){
                                    if(totalEnemys>0 || map[player.Pos.Y][player.Pos.X]==bombBlock){
                                        if(y==-2 && x==0){
                                            if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock){
                                                int y1 = y+1;
                                                int x1 = x-1;
                                                if(map[player.Pos.Y+y1][player.Pos.X+x1]!=freeBlock){
                                                    int y2 = y1;
                                                    int x2 = x1+2;
                                                    if(map[player.Pos.Y+y2][player.Pos.X+x2]!=freeBlock){
                                                        found_danger(danger,y,x);
                                                    }
                                                }
                                            }
                                        }
                                        if(y==0 && x==2){
                                            if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock){
                                                int y1 = y-1;
                                                int x1 = x-1;
                                                if(map[player.Pos.Y+y1][player.Pos.X+x1]!=freeBlock){
                                                    int y2 = y1+2;
                                                    int x2 = x1;
                                                    if(map[player.Pos.Y+y2][player.Pos.X+x2]!=freeBlock){
                                                        found_danger(danger,y,x);
                                                    }
                                                }
                                            }
                                        }
                                        if(y==2 && x==0){
                                            if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock){
                                                int y1 = y-1;
                                                int x1 = x+1;
                                                if(map[player.Pos.Y+y1][player.Pos.X+x1]!=freeBlock){
                                                    int y2 = y1;
                                                    int x2 = x1-2;
                                                    if(map[player.Pos.Y+y2][player.Pos.X+x2]!=freeBlock){
                                                        found_danger(danger,y,x);
                                                    }
                                                }
                                            }
                                        }
                                        if(y==0 && x==-2){
                                            if(map[player.Pos.Y+y][player.Pos.X+x]!=freeBlock){
                                                int y1 = y+1;
                                                int x1 = x+1;
                                                if(map[player.Pos.Y+y1][player.Pos.X+x1]!=freeBlock){
                                                    int y2 = y1-2;
                                                    int x2 = x1;
                                                    if(map[player.Pos.Y+y2][player.Pos.X+x2]!=freeBlock){
                                                        found_danger(danger,y,x);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    int safes = 0;
                    for(int i=0;i<4;i++){
                        safes += (danger[i]==-1) ? 1 : 0;
                    }
                    if(rand()%2==0 && !theres_bomb(bombs,maximumBombs) && totalEnemys>=1){
                        int enemyChoiced = -1;
                        for(int enemy=0;enemy<enemysQuantity;enemy++){
                            if(enemys[enemy].Pos.X==-1 && enemys[enemy].Pos.Y==-1){
                                continue;
                            }
                            if(enemys[enemy].Pos.X!=-1 && enemys[enemy].Pos.Y!=-1){
                                if(enemyChoiced==-1){
                                    enemyChoiced = enemy;
                                }else{
                                    if((VA(player.Pos.Y-enemys[enemy].Pos.Y) < VA(player.Pos.Y-enemys[enemyChoiced].Pos.Y)) && (VA(player.Pos.X-enemys[enemy].Pos.X) < VA(player.Pos.X-enemys[enemyChoiced].Pos.X))){
                                        enemyChoiced = enemy;
                                    }
                                }
                            }
                        }
                        int y = player.Pos.Y-enemys[enemyChoiced].Pos.Y;
                        int x = player.Pos.X-enemys[enemyChoiced].Pos.X;
                        found_danger(danger,y,x);
                    }
                    for(int tryIt=0;tryIt<11;tryIt++){
                        if(tryIt==10){
                            key = 0;
                            break;
                        }
                        bool success = true;
                        key = rand()%4;
                        key = (key==0) ? 119 : (key==1) ? 115 : (key==2) ? 97 : (key==3) ? 100 : 0;
                        for(int i=0;i<4;i++){
                            if(key==danger[i]){
                                success = false;
                            }
                        }
                        if(success){
                            break;
                        }
                    }
                    for(int y=-1;y<=1;y++){
                        for(int x=-1;x<=1;x++){
                            if((y==0||x==0) && (map[player.Pos.Y+y][player.Pos.X+x]==fragileBlock) && (rand()%4==0) && totalEnemys==0 && (((y>=-1||x>=-1) && (y<=1||x<=1)) && map[player.Pos.Y+y][player.Pos.X+x]!=bombBlock)){
                                key = 32;
                            }
                            if((y==0||x==0) && putBomb && (rand()%4==0) && safes>=1){
                                key = 32;
                            }
                        }
                    }
                }
            }
//----------------------< CPU PLAYER <----------------------//
            switch(key){
                case 119: // Cima
                    target.Pos.Y--;
                    break;
                case 115: // Baixo
                    target.Pos.Y++;
                    break;
                case 97: // Esquerda
                    target.Pos.X--;
                    break;
                case 100: // Direira
                    target.Pos.X++;
                    break;
            }
            player.Pos.Y += target.Pos.Y;
            player.Pos.X += target.Pos.X;
            if(key!=0){
                if(player.Item==2 && player.Invincible==0){
                    if(map[player.Pos.Y][player.Pos.X] != freeBlock && map[player.Pos.Y][player.Pos.X] != explosionBlock && map[player.Pos.Y][player.Pos.X] != fragileBlock){
                        player.Pos.Y -= target.Pos.Y;
                        player.Pos.X -= target.Pos.X;
                    }else{
                        playerTotalMoves++;
                    }
                }else{
                    if((map[player.Pos.Y][player.Pos.X] != freeBlock && map[player.Pos.Y][player.Pos.X] != explosionBlock)){ // Testa colisão e cancela o movimento se verdadeiro
                        player.Pos.Y -= target.Pos.Y;
                        player.Pos.X -= target.Pos.X;
                    }else{
                        playerTotalMoves++;
                    }
                }
                if(key == 32 && (map[player.Pos.Y][player.Pos.X]==freeBlock) && player.Invincible==0 && (bombs[player.ActualBomb].Pos.Y == -1 && bombs[player.ActualBomb].Pos.X == -1)){
                    if(count_bombs(bombs,maximumBombs)<player.MaxBombs){
                        playerTotalBombs++;
                        playerPoints-=10;
                        bombs[player.ActualBomb].Pos.Y = player.Pos.Y;
                        bombs[player.ActualBomb].Pos.X = player.Pos.X;
                        bombs[player.ActualBomb].Time = clock();
                    }
                }
            }
        }
//---------------------< SISTEMA DO PLAYER <----------------------//

//----------------------------> SISTEMA DOS INIMIGOS >----------------------------//
        if(((clock()-moveEnemy) >= 350) && freezeEnemys==0){
            // int actualEnemys = 0;
            moveEnemy = clock();
            for(int enemy = 0; enemy<enemysQuantity; enemy++){
                /*
                if(enemys[enemy].Pos.Y == -1 && enemys[enemy].Pos.X == -1){
                    if (enemy == enemysQuantity-1) {
                        if (actualEnemys == 0){
                            phase++;
                            return 1;
                            break;
                        }
                    }
                    continue;
                }
                actualEnemys++;
                */
                for(int box=0;box<fragileWallQuantity;box++){
                    if(enemys[enemy].Pos.Y == boxs[box].Pos.Y && enemys[enemy].Pos.X == boxs[box].Pos.X){
                        boxs[box].Pos.Y = -1;
                        boxs[box].Pos.X = -1;
                    }
                }

                int Y = 0;
                int X = 0;
                int enemyTarget = rand()%4+1;
                int forbidden[4] = {solidBlock, fragileBlock, bombBlock, explosionBlock};
                bool success = true;
                if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == explosionBlock){
                    continue;
                }
                if(enemys[enemy].Move == 1) {
                    Y--;
                }
                if(enemys[enemy].Move == 2) {
                    Y++;
                }
                if(enemys[enemy].Move == 3) {
                    X--;
                }
                if(enemys[enemy].Move == 4) {
                    X++;
                }
                enemys[enemy].Pos.Y += Y;
                enemys[enemy].Pos.X += X;
                if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == explosionBlock){
                    success = false;
                }
                for (int i = 0; i < 4; i++){
                    if (map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == forbidden[i]) {
                        success = false;
                        break;
                    }
                }
                for (int otherEnemy = 0; otherEnemy < enemysQuantity; otherEnemy++) {
                    if (otherEnemy == enemy) {
                        continue;
                    }
                    if (enemys[enemy].Pos.Y == enemys[otherEnemy].Pos.Y && enemys[enemy].Pos.X == enemys[otherEnemy].Pos.X){
                        success = false;
                        break;
                    }
                }
                if(!success){
                    enemys[enemy].Pos.Y -= Y;
                    enemys[enemy].Pos.X -= X;
                    enemys[enemy].Move = 0;
                }
                for( int toTry = 0; toTry < 4; toTry++) {
                    success = true;
                    Y = 0;
                    X = 0;
                    if(difficulty==2){
                        if(rand()%2!=0){
                            enemyTarget = 0;
                        }else{
                            enemyTarget = rand()%4+1;
                        }
                    }
                    if(difficulty==3){
                        if(rand()%4!=0){
                            enemyTarget = 0;
                        }else{
                            enemyTarget = rand()%4+1;
                        }
                    }
                    if(toTry>0 || theres_bomb(bombs,maximumBombs)){
                        enemyTarget = rand()%4+1;
                    }
                    if(enemyTarget==0){
                        int y = enemys[enemy].Pos.Y-player.Pos.Y;
                        int x = enemys[enemy].Pos.X-player.Pos.X;
                        if(rand()%2==0){
                            if(y<0){
                                enemyTarget = 2;
                            }
                            if(y>0){
                                enemyTarget = 1;
                            }
                        }else{
                            if(x<0){
                                enemyTarget = 4;
                            }
                            if(x>0){
                                enemyTarget = 3;
                            }
                        }
                    }
                    if(enemyTarget == 1) {
                        Y--;
                    }
                    if(enemyTarget == 2) {
                        Y++;
                    }
                    if(enemyTarget == 3) {
                        X--;
                    }
                    if(enemyTarget == 4) {
                        X++;
                    }
                    enemys[enemy].Pos.Y += Y;
                    enemys[enemy].Pos.X += X;
                    for (int i = 0; i < 4; i++){
                        if (map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == forbidden[i]) {
                            success = false;
                            break;
                        }
                    }
                    for (int otherEnemy = 0; otherEnemy < enemysQuantity; otherEnemy++) {
                        if (otherEnemy == enemy) {
                            continue;
                        }
                        if (enemys[enemy].Pos.Y == enemys[otherEnemy].Pos.Y && enemys[enemy].Pos.X == enemys[otherEnemy].Pos.X){
                            success = false;
                            break;
                        }
                    }
                    enemys[enemy].Pos.Y -= Y;
                    enemys[enemy].Pos.X -= X;
                    if (success){
                        enemys[enemy].Move = enemyTarget;
                        break;
                    } else {
                        continue;
                    }
                }
            }
        }
//----------------------------< SISTEMA DOS INIMIGOS <----------------------------//

//-----------------------------------> SISTEMA DAS BOMBAS >-----------------------------------//
        player.ActualBomb++;
        for(int bomb=0;bomb<maximumBombs;bomb++){
            bombs[bomb].Range = player.MaxRange;
            if(player.Item==3){
                player.Item = 0;
                bombs[bomb].Range = 1000;
            }
            if(map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X]==freeBlock){
                map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X] = bombBlock;
            }
            if(bombs[bomb].Pos.Y>0 && bombs[bomb].Pos.X>0 && ((clock()-bombs[bomb].Time) >= 1000)){
                map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X] = explosionBlock;
                explode(map,bombs[bomb],1,0,playerPoints);
                explode(map,bombs[bomb],0,1,playerPoints);
                explode(map,bombs[bomb],-1,0,playerPoints);
                explode(map,bombs[bomb],0,-1,playerPoints);
                explosion[bomb] = bombs[bomb];
                bombs[bomb].Pos.Y = -1;
                bombs[bomb].Pos.X = -1;
                bombs[bomb].Time = 0;
            }
            if(explosion[bomb].Pos.Y>0 && explosion[bomb].Pos.X>0 && ((clock()-explosion[bomb].Time) >= 1350)){
                for(int enemy = 0; enemy<enemysQuantity; enemy++){
                    if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == explosionBlock){
                        playerPoints+=200;
                        enemys[enemy].Pos.Y = -1;
                        enemys[enemy].Pos.X = -1;
                    }
                }
                for(int y = 0; y < mapSizeY; y++) {
                    for (int x = 0; x < mapSizeX; x++) {
                        if (map[y][x] == explosionBlock) {
                            map[y][x] = freeBlock;
                        }
                    }
                }
                explosion[bomb].Pos.Y = -1;
                explosion[bomb].Pos.X = -1;
                explosion[bomb].Time = 0;
            }
        }
//-----------------------------------< SISTEMA DAS BOMBAS <-----------------------------------//

//------------------------------> TIMER >------------------------------//
        if((clock()-timerClock)>=1000){
            if(player.Invincible>0){
                player.Invincible--;
            }
            if(freezeEnemys>0){
                freezeEnemys--;
            }
            timerClock = clock();
            timer.Sec++;
            if(timer.Sec>=60){
                timer.Sec = 0;
                timer.Min++;
            }
            if(timer.Min>=60){
                timer.Min = 0;
                timer.Hour++;
            }
            if(timer.Hour>=60){
                return 0;
            }
        }
    }
}
//------------------------------< TIMER <------------------------------//

//-------------------------> SISTAMA DOS MENUS >-------------------------//
int main(){
    SetConsoleOutputCP(CP_UTF8);

    bool running = true;
    int verticalMenu = 4;
    int codex = 4;

    while (running){
        bool menuCodex = true;

        cout << "\e[?25l\e[H";
        new_line("┏","━","┓",15);
        new_line("┃   ","BOMBERMAN","   ┃",1);
        new_line("┣","━","┫",15);
        if(verticalMenu == 4) {
            cout << "┃  \e[93m[Jogar]      \e[0m┃\n";
        } else
            cout << "┃ [Jogar]       ┃\n";

        if(verticalMenu== 5) {
            cout << "┃  \e[93m[Codex]      \e[0m┃\n";
        } else
            cout << "┃ [Codex]       ┃\n";

        if(verticalMenu== 6) {
            cout << "┃  \e[93m[Créditos]   \e[0m┃\n";
        } else
            cout << "┃ [Créditos]    ┃\n";

        if(verticalMenu== 7) {
            cout << "┃  \e[93m[Sair]       \e[0m┃\n";
        } else {
            cout << "┃ [Sair]        ┃\n";
        }
        new_line("┗","━","┛",15);
        int key = getch();
        switch(key){
            case 119: // Ir para cima
                verticalMenu--;
                if (verticalMenu < 4) {
                    verticalMenu = 7;
                }
                break;
            case 115: // Ir para baixo
                    verticalMenu++;
                    if (verticalMenu > 7) {
                        verticalMenu = 4;
                    }
                    break;
            case 13: // Input (ENTER)
                if(verticalMenu==4){
                    bool success = false;
                    int difficulty = 2;
                    int players = 2;
                    int gameMenu = 1;
                    int phase = 1;
                    int playerTotalMoves = 0;
                    int playerTotalbombs = 0;
                    int playerPoints = 0;
                    TimerType timer = {0,0,0};
                    while(true){
                        bool kill = false;
                        cout << "\e[?25l\e[1;18H";
                        new_line("┏","━","┓",24);
                        cout << "\e[2;18H";
                        if(gameMenu == 1) {
                            if(difficulty==1){
                                cout << "┃\e[93m  [Dificuldade Facil]   \e[0m┃\n";
                            }
                            if(difficulty==2){
                                cout << "┃\e[93m  [Dificuldade Medía]   \e[0m┃\n";
                            }
                            if(difficulty==3){
                                cout << "┃\e[93m  [Dificuldade Dificil] \e[0m┃\n";
                            }
                        } else {
                            if(difficulty==1){
                                cout << "┃ [Dificuldade Facil]    ┃\n";
                            }
                            if(difficulty==2){
                                cout << "┃ [Dificuldade Medía]    ┃\n";
                            }
                            if(difficulty==3){
                                cout << "┃ [Dificuldade Dificil]  ┃\n";
                            }
                        }
                        cout << "\e[3;18H";
                        if(gameMenu == 2) {
                            if(players==1){
                                cout << "┃\e[93m  [CPU Player]          \e[0m┃\n";
                            }
                            if(players==2){
                                cout << "┃\e[93m  [Um Player]           \e[0m┃\n";
                            }
                            if(players==3){
                                cout << "┃\e[93m  [Dois Players]        \e[0m┃\n";
                            }
                        } else {
                            if(players==1){
                                cout << "┃ [CPU Player]           ┃\n";
                            }
                            if(players==2){
                                cout << "┃ [Um Player]            ┃\n";
                            }
                            if(players==3){
                                cout << "┃ [Dois Players]         ┃\n";
                            }
                        }
                        cout << "\e[4;18H";
                        new_line("┃"," ","┃",24);
                        cout << "\e[5;18H";
                        if(gameMenu == 3) {
                            cout << "┃\e[93m  [JOGAR!]              \e[0m┃\n";
                        } else {
                            cout << "┃ [JOGAR!]               ┃\n";
                        }
                        cout << "\e[6;18H";
                        new_line("┗","━","┛",24);
                        int key = getch();
                        switch(key){
                            case 119: // Ir para cima
                                gameMenu--;
                                if (gameMenu < 1) {
                                    gameMenu = 3;
                                }
                            break;
                            case 115: // Ir para baixo
                                gameMenu++;
                                if (gameMenu > 3) {
                                    gameMenu = 1;
                                }
                            break;
                            case 97: // Esquerda
                                if(gameMenu==1){
                                    difficulty--;
                                    if(difficulty < 1){
                                        difficulty = 3;
                                    }
                                }
                                if(gameMenu==2){
                                    players--;
                                    if(players < 1){
                                        players = 3;
                                    }
                                }
                            break;
                            case 100: // Direira
                                if(gameMenu==1){
                                    difficulty++;
                                    if(difficulty > 3){
                                        difficulty = 1;
                                    }
                                }
                                if(gameMenu==2){
                                    players++;
                                    if(players > 3){
                                        players = 1;
                                    }
                                }
                            break;
                            case 13:
                                if(gameMenu==3){
                                    success = true;
                                }
                            break;
                            case 27:
                                cout << "\ec";
                                success = false;
                                kill = true;
                            break;
                        }
                        if(kill)break;
                        if(success)break;
                    }
                    while(success){
                        cout<<"\ec\e[?25l";
                        int deadMenu = -1;
                        if(game(difficulty,players,timer,phase,playerTotalMoves,playerTotalbombs,playerPoints)){
                            wait(100);
                            if(phase>3){
                                deadMenu = 1;
                                cout<<"\e[13;"<<mapSizeX+3<<"H";
                                cout << "┃ PLAYER VENCEU!     ┃\n";
                            }
                        }else{
                            wait(100);
                            cout<<"\e[13;"<<mapSizeX+3<<"H";
                            cout << "┃ PLAYER PERDEU!     ┃\n";
                            deadMenu = 1;
                            timer = {0,0,0};
                            phase = 1;
                            playerTotalMoves = 0;
                            playerTotalbombs = 0;
                            playerPoints = 0;
                        }
                        cout<<"\e[12;"<<mapSizeX+3<<"H";
                        new_line("┏","━","┓",20);
                        cout<<"\e[14;"<<mapSizeX+3<<"H";
                        new_line("┃"," ","┃",20);

                        if(deadMenu==1){
                            while(true){
                                bool kill = false;
                                cout<<"\e[15;"<<mapSizeX+3<<"H";
                                if(deadMenu==1){
                                    cout<<"┃\e[93m  [REINICIAR]       \e[0m┃\n";
                                }else{
                                    cout<<"┃ [REINICIAR]        ┃\n";
                                }
                                cout<<"\e[16;"<<mapSizeX+3<<"H";
                                if(deadMenu==2){
                                    cout<<"┃\e[93m  [MENU]            \e[0m┃\n";
                                }else{
                                    cout<<"┃ [MENU]             ┃\n";
                                }
                                cout<<"\e[17;"<<mapSizeX+3<<"H";
                                new_line("┗","━","┛",20);
                                int key = getch();
                                switch(key){
                                    case 119: // Ir para cima
                                        deadMenu--;
                                        if (deadMenu < 1) {
                                            deadMenu = 2;
                                        }
                                    break;
                                    case 115: // Ir para baixo
                                        deadMenu++;
                                        if (deadMenu > 2) {
                                            deadMenu = 1;
                                        }
                                    break;
                                    case 13:
                                        kill = true;
                                        if(deadMenu==2){
                                            success = false;
                                        }
                                        cout<<"\ec";
                                    break;
                                }
                                if(kill)break;
                            }
                        }
                    }
                }
                if(verticalMenu==5){
                    while(menuCodex){
                        bool paginaItens = true;
                        int paginaVertical = 1;
                        int paginaHorizontal = 1;

                        cout << "\e[?25l"; // Deixa o cursor invisivel
                        if (codex > 8) {
                            codex = 4;
                        }
                        if (codex < 4) {
                            codex = 8;
                        }
                        cout << "\e[?25l\e[H";
                        new_line("┏","━","┓",15);
                        cout << "┃     CODEX     ┃\n";
                        new_line("┣","━","┫",15);

                        if (codex == 4) {
                            cout << "┃  \e[93m[Controles]  \e[0m┃\n";
                        } else {
                            cout << "┃ [Controles]   ┃\n";
                        }

                        if (codex == 5) {
                            cout << "┃  \e[93m[Como jogar] \e[0m┃\n";
                        } else {
                            cout << "┃ [Como jogar]  ┃\n";
                        }

                        if (codex == 6) {
                            cout << "┃  \e[93m[Itens]      \e[0m┃\n";
                        } else {
                            cout << "┃ [Itens]       ┃\n";
                        }

                        if (codex == 7) {
                            cout << "┃  \e[93m[Rank]       \e[0m┃\n";
                        } else {
                            cout << "┃ [Rank]        ┃\n";
                        }

                        if (codex == 8) {
                            cout << "┃  \e[93m[Menu]       \e[0m┃\n";
                        } else {
                            cout << "┃ [Menu]        ┃\n";
                        }
                        new_line("┗","━","┛",15);

                        int key = getch();

                        switch (key){
                        case 72: case 'w': case 'W': // Ir para cima
                            codex--;
                        break;
                        case 80: case 's': case 'S': // Ir para baixo
                            codex++;
                        break;
                        case 13: // Input (ENTER)
                            switch(codex){
                                case 4:
                                    new_line("┏","━","┓",46);
                                    cout << "┃ Controles                                    ┃\n";
                                    cout << "┃                                              ┃\n";
                                    cout << "┃                Cima                          ┃\n";
                                    cout << "┃                 ^                            ┃\n";
                                    cout << "┃                 W                            ┃\n";
                                    cout << "┃    Esquerda < A S D > Direita                ┃\n";
                                    cout << "┃                 V                            ┃\n";
                                    cout << "┃               Baixo                          ┃\n";
                                    cout << "┃                                              ┃\n";
                                    new_line("┣","━","┫",46);
                                    cout << "┃ ( Espace ) -> Impaltar explosivo             ┃\n";
                                    new_line("┗","━","┛",46);
                                    getch();
                                    cout << "\ec";
                                break;

                                case 5:
                                    new_line("┏","━","┓",80);
                                    cout << "┃ - Objetivo: Exploda todos os inimigos da fase para ganhar, ou encontre a       ┃\n";
                                    cout << "┃    saída secreta.                                                              ┃\n";
                                    cout << "┃ - Você só tem uma vida, colete Auto-Desfibriladores para ganhar vidas extras.  ┃\n";
                                    cout << "┃ - Você pode só pode colocar um explosivo por vez, colete Multi-Detonares para  ┃\n";
                                    cout << "┃    poder colocar mais explosivos.                                              ┃\n";
                                    cout << "┃ - A explosão das bombas normais e C4 tem um formato de cruz (+), com um        ┃\n";
                                    cout << "┃    alcançe de 1x1.                                                             ┃\n";
                                    cout << "┃ - Ao colocar uma explosivo, não dá para passar por ela.                        ┃\n";
                                    cout << "┃ - As bombas conseguem destruir paredes frageis, mas não as resistentes.        ┃\n";
                                    cout << "┃ - PONTUAÇÃO: Você começa com 0 pontos       ┃\n";
                                    new_line("┗","━","┛",80);
                                    getch();
                                    cout << "\ec";
                                break;

                                case 6:
                                    cout << "\ec";
                                    while (paginaItens) {
                                        if (paginaVertical < 1) {
                                            paginaVertical = 2;
                                        }
                                        if (paginaVertical > 2) {
                                            paginaVertical = 1;
                                        }
                                        if (paginaHorizontal < 1) {
                                            paginaHorizontal = 3;
                                        }
                                        if (paginaHorizontal > 3) {
                                            paginaHorizontal = 1;
                                        }
                                        cout << "\e[?25l\e[H";
                                        new_line("┏","━","┓",26);
                                        cout << "┃       CODEX: ITENS       ┃\n";
                                        new_line("┣","━","┫",26);
                                        if (paginaVertical == 1) {
                                            if (paginaHorizontal == 1) {
                                                cout << "┃\e[93m  [Página 1 - Explosivos] \e[0m┃\n";
                                            }
                                            if (paginaHorizontal == 2) {
                                                cout << "┃\e[93m  [Página 2 - Trajes]     \e[0m┃\n";
                                            }
                                            if (paginaHorizontal == 3) {
                                                cout << "┃\e[93m  [Página 3 - Passivos]   \e[0m┃\n";
                                            }
                                            cout << "┃                          ┃\n";
                                        } else {
                                            if (paginaHorizontal == 1) {
                                                cout << "┃ [Página 1 - Explosivos]  ┃\n";
                                            }
                                            if (paginaHorizontal == 2) {
                                                cout << "┃ [Página 2 - Trajes]      ┃\n";
                                            }
                                            if (paginaHorizontal == 3) {
                                                cout << "┃ [Página 3 - Passivos]    ┃\n";
                                            }
                                            cout << "┃                          ┃\n";
                                        }
                                        if (paginaVertical == 2) {
                                            cout << "┃\e[93m  [Voltar]                \e[0m┃\n";
                                        } else {
                                            cout << "┃ [Voltar]                 ┃\n";
                                        }
                                        new_line("┗","━","┛",26);
                                        cout<<"\e[?25l";
                                        switch (paginaHorizontal) {
                                            case 1:
                                                    new_line("┏","━","┓",109);
                                                    cout << "┃                                      PÁGINA 1 - EXPLOSIVOS                                                  ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Bomba padrão (◉): A bomba padrão é o seu explosivo básico, tem uma quantidade infinita no seu inventário. ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - C4 (▣): Um explosivo que pode ser detonado remotamente (sem time de detonação), uma caixa de C4 concede   ┃\n";
                                                    cout << "┃    2 unidades, e podem ser implantadas ao mesmo tempo. Pressionar (R) vai detonar as C4. O raio de          ┃\n";
                                                    cout << "┃    explosão é igual da bomba padrão.                                                                        ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Claymore (ᛟ): Um explosivo anti-pessoal, se detona quando alguma entidade (jogador ou inimigo) passa por  ┃\n";
                                                    cout << "┃   cima dela. A explosão afeta apenas a célula onde foi implantada. Um pacote de Claymores concede 2         ┃\n";
                                                    cout << "┃   unidades.                                                                                                 ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Protótipo Bomba 'ÔM3GA' (Ω): Um explosivo com um grande poder de destruição, a explosão da 'ÔM3GA' irá    ┃\n";
                                                    cout << "┃   se extender por todos os lados até chegar na borda do mapa, destruindo tudo pelo caminho: Paredes         ┃\n";
                                                    cout << "┃   fragéis, inimigos, jogadores. Nada para ela.                                                              ┃\n";
                                                    new_line("┗","━","┛",109);
                                            break;

                                            case 2:
                                                    new_line("┏","━","┓",109);
                                                    cout << "┃                                      PÁGINA 2 - TRAJES                                                      ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Traje blindado 'Aegis' ( ): Um traje blindando que protege o jogador de qualquer dano (seja de inimigos   ┃\n";
                                                    cout << "┃   ou de explosões). O traje se quebra ao receber dano, mas, evita que o jogador morra. Ao se quebrar, o     ┃\n";
                                                    cout << "┃   jogador fica invuneravel por um curto período de tempo.                                                   ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Prótitpo de traje 'Espectro' (⬘): Um traje experimental que permite o jogador atravesar paredes frágies   ┃\n";
                                                    cout << "┃   por 60 segundos. Após esse tempo, o traje se quebra. Não é possível aumentar o tempo do contador.         ┃\n";
                                                    new_line("┗","━","┛",109);
                                            break;

                                            case 3:
                                                    new_line("┏","━","┓",109);
                                                    cout << "┃                                      PÁGINA 3 - PASSIVOS                                                    ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Polvora (▲): Aumenta o raio de explosão do próximo explosivo em 3 células, em todas as direções. É        ┃\n";
                                                    cout << "┃    consumido ao ser usado.                                                                                  ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Multi-detonador ( ): Permite colocar uma segunda bomba padrão em seguida. É consumida ao ser usada.       ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Martelo pneumatico ( ): Quebra as paredes frágeis ao ficar próxima de alguma.                             ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Prótotipo 'Crono-Hourglass' (⧗): Uma ampulheta capaz de congelar todos os inimigos ao chegar perto de     ┃\n";
                                                    cout << "┃   um inimigo.                                                                                               ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Prótitpo Manoplas 'Punho de Atlas' ( ): Manoplas sismicas que, quando o usuario soca o chão, ergue uma    ┃\n";
                                                    cout << "┃   rocha do subsollo na posição atual. Essa rocha possui as mesmas propriedades de uma parede frágil.        ┃\n";                                                                                              \
                                                    new_line("┗","━","┛",109);
                                            break;

                                            case 4:
                                                cout << "\ec";
                                                paginaHorizontal = 1;
                                            break;
                                        }
                                        key = getch();
                                        switch (key) {
                                            case 119: // Cima
                                                paginaVertical--;
                                            break;

                                            case 115: //Baixo
                                                paginaVertical++;
                                            break;

                                            case 97: //Esquerda
                                                paginaHorizontal--;
                                                cout << "\ec";
                                            break;

                                            case 100: //Direita
                                                paginaHorizontal++;
                                                cout << "\ec";
                                            break;

                                            case 13:
                                                if(paginaVertical==2){
                                                    cout << "\ec";
                                                    paginaItens = false;
                                                }
                                            break;
                                        }
                                    }
                                break;

                                case 7:

                                    cout << "\nTemplate de Rank.";
                                    getch();
                                    cout << "\ec";

                                break;

                                case 8:
                                    cout << "\ec";
                                    codex = 4;
                                    menuCodex = false;
                                break;
                            }
                    }
                }
            }
            if(verticalMenu==6){
                cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓";
                cout << "\n┃       Universidade do Vale do Itajaí - UNIVALI     ┃";
                cout << "\n┃              Escola Politícnica  - POLI            ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Curso: Ciência da Computação                       ┃";
                cout << "\n┃ Disciplina: Algoritmos e Programação II            ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Equipe de Desenvolvimento:                         ┃";
                cout << "\n┃  Alunos:                                           ┃";
                cout << "\n┃   - Adolfo Schaeffer.                              ┃";
                cout << "\n┃   - Douglas Vilar.                                 ┃";
                cout << "\n┃   - Gustavo Alves.                                 ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Professor: Alex Rese.                              ┃";
                cout << "\n┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫";
                cout << "\n┃ Local/Data da produção: Itajaí-SC, Março/2026.     ┃";
                cout << "\n┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛";
                getch();
                cout << "\ec";
            }
            if(verticalMenu==7){
                cout << "\ec";
                running = false;
            }
        }
    }
    return 0;
}
//-------------------------< SISTAMA DOS MENUS <-------------------------//
