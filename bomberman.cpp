#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>

using namespace std;

#define freeArea 0
#define solidWall 1
#define fragileWall 2
#define bomb 3
#define explosion 4

struct PositionType{int X; int Y;};
struct BombType{int X;int Y;int Time;int Range;};
struct TimerType{int Sec;int Min; int Hour;};

const int mapSizeY = 15;
const int mapSizeX = 23;

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

bool verifier_explosion(int map[mapSizeY][mapSizeX], BombType bombPos, int y, int x){
    int explosive[4] = {freeArea, bomb, fragileWall, explosion};
    for(int i = 0; i < 4; i++){
        if (map[bombPos.Y+y][bombPos.X+x] == explosive[i]){
            map[bombPos.Y+y][bombPos.X+x] = explosion;
            return true;
            break;
        }
    }
    return false;
}

void new_line(string x, string y, string z,int size){
    cout<<x;
    for(int i = 0; i<size; i++){
        cout<<y;
    }
    cout<<z<<endl;
}

void render_details(int passiveItem,TimerType timer, int itens[]){
    cout << "\e[2;"<<mapSizeX+12<<"H";
    if(timer.Sec<10){
        cout << 0;
    }
    cout << timer.Sec;
    cout << "\e[2;"<<mapSizeX+9<<"H";
    if(timer.Min<10){
        cout << 0;
    }
    cout << timer.Min;
    cout << "\e[2;"<<mapSizeX+6<<"H";
    if(timer.Hour<10){
        cout << 0;
    }
    cout << timer.Hour;
    
    cout<<"\e[6;"<<mapSizeX+4<<"H";
    for(int i=0;i<5;i++){

        if(itens[i]==0){
            cout<<" ";
        }
        if(itens[i]==1){
            cout<<"▲";
        }
        if(itens[i]==2){
            cout<<"◉";
        }
        if(itens[i]==3){
            cout<<"Ω";
        }
    }

    cout<<"\e[6;"<<mapSizeX+10<<"H";
    if(passiveItem==0){
        cout<<" ";
    }
    if(passiveItem==1){
        cout<<"☤";
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

int game(int difficulty, int players,TimerType &timer, int &phase){

//------------------------> VARIAVEIS GERAIS >------------------------//

    int mapSizeBy2 = ((mapSizeY/2)*(mapSizeX/2));
    int maximumBombs = mapSizeBy2/2;

    int itens[5] = {0,0,0,0,0};
    int passiveItem = 0;

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
    PositionType enemysPos[enemysQuantity];

    int fragileWallQuantity = rand()%(mapSizeBy2-enemysQuantity);
    fragileWallQuantity = (fragileWallQuantity<(mapSizeBy2-enemysQuantity)/2) ? (mapSizeBy2-enemysQuantity)/2 : fragileWallQuantity;
    PositionType boxPos[fragileWallQuantity];
    for (int i=0; i<fragileWallQuantity; i++){
        boxPos[i] = {-1,-1};
    }
    
    int cpuMoves = clock();
    
    PositionType playerPos;
    int invincible = 2;
    BombType bombsPos[maximumBombs];
    for(int i=0;i<maximumBombs;i++){
        bombsPos[i] = {-1,-1,0,1};
    }
    int actualBomb = 0;
    
    int timerClock = clock();

    int map[mapSizeY][mapSizeX];

//------------------------< VARIAVEIS GERAIS <------------------------//

//------------------------> GERAÇÃO DO MAPA >------------------------//
    srand(time(0));
    for (int y = 0; y < mapSizeY; y++) {
        for (int x = 0; x  <mapSizeX; x++) {
            if (x == 0 || y == 0) {
               map[y][x] = solidWall;
            } else if (x == mapSizeX - 1 || y == mapSizeY - 1) {
               map[y][x] = solidWall;
            } else {
               map[y][x] = freeArea;
            }
        }
    }
    if(phase!=3){
        for (int y = 0; y < mapSizeY; y += 2){
            for (int x = 0; x < mapSizeX; x += 2){
                if(map[y][x]==freeArea){
                    if(rand()%10!=0){
                        map[y][x] = solidWall;
                    }else{
                        map[y][x] = fragileWall;
                    }
                }
            }
        }
        for (int i=0; i < fragileWallQuantity; i++){
            PositionType fragil;
            while(true){
                fragil.Y = rand()%mapSizeY;
                fragil.X = rand()%mapSizeX;
                if (map[fragil.Y][fragil.X] == freeArea){
                    map[fragil.Y][fragil.X] = fragileWall;
                    if(rand()%5==0){
                        boxPos[i].Y=fragil.Y;
                        boxPos[i].X=fragil.X;
                    }
                    break;
                }
            }
        }
    }
    while(true){ // Gera a posição do jogador
        bool success = false;
        playerPos.Y = rand()%mapSizeY,
        playerPos.X = rand()%mapSizeX;
        if (map[playerPos.Y][playerPos.X] == freeArea){ // Verifica se a posição do playerPos é uma area livre, se for, ele faz uma verificação para não gerar o playerPos perto de paredes frageis
            success = true;
            for (int y = -1; y < 2; y++){
                for (int x = -1; x < 2; x++){ // Passa por uma matriz 3x3 em volta do playerPos
                    if(y==0 || x==0){ // Verifica se tem uma parede fragil em volta do playerPos, se tiver, ele gera outra posição
                        if (map[playerPos.Y+y][playerPos.X+x] == fragileWall){
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
    for (int  i = 0; i < enemysQuantity; i++){ // Gera as posições dos enemysPos
        PositionType enemy;   // Posição do inimigo
        while(true){ // Gera uma posição aleatoria para o inimigo e faz as verificações para não gerar o inimigo emcima do playerPos, de outros enemysPos ou em blocos proibidos
            bool success = false;
            enemy.Y = rand()%mapSizeY;
            enemy.X = rand()%mapSizeX;
            for (int enemys = 0; enemys < enemysQuantity; enemys++){
                if (enemy.X == enemysPos[enemys].X && enemy.Y == enemysPos[enemys].Y){
                    enemy = playerPos;
                }
            }
            if(enemy.X != playerPos.X && enemy.Y != playerPos.Y) {
                if(map[enemy.Y][enemy.X] == freeArea){
                    success = true;
                    for (int y = -1; y < 2; y++){
                        for (int x = -1; x < 2; x++){
                            if(y==0 || x==0){
                                if (map[enemy.Y+y][enemy.X+x] == fragileWall){
                                    success = false;
                                }                        
                            }
                        }
                    }
                    if(success){
                        enemysPos[i] = {enemy};
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
    cout << "┃"<<phase<<" 00:00:00";

    cout<<"\e[5;"<<mapSizeX+3<<"H";
    new_line("┏","━","┳",5);
    cout<<"\e[6;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",5);
    cout<<"\e[7;"<<mapSizeX+3<<"H";
    new_line("┗","━","┻",5);
    cout<<"\e[5;"<<mapSizeX+9<<"H";
    new_line("┳","━","┓",1);
    cout<<"\e[6;"<<mapSizeX+9<<"H";
    new_line("┃"," ","┃",1);
    cout<<"\e[7;"<<mapSizeX+9<<"H";
    new_line("┻","━","┛",1);
//------------------------< BORDAS LATERAIS <------------------------//

//---------------------------> RENDERIZAÇÃO DO MAPA >---------------------------//
    while(true){
        bool theresBomb = false;
        if(bombsPos[actualBomb].Y!=-1 && bombsPos[actualBomb].X!=-1){
            theresBomb = true;
        }
        if(actualBomb>maximumBombs){
            actualBomb = 0;
        }
        cout << "\e[?25l\e[1;1H";
        new_line("┏","━","┓",mapSizeX);
        for (int y = 0; y < mapSizeY; y++) {
            cout<<"┃";
            for (int x = 0; x < mapSizeX; x++) {
                 bool block = true;
                 if(block){
                    for(int enemys=0; enemys<enemysQuantity; enemys++) {
                        if(y==enemysPos[enemys].Y && x==enemysPos[enemys].X) {
                            block = false;
                            if(map[enemysPos[enemys].Y][enemysPos[enemys].X]==explosion) { // Testa se naquela posição tem um bloco de explosão
                                cout << "\e[31;43m\u25A1"; // INIMIGO MORTO NA EXPLOSÃO
                            }else{
                                if(playerPos.Y==enemysPos[enemys].Y && playerPos.X==enemysPos[enemys].X) {
                                    cout << "\e[31;42m\u25CB"; // PLAYER MORTO NO INIMIGO
                                    break;
                                }else{
                                    cout << "\e[31;42m\u25A0"; // INIMIGO
                                    break;
                                }
                            }
                        }
                    }
                }
                if(block){
                    if (playerPos.Y == y && playerPos.X == x) {
                        block = false;
                        if(map[playerPos.Y][playerPos.X] == explosion){
                            cout << "\e[97;43m\u25CB"; // PLAYER MORTO NA EXPLOSÃO
                        }else{
                            if(invincible==0){
                                cout << "\e[97;42m\u25CF"; // PLAYER
                            }else{
                                if(rand()%2){
                                    cout << "\e[0;42m\e[38;5;51m\u25CF";
                                }else{
                                    cout << "\e[0;42m\e[38;5;159m\u25CF";
                                }
                            }
                        }
                    }
                }
                if (block){
                    if(map[y][x] == explosion){
                        cout << "\e[33;43m "; // EXPLOSÃO
                        block = false;
                    }
                }
                if(block){
                    for(int box=0;box<fragileWallQuantity;box++){
                        if(y==boxPos[box].Y && x==boxPos[box].X){
                            if(map[y][x] == freeArea){
                                cout<<"\e[32;42m\e[38;5;94m\u25D9";
                                block = false;
                            }
                        }
                    }
                }
                if (block) {
                    if (map[y][x] == freeArea){
                        cout << "\e[32;42m "; // AREÁ LIVRE
                        continue;
                    }
                    if (map[y][x] == solidWall){
                        cout << "\e[48;5;243m "; // PAREDE SOLIDA
                        continue;
                    }
                    if (map[y][x] == fragileWall){
                        cout << "\e[48;5;245m "; // PAREDE FRAGIL
                        continue;
                    }
                    if (map[y][x] == bomb){
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
            cout << "\e[0m┃\n";
        }
        new_line("┗","━","┛",mapSizeX);
        render_details(passiveItem,timer,itens);
//---------------------------< RENDERIZAÇÃO DO MAPA <---------------------------//

//----------------------> SISTEMA DO PLAYER >----------------------//
        if(invincible==0){
            if (map[playerPos.Y][playerPos.X] == explosion){
                if(passiveItem==1){
                    passiveItem = 0;
                    invincible = 1;
                    continue;
                }else{
                    return 0;
                }
            }
            for (int enemys = 0; enemys < enemysQuantity; enemys++) {
                if(playerPos.Y == enemysPos[enemys].Y && playerPos.X == enemysPos[enemys].X){
                    if(passiveItem==1){
                        passiveItem = 0;
                        invincible = 1;
                        continue;
                    }else{
                        return 0;
                    }
                }
            }
        }
        for(int i=0;i<5;i++){
            if(i>0){
                if(itens[i]!=0){
                    if(itens[i-1]==0){
                        itens[i-1]=itens[i];
                        itens[i] = 0;
                    }
                }
            }
        }
        for(int box=0;box<fragileWallQuantity;box++){
            if(playerPos.Y==boxPos[box].Y && playerPos.X==boxPos[box].X){
                boxPos[box] = {-1,-1};
                if(rand()%10!=0){
                    for(int i=0;i<5;i++){
                        if(itens[i]==0){
                            bool success = false;
                            int item;
                            while(!success){
                                success = true;
                                item = rand()%3+1;
                                if(rand()%5!=0 && item==3){
                                    success = false;
                                }
                            }
                            itens[i] = item;
                            break;
                        }
                    }
                }else{
                    bool success = false;
                    while(!success){
                        int item = rand()%2+1;
                        if(item==1 && passiveItem!=0){
                            passiveItem=0;
                            success = true;
                        }
                        if(item==2){
                            passiveItem=1;
                            success = true;
                        }
                    }
                }
            }
        }
        
        if (kbhit() || players==1){
            PositionType target;
            target = {0,0};
            int key;
            
            if(players==2){
                key = getch();
            }
//----------------------> CPU PLAYER >----------------------//
            if(players==1){
                key = 0;
                int enemyAt[4] = {-1,-1,-1,-1};
                int danger[4] = {-1,-1,-1,-1};
                if((clock()-cpuMoves) >= 75){
                    cpuMoves = clock();
                    bool nearEnemy = false;
                    for(int y=-2;y<=2;y++){
                        for(int x=-2;x<=2;x++){
                            if(playerPos.Y+y>0 && playerPos.Y+y<mapSizeY && playerPos.X+x>0 && playerPos.X+x<mapSizeX){
                                if(((y>=-1&&x>=-1) && (y<=1&&x<=1)) || (y==0||x==0)){
                                    for(int enemys=0;enemys<enemysQuantity;enemys++){
                                        if(playerPos.Y+y == enemysPos[enemys].Y && playerPos.X+x == enemysPos[enemys].X){
                                            nearEnemy = true;
                                            found_danger(danger,y,x);
                                        }
                                    }
                                    if(map[playerPos.Y+y][playerPos.X+x]==bomb){
                                        found_danger(danger,y,x);
                                    }
                                    if(map[playerPos.Y][playerPos.X]==bomb){
                                        if(y==-2 && x==0){
                                            if(map[playerPos.Y+y][playerPos.X+x]==fragileWall || map[playerPos.Y+y][playerPos.X+x]==solidWall){
                                                int y1 = y+1;
                                                int x1 = x-1;
                                                if(map[playerPos.Y+y1][playerPos.X+x1]==fragileWall || map[playerPos.Y+y1][playerPos.X+x1]==solidWall){
                                                    int y2 = y1;
                                                    int x2 = x1+2;
                                                    if(map[playerPos.Y+y2][playerPos.X+x2]==fragileWall || map[playerPos.Y+y2][playerPos.X+x2]==solidWall){
                                                        danger[0] = 119;
                                                    }   
                                                }
                                            }
                                        }
                                        if(y==0 && x==2){
                                            if(map[playerPos.Y+y][playerPos.X+x]==fragileWall || map[playerPos.Y+y][playerPos.X+x]==solidWall){
                                                int y1 = y-1;
                                                int x1 = x-1;
                                                if(map[playerPos.Y+y1][playerPos.X+x1]==fragileWall || map[playerPos.Y+y1][playerPos.X+x1]==solidWall){
                                                    int y2 = y1+2;
                                                    int x2 = x1;
                                                    if(map[playerPos.Y+y2][playerPos.X+x2]==fragileWall || map[playerPos.Y+y2][playerPos.X+x2]==solidWall){
                                                        danger[3] = 100;
                                                        
                                                    }
                                                }
                                            }
                                        }
                                        if(y==2 && x==0){
                                            if(map[playerPos.Y+y][playerPos.X+x]==fragileWall || map[playerPos.Y+y][playerPos.X+x]==solidWall){
                                                int y1 = y-1;
                                                int x1 = x+1;
                                                if(map[playerPos.Y+y1][playerPos.X+x1]==fragileWall || map[playerPos.Y+y1][playerPos.X+x1]==solidWall){
                                                    int y2 = y1;
                                                    int x2 = x1-2;
                                                    if(map[playerPos.Y+y2][playerPos.X+x2]==fragileWall || map[playerPos.Y+y2][playerPos.X+x2]==solidWall){
                                                        danger[1] = 115;
                                                    }
                                                }
                                            }
                                        }
                                        if(y==0 && x==-2){
                                            if(map[playerPos.Y+y][playerPos.X+x]==fragileWall || map[playerPos.Y+y][playerPos.X+x]==solidWall){
                                                int y1 = y+1;
                                                int x1 = x+1;
                                                if(map[playerPos.Y+y1][playerPos.X+x1]==fragileWall || map[playerPos.Y+y1][playerPos.X+x1]==solidWall){
                                                    int y2 = y1-2;
                                                    int x2 = x1;
                                                    if(map[playerPos.Y+y2][playerPos.X+x2]==fragileWall || map[playerPos.Y+y2][playerPos.X+x2]==solidWall){
                                                        danger[2] = 97;
                                                    }
                                                }
                                            }
                                        }
                                    }else{
                                        enemyAt[0] = -1;
                                        enemyAt[1] = -1;
                                        enemyAt[2] = -1;
                                        enemyAt[3] = -1;
                                    }
                                }
                            }
                        }
                    }
                    for(int y=-1;y<=1;y++){
                        for(int x=-1;x<=1;x++){
                            if((y==0||x==0)){
                                if(map[playerPos.Y+y][playerPos.X+x]==explosion){
                                    found_danger(danger,y,x);
                                }
                                if(map[playerPos.Y+y][playerPos.X+x]==fragileWall || map[playerPos.Y+y][playerPos.X+x]==solidWall){
                                    found_danger(danger,y,x);
                                }
                            }
                        }
                    }
                    if(enemyAt[0]==119){
                        danger[0] = 119;
                    }
                    if(enemyAt[1]==115){
                        danger[1] = 115;
                    }
                    if(enemyAt[2]==97){
                        danger[2] = 97;
                    }
                    if(enemyAt[3]==100){
                        danger[3] = 100;
                    }
                    int safes = 0;
                    for(int i=0;i<4;i++){
                        safes = (danger[i]==-1) ? safes+=1 : safes+=0;
                    }
                    if(rand()%4>0 && !nearEnemy && !theresBomb){
                        int enemyChoiced = -1;
                        for(int enemys=0;enemys<enemysQuantity;enemys++){
                            if(enemysPos[enemys].X==-1 && enemysPos[enemys].Y==-1){
                                continue;
                            }
                            if(enemysPos[enemys].X!=-1 && enemysPos[enemys].Y!=-1){
                                if(enemyChoiced==-1){
                                    enemyChoiced = enemys;
                                }else{
                                    if((VA(playerPos.Y-enemysPos[enemys].Y) < VA(playerPos.Y-enemysPos[enemyChoiced].Y)) && (VA(playerPos.X-enemysPos[enemys].X) < VA(playerPos.X-enemysPos[enemyChoiced].X))){
                                        enemyChoiced = enemys;
                                    }
                                }
                            }
                        }
                        int x = playerPos.X-enemysPos[enemyChoiced].X;
                        int y = playerPos.Y-enemysPos[enemyChoiced].Y;
                        if(y>0){
                            danger[1] = 115;
                        }
                        if(y<0){
                            danger[0] = 119;
                        }
                        if(x>0){
                            danger[3] = 100;
                        }
                        if(x<0){
                            danger[2] = 97;
                        }
                    }
                    for(int tryIt=0;tryIt<11;tryIt++){
                        if(tryIt==10){
                            key = 0;
                            break;
                        }
                        bool success = true;
                        key = rand()%4;
                        if(key==0){
                            key=119;
                        }
                        if(key==1){
                            key=115;
                        }
                        if(key==2){
                            key=97;
                        }
                        if(key==3){
                            key=100;
                        }
                        for(int i=0;i<4;i++){
                            if(key==danger[i]){
                                success = false;
                            }
                        }
                        if(success){
                            break;
                        }
                    }
                    for(int y=-2;y<=2;y++){
                        for(int x=-2;x<=2;x++){ 
                            if(playerPos.Y+y>0 && playerPos.Y+y<mapSizeY && playerPos.X+x>0 && playerPos.X+x<mapSizeX){
                                if((y==0||x==0) && rand()%2==0 && safes>=2){
                                    for(int enemys=0;enemys<enemysQuantity;enemys++){
                                        if(playerPos.Y+y == enemysPos[enemys].Y && playerPos.X+x == enemysPos[enemys].X){
                                            if(y==-2){
                                                y++;
                                                if(map[playerPos.Y+y][playerPos.X+x]==freeArea){
                                                    enemyAt[0] = 119;
                                                    key = 32;
                                                }
                                            }
                                            if(y==2){
                                                y--;
                                                if(map[playerPos.Y+y][playerPos.X+x]==freeArea){
                                                    enemyAt[1] = 115;
                                                    key = 32;
                                                }
                                            }
                                            if(x==-2){
                                                x++;
                                                if(map[playerPos.Y+y][playerPos.X+x]==freeArea){
                                                    enemyAt[2] = 97;
                                                    key = 32;
                                                }
                                            }
                                            if(x==2){
                                                x--;
                                                if(map[playerPos.Y+y][playerPos.X+x]==freeArea){
                                                    enemyAt[3] = 100;
                                                    key = 32;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    for(int y=-1;y<=1;y++){
                        for(int x=-1;x<=1;x++){
                            if((y==0||x==0) && (map[playerPos.Y+y][playerPos.X+x]==fragileWall) && (rand()%4==0)){
                                key = 32;
                            }
                        }
                    }
                }
            }
//----------------------< CPU PLAYER <----------------------//
            switch(key){
                case 119: // Cima
                    target.Y--;
                    break;
                case 115: // Baixo
                    target.Y++;
                    break;
                case 97: // Esquerda
                    target.X--;
                    break;
                case 100: // Direira
                    target.X++;
                    break;
            }
            playerPos.Y += target.Y;
            playerPos.X += target.X; // Torna a posição desejada a nova posição do playerPos
            if (map[playerPos.Y][playerPos.X] != freeArea && map[playerPos.Y][playerPos.X] != explosion) { // Testa colisão e cancela o movimento se verdadeiro
                playerPos.Y -= target.Y;
                playerPos.X -= target.X;
            }
            if(key == 32 && (map[playerPos.Y][playerPos.X]==freeArea) && invincible==0){
                if(itens[0]==2 && theresBomb){
                    actualBomb++;
                    itens[0] = 0;
                }
                if (bombsPos[actualBomb].Y == -1 && bombsPos[actualBomb].X == -1){
                    if(itens[0]==1){
                        bombsPos[actualBomb].Range=2;
                        itens[0] = 0;
                    }
                    if(itens[0]==3){
                        bombsPos[actualBomb].Range=(mapSizeX+mapSizeY);
                        itens[0] = 0;
                    }
                    bombsPos[actualBomb].Y = playerPos.Y;
                    bombsPos[actualBomb].X = playerPos.X;
                    bombsPos[actualBomb].Time = clock();
                }
            }
        }
//---------------------< SISTEMA DO PLAYER <----------------------//

//----------------------------> SISTEMA DOS INIMIGOS >----------------------------//
        if((clock()-moveEnemy) >= 350){
            int actualEnemys = 0;
            moveEnemy = clock();
            for(int enemys = 0; enemys<enemysQuantity; enemys++){
                if(enemysPos[enemys].Y == -1 && enemysPos[enemys].X == -1){
                    if (enemys == enemysQuantity-1) {
                        if (actualEnemys == 0){
                            phase++;
                            return 1;
                            break;
                        }
                    }
                    continue;
                }
                actualEnemys++;
                int enemyTarget = rand()%4+1;
                int forbidden[4] = {solidWall, fragileWall, bomb, explosion};
                for(int box=0;box<fragileWallQuantity;box++){
                    if(enemysPos[enemys].Y == boxPos[box].Y && enemysPos[enemys].X == boxPos[box].X){
                        boxPos[box].Y = -1;
                        boxPos[box].X = -1;
                    }
                }
                for( int toTry = 0; toTry < 4; toTry++) {
                    bool success = true;
                    int Y = 0, X = 0;
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
                    if(toTry>0){
                        enemyTarget = rand()%4+1;
                    }
                    if(enemyTarget==0){
                        int y = enemysPos[enemys].Y-playerPos.Y;
                        int x = enemysPos[enemys].X-playerPos.X;
                        if(rand()%2==0){
                            if(y<0){
                                Y++;
                            }
                            if(y>0){
                                Y--;
                            }
                        }else{
                            if(x<0){
                                X++;
                            }
                            if(x>0){
                                X--;
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
                    if(map[enemysPos[enemys].Y][enemysPos[enemys].X] == explosion){
                        break;
                    }
                    enemysPos[enemys].Y += Y;
                    enemysPos[enemys].X += X;
                    for (int i = 0; i < 4; i++){
                        if (map[enemysPos[enemys].Y][enemysPos[enemys].X] == forbidden[i]) {
                            success = false;
                            break;
                        }
                    }
                    for (int otherEnemy = 0; otherEnemy < enemysQuantity; otherEnemy++) {
                        if (otherEnemy == enemys) {
                            continue;
                        }
                        if (enemysPos[enemys].Y == enemysPos[otherEnemy].Y && enemysPos[enemys].X == enemysPos[otherEnemy].X){
                            success = false;
                            break;
                        }
                    }
                    if (success) {
                        break;
                    } else {
                        enemysPos[enemys].Y -= Y;enemysPos[enemys].X -= X;
                        enemyTarget = rand()%4+1;
                        continue;
                    }
                }
            }
        }
//----------------------------< SISTEMA DOS INIMIGOS <----------------------------//

//-----------------------------------> SISTEMA DAS BOMBAS >-----------------------------------//
        for(int i=0;i<maximumBombs;i++){
            if(map[bombsPos[i].Y][bombsPos[i].X]==freeArea){
                map[bombsPos[i].Y][bombsPos[i].X] = bomb;
            }
            if(bombsPos[i].Y>0 && bombsPos[i].X>0 && ((clock()-bombsPos[i].Time) >= 1000)){
                for(int y = 0; y <= bombsPos[i].Range; y++){
                    bool success = verifier_explosion(map,bombsPos[i],y,0);
                    if(!success){break;}
                }
                for(int x = 0; x <= bombsPos[i].Range; x++){
                    bool success = verifier_explosion(map,bombsPos[i],0,x);
                    if(!success){break;}
                }
                for(int y = 0; y >= bombsPos[i].Range*-1; y--){
                    bool success = verifier_explosion(map,bombsPos[i],y,0);
                    if(!success){break;}
                }
                for(int x = 0; x >= bombsPos[i].Range*-1; x--){
                    bool success = verifier_explosion(map,bombsPos[i],0,x);
                    if(!success){break;}
                }
                actualBomb++;
            }
            if(bombsPos[i].Y>0 && bombsPos[i].X>0 && ((clock()-bombsPos[i].Time) >= 1350)){
                for(int enemys = 0; enemys<enemysQuantity; enemys++){
                    if (map[enemysPos[enemys].Y][enemysPos[enemys].X] == explosion) {
                        enemysPos[enemys].Y = -1;
                        enemysPos[enemys].X = -1;
                    }
                }
                for(int y = 0; y < mapSizeY; y++) {
                    for (int x = 0; x < mapSizeX; x++) {
                        if (map[y][x] == explosion) {
                            map[y][x] = freeArea;
                        }
                    }
                }
                bombsPos[i] = {-1,-1,0,1};
            }
        }
//-----------------------------------< SISTEMA DAS BOMBAS <-----------------------------------//

//------------------------------> TIMER >------------------------------//
        if((clock()-timerClock)>=1000){
            if(invincible>0){
                invincible--;
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
    int codexPaginas = 2;

    while (running){
        bool menuCodex = true;
        bool codexItens = true;

        cout << "\e[?25l\e[H";
        new_line("┏","━","┓",15);
        cout << "┃   BOMBERMAN   ┃\n";
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
                        if(game(difficulty,players,timer,phase)){
                            wait(100);
                            if(phase>3){
                                deadMenu = 1;
                                cout<<"\e[9;"<<mapSizeX+3<<"H";
                                cout << "┃ PLAYER VENCEU!     ┃\n";
                            }
                        }else{
                            wait(100);
                            cout<<"\e[9;"<<mapSizeX+3<<"H";
                            cout << "┃ PLAYER PERDEU!     ┃\n";
                            deadMenu = 1;
                            timer = {0,0,0};
                            phase = 1;
                        }
                        cout<<"\e[8;"<<mapSizeX+3<<"H";
                        new_line("┏","━","┓",20);
                        cout<<"\e[10;"<<mapSizeX+3<<"H";
                        new_line("┃"," ","┃",20);
                        
                        if(deadMenu==1){
                            while(true){
                                bool kill = false;
                                cout<<"\e[11;"<<mapSizeX+3<<"H";
                                if(deadMenu==1){
                                    cout<<"┃\e[93m  [REINICIAR]       \e[0m┃\n";
                                }else{
                                    cout<<"┃ [REINICIAR]        ┃\n";
                                }
                                cout<<"\e[12;"<<mapSizeX+3<<"H";
                                if(deadMenu==2){
                                    cout<<"┃\e[93m  [MENU]            \e[0m┃\n";
                                }else{
                                    cout<<"┃ [MENU]             ┃\n";
                                }
                                cout<<"\e[13;"<<mapSizeX+3<<"H";
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
                        cout << "\e[?25l"; // Deixa o cursor invisivel
                        if (codex > 7) {
                            codex = 4;
                        }
                        if (codex < 4) {
                            codex = 7;
                        }
                        cout << "\e[1;1H"; // Posiciona o cursor do console na primeira linha ( \e[{linha};{coluna}H ) <-- IMPORTANTE
                        new_line("┏","━","┓",15);
                        cout << "┃     CODEX     ┃\n";
                        new_line("┣","━","┫",15);

                        if (codex == 4) {
                            cout << "┃ \e[93m[Controles]   \e[0m┃\n";
                        } else {
                            cout << "┃ [Controles]   ┃\n";
                        }

                        if (codex == 5) {
                            cout << "┃ \e[93m[Como jogar]  \e[0m┃\n";
                        } else {
                            cout << "┃ [Como jogar]  ┃\n";
                        }

                        if (codex == 6) {
                            cout << "┃ \e[93m[Itens]       \e[0m┃\n";
                        } else {
                            cout << "┃ [Itens]       ┃\n";
                        }

                        if (codex == 7) {
                            cout << "┃ \e[93m[Menu]        \e[0m┃\n";
                        } else {
                            cout << "┃ [Menu]        ┃\n";
                        }
                        new_line("┗","━","┛",15);

                        cout << "\e[" << codex << ";2H"; // Posiciona o cursor do console para a posição da seleção
                        cout << "\e[93m>\e[0m";

                        key = getch();

                        switch(key){
                            case 72: case 'w': case 'W': // Ir para cima
                            codex--;
                        break;
                        case 80: case 's': case 'S': // Ir para baixo
                            codex++;
                        break;
                        case 13: // Input (ENTER)
                            cout<<"\ec";
                            cout<<"\e[?25l";
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
                                    cout << "┃ ( Espace ) -> Bomba normal                   ┃\n";
                                    cout << "┃ ( C )-> C4  /  R -> Detona as cargas de C4   ┃\n";
                                    cout << "┃ ( P ) -> Bomba Pandora                       ┃\n";
                                    cout << "┃ ( O ) -> Bomba Ômega                         ┃\n";
                                    cout << "┃ ( T ) -> Claymore                            ┃\n";
                                    new_line("┗","━","┛",46);
                                break;
                                case 5:
                                    new_line("┏","━","┓",80);
                                    cout << "┃ - Objetivo: Exploda todos os inimigos da fase                                  ┃\n";
                                    cout << "┃    para ganhar, ou encontre a saída secreta.                                   ┃\n";
                                    cout << "┃ - Você só tem uma vida, colete Auto-Desfibriladores para ganhar vidas extras.  ┃\n";
                                    cout << "┃ - Você pode só pode colocar uma bomba por vez,                                 ┃\n";
                                    cout << "┃    colete Multi-Detonadores para poder colocar mais explosivos.                ┃\n";
                                    cout << "┃ - A explosão das bombas normais e C4 tem um                                    ┃\n";
                                    cout << "┃    formato de cruz ( + ), com um alcançe de 1x1.                               ┃\n";
                                    cout << "┃ - Colete Polvora Melhorada para aumentar o raio de explosão dos explosivos.    ┃\n";
                                    cout << "┃ - Ao colocar uma bomba, não dá para passar por ela.                            ┃\n";
                                    cout << "┃ - As bombas conseguem destruir paredes frageis, mas não as resistentes.        ┃\n";
                                    new_line("┗","━","┛",80);
                                break;
                                case 6:

                                break;
                                case 7:
                                    codex = 4;
                                    menuCodex = false;
                                break;
                            }
                            if(menuCodex){
                                cout << "\n\nAperte qualquer tecla para voltar ao MENU DO CODEX...";
                                getch();
                                cout << "\ec";
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
            break;
        }
    }
    return 0;
}
//-------------------------< SISTAMA DOS MENUS <-------------------------//