#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

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

struct BossType{
    XY Pos;
    XY Target;
    int Move;
    int HP;
    int MaxHP;
    bool Alive = false;
    int Clock = clock();
};

struct PlayerType{
    XY Pos;
    int MaxBombs = 1;
    int MaxRange = 1;
    int Lifes = 1;
    int ActualBomb = 0;
    int Invincible = 3;
    int Item = 0;
    int Slot = 0;
    int TotalMoves = 0;
    int Totalbombs = 0;
    int Points = 1000;
    bool Alive = true;
};

struct TimerType{
    int Sec;
    int Min;
    int Hour;
};

struct InfoType{
    string name;
    int difficulty = 2;
    int players = 2;
    int phase = 1;
    int maxPoints = 0;
    PlayerType player1;
    PlayerType player2;
    TimerType timer = {0,0,0};
};

const int mapSizeY = 15;
const int mapSizeX = 25;

int VA(int number){
    if(number<0){
        number*=-1;
    }
    return number;
}

template<typename number> 
int swaper(number x){
    static int i = 0;
    if(i==1){
        i = 0;
    }else{
        i++;
    }
    return i;
}

void mostrar_pontos(){
    int i = 1;
    int j = 1;
    int pointsMax = 0;
    string ignore;
    ifstream file;

    // Conta quantos pontos tem no arquivo para definir o tamanho dos arrays
    file.open("Scores.txt");
    if(file.is_open()){
        while(!file.eof()){
            file>>ignore;
            pointsMax++;
        }
    }else{
        pointsMax = 1;
    }
    file.close();

    // divide o total de pontos por 2, pois cada ponto tem um nome e um valor
    pointsMax/=2;
    pointsMax++;
    
    string nomes[pointsMax];
    int pontos[pointsMax];

    // Lê o arquivo novamente, mas dessa vez armazena os pontos e os nomes em arrays para depois ordenar e mostrar na tela
    file.open("Scores.txt");
    if(file.is_open()){
        while(!file.eof()){
            if(i%2==0){
                file>>pontos[j];
                j++;
            }else{
                file>>nomes[j];
            }
            i++;
        }
    }
    file.close();

    // Ordena os pontos e os nomes, ele procura o maior ponto e coloca na posição correta, depois repete o processo para o próximo ponto até ordenar todoss os pontos
    int idAlto = 1;
    for(int id=1; id<pointsMax; id++){
        int maiorPonto = 0;
        for(int i=id; i<pointsMax; i++){
            if(pontos[i]>maiorPonto){
                maiorPonto = pontos[i];
                idAlto = i;
            }
        }
        if(idAlto<pointsMax){
            int auxPonto = pontos[idAlto];
            string auxNome = nomes[idAlto];
            pontos[idAlto] = pontos[id];
            nomes[idAlto] = nomes[id];
            pontos[id] = auxPonto;
            nomes[id] = auxNome;
        }
    }
    cout<<"\e[10;1H";
    cout<<"\nPontuações:"<<endl<<endl;
    if(pointsMax==1){
        cout<<"    Nenhuma pontuação registrada"<<endl;
    }
    for(int i=1; i<pointsMax; i++){
        cout<<"    "<<i<<" - "<<nomes[i]<<"   \e[38;5;46m"<<pontos[i]<<"\e[0m"<<endl;
    }
}

void new_line(string x, string y, string z,int size){
    cout<<x;
    for(int i = 0; i<size; i++){
        cout<<y;
    }
    cout<<z<<endl;
}

void render_details(InfoType &info,PlayerType &player1,PlayerType &player2,BossType &boss){
    cout << "\e[2;"<<mapSizeX+36<<"H";
    if(info.timer.Sec<10){
        cout << 0;
    }
    cout << info.timer.Sec;
    cout << "\e[2;"<<mapSizeX+33<<"H";
    if(info.timer.Min<10){
        cout << 0;
    }
    cout << info.timer.Min;
    cout << "\e[2;"<<mapSizeX+30<<"H";
    if(info.timer.Hour<10){
        cout << 0;
    }
    cout << info.timer.Hour;

    cout<<"\e[2;"<<mapSizeX+16<<"H";
    if(!player1.Alive){
        cout<<"\e[38;5;9m";
        player1.Slot = 0;
    }
    for(int i = player1.Points; i<1000000; i*=10){
        if(i==0){
            i = 1;
        }
        cout<<0;
    }
    if(player1.Points!=0 && player1.Alive){
        cout<<"\e[38;5;46m"<<player1.Points<<"\e[0m";
    }else{
        cout<<player1.Points;
    }

    cout<<"\e[2;"<<mapSizeX+4<<"H";
    cout<<"["<<player1.TotalMoves<<"]["<<player1.Totalbombs<<"]";

    cout<<"\e[3;"<<mapSizeX+4<<"H";
    cout<<"[";
    if(player1.Slot==1){
        cout<<"\e[38;5;3m";
    }
    cout<<"◉:";
    if(player1.MaxBombs<10){
        cout<<"0"<<player1.MaxBombs;
    }else{
        cout<<player1.MaxBombs;
    }
    if(player1.Alive){
        cout<<"\e[0m";
        if(player1.Slot==2){
            cout<<"\e[38;5;3m";
        }
    }
    cout<<" ◈:";
    if(player1.MaxRange<10){
        cout<<"0"<<player1.MaxRange;
    }else{
        cout<<player1.MaxRange;
    }
    if(player1.Alive){
        cout<<"\e[0m";
        if(player1.Slot==3){
            cout<<"\e[38;5;3m";
        }
    }
    cout<<" ♥:";
    if(player1.Lifes<10){
        cout<<"0"<<player1.Lifes;
    }else{
        cout<<player1.Lifes;
    }
    if(player1.Alive){
        cout<<"\e[0m";
    }
    cout<<"]";

    cout<<"\e[3;"<<mapSizeX+20<<"H";
    if(player1.Slot==4){
        cout<<"\e[38;5;3m";
    }
    if(player1.Item==0){
        if(player1.Slot==4){
            player1.Slot = 0;
        }
        cout<<"[ ]";
    }
    if(player1.Item==1){
        cout<<"[◊]";
    }
    if(player1.Item==2){
        cout<<"[⬘]";
    }
    if(player1.Item==3){
        cout<<"[Ω]";
    }
    if(player1.Item==4){
        cout<<"[ᛰ]";
    }
    if(player1.Item==5){
        cout<<"[☖]";
    }
    if(player1.Item==6){
        cout<<"[ᛟ]";
    }
    cout<<"\e[0m";

    if(info.players==3){
        cout<<"\e[6;"<<mapSizeX+16<<"H";
        if(!player2.Alive){
            cout<<"\e[38;5;9m";
            player2.Slot = 0;
        }
        for(int i = player2.Points; i<1000000; i*=10){
            if(i==0){
                i = 1;
            }
            cout<<0;
        }
        if(player2.Points!=0 && player2.Alive){
            cout<<"\e[38;5;46m"<<player2.Points<<"\e[0m";
        }else{
            cout<<player2.Points;
        }

        cout<<"\e[6;"<<mapSizeX+4<<"H";
        cout<<"["<<player2.TotalMoves<<"]["<<player2.Totalbombs<<"]";

        cout<<"\e[7;"<<mapSizeX+4<<"H";
        cout<<"[";
        if(player2.Slot==1){
            cout<<"\e[38;5;3m";
        }
        cout<<"◉:";
        if(player2.MaxBombs<10){
            cout<<0<<player2.MaxBombs;
        }else{
            cout<<player2.MaxBombs;
        }
        if(player2.Alive){
            cout<<"\e[0m";
            if(player2.Slot==2){
                cout<<"\e[38;5;3m";
            }
        }
        cout<<" ◈:";
        if(player2.MaxRange<10){
            cout<<0<<player2.MaxRange;
        }else{
            cout<<player2.MaxRange;
        }
        if(player2.Alive){
            cout<<"\e[0m";
            if(player2.Slot==3){
                cout<<"\e[38;5;3m";
            }
        }
        cout<<" ♥:";
        if(player2.Lifes<10){
            cout<<0<<player2.Lifes;
        }else{
            cout<<player2.Lifes;
        }
        if(player2.Alive){
            cout<<"\e[0m";
        }
        cout<<"]";

        cout<<"\e[7;"<<mapSizeX+20<<"H";
        if(player2.Slot==4){
            cout<<"\e[38;5;3m";
        }
        if(player2.Item==0){
            if(player2.Slot==4){
                player2.Slot = 0;
            }
            cout<<"[ ]";
        }
        if(player2.Item==1){
            cout<<"[◊]";
        }
        if(player2.Item==2){
            cout<<"[⬘]";
        }
        if(player2.Item==3){
            cout<<"[Ω]";
        }
        if(player2.Item==4){
            cout<<"[ᛰ]";
        }
        if(player2.Item==5){
            cout<<"[☖]";
        }
        if(player2.Item==6){
            cout<<"[ᛟ]";
        }
        cout<<"\e[0m";
    }
    cout << "\e["<<mapSizeY+4<<";2H";
    if(boss.Alive){
        for(int hp=0;hp<boss.MaxHP;hp++){
            if(hp<boss.HP){
                cout<<"\e[48;5;9m\e[38;5;9m \e[0m";
            }else{
                cout<<"\e[48;5;0m\e[38;5;0m \e[0m";
            }
        }
    }else{
        new_line(""," ","", boss.MaxHP);
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

bool explode(int map[mapSizeY][mapSizeX], BombType bomb, int y, int x,int i=0){
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
        explode(map,bomb,y,x,i);
        return false;
    }
}

bool explode_pandora(int map[mapSizeY][mapSizeX], BombType bomb, PlayerType player1, PlayerType player2, EnemyType enemys[], int enemysQuantity, BossType boss){
    while(true){
        int y;
        int x;
        bool success = true;
        while(true){
            y = rand()%3 - 1;
            x = rand()%3 - 1;
            if(y==0 || x==0){
                if(map[bomb.Pos.Y+y][bomb.Pos.X+x] == freeBlock || map[bomb.Pos.Y+y][bomb.Pos.X+x] == fragileBlock || map[bomb.Pos.Y+y][bomb.Pos.X+x] == explosionBlock){
                    break;
                }
            }
        }
        if(map[bomb.Pos.Y+y][bomb.Pos.X+x] == fragileBlock){
            success = false;
        }
        if(bomb.Pos.Y+y == player1.Pos.Y && bomb.Pos.X+x == player1.Pos.X){
            success = false;
        }
        if(bomb.Pos.Y+y == player2.Pos.Y && bomb.Pos.X+x == player2.Pos.X){
            success = false;
        }
        if(success){
            map[bomb.Pos.Y+y][bomb.Pos.X+x] = explosionBlock;
        }else{
            map[bomb.Pos.Y+y][bomb.Pos.X+x] = explosionBlock;
            return false;
        }
        bomb.Pos.Y += y;
        bomb.Pos.X += x;
    }
}

int bombs_explosion(InfoType info,BossType &boss,BombType bombs[], int bomb, PlayerType &player,PlayerType &otherPlayer, int map[mapSizeY][mapSizeX], BombType explosions[], int enemysQuantity, EnemyType enemys[]){
    int sound = 0;
    if (map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X] == freeBlock){
        map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X] = bombBlock;
    }
    if (bombs[bomb].Pos.Y > 0 && bombs[bomb].Pos.X > 0 && ((clock() - bombs[bomb].Time) >= 1000)){
        map[bombs[bomb].Pos.Y][bombs[bomb].Pos.X] = explosionBlock;
        if(bombs[bomb].Range==2000){
            explode_pandora(map,bombs[bomb],player,otherPlayer,enemys,enemysQuantity,boss);
        }else{
            explode(map, bombs[bomb], 1, 0);
            explode(map, bombs[bomb], 0, 1);
            explode(map, bombs[bomb], -1, 0);
            explode(map, bombs[bomb], 0, -1);
        }
        explosions[bomb] = bombs[bomb];
        bombs[bomb].Pos.Y = -1;
        bombs[bomb].Pos.X = -1;
        bombs[bomb].Time = 0;
        sound = 1;
    }
    if (explosions[bomb].Pos.Y > 0 && explosions[bomb].Pos.X > 0 && ((clock() - explosions[bomb].Time) >= 1350)){
        double combo = 1;
        for (int enemy = 0; enemy < enemysQuantity; enemy++){
            if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X] == explosionBlock){
                if(enemys[enemy].Pos.Y != -1 && enemys[enemy].Pos.X != -1){
                    player.Points += 250 * combo;
                    combo+=0.333333333333333333333333333333333333333333333333333333334;
                    enemys[enemy].Pos.Y = -1;
                    enemys[enemy].Pos.X = -1;
                    if(boss.HP>1){
                        boss.HP--;  
                    }
                    sound = 2;
                }
            }
        }
        for (int y = 0; y < mapSizeY; y++){
            for (int x = 0; x < mapSizeX; x++){
                if (map[y][x] == explosionBlock){
                    map[y][x] = freeBlock;
                }
            }
        }
        explosions[bomb].Pos.Y = -1;
        explosions[bomb].Pos.X = -1;
        explosions[bomb].Time = 0;
    }
    return sound;
}

void lost_life(PlayerType &player, int &moveEnemy, int &timerClock){
    player.Lifes--;
    if(player.Slot!=4){
        player.Slot = 0;
    }
    if (player.Lifes >= 1){
        player.Points -= 250;
        player.MaxBombs = 1;
        player.MaxRange = 1;
        moveEnemy = clock();
        timerClock = clock();
        player.Item = 0;
        player.Invincible = 1;
    }
    else{
        player.Pos.Y = -2;
        player.Pos.X = -2;
        player.Alive = false;
    }
}

int player_verifier(int enemysQuantity, PlayerType &player, EnemyType enemys[], int &freezeEnemys, int &timerClock, int map[mapSizeY][mapSizeX], int &moveEnemy, int fragileWallQuantity, PositionType boxs[], BossType boss){
    int sound = 0;
    for(int enemy = 0; enemy < enemysQuantity; enemy++){
        if(player.Item == 1){
            for(int y = -1; y <= 1; y++){
                for(int x = -1; x <= 1; x++){
                    if((player.Pos.Y + y == enemys[enemy].Pos.Y && player.Pos.X + x == enemys[enemy].Pos.X)){
                        freezeEnemys = 3;
                        player.Item = 0;
                        timerClock = clock();
                        sound = 3;
                    }
                }
            }
        }
    }
    if(player.Item == 5){
        int combo = 1;
        for(int enemy = 0; enemy < enemysQuantity; enemy++){
            if((player.Pos.Y == enemys[enemy].Pos.Y && player.Pos.X == enemys[enemy].Pos.X) || (player.Pos.Y == boss.Pos.Y && player.Pos.X == boss.Pos.X)){
                player.Item = 0;
                if(player.Slot==4){
                    player.Slot = 0;
                }
                enemys[enemy].Pos.Y = -1;
                enemys[enemy].Pos.X = -1;
                player.Points += 250 * combo;
                combo+=0.333333333333333333333333333333333333333333333333333333334;
                for(int y=-1;y<=1;y++){
                    for(int x=-1;x<=1;x++){
                        if(y!=0 && x!=0){
                            if(player.Pos.Y+y == enemys[enemy].Pos.Y && player.Pos.X+x == enemys[enemy].Pos.X){
                                enemys[enemy].Pos.Y = -1;
                                enemys[enemy].Pos.X = -1;
                                player.Points += 250 * combo;
                                combo+=0.333333333333333333333333333333333333333333333333333333334;
                            }
                            if(player.Pos.Y+y == boss.Pos.Y && player.Pos.X+x == boss.Pos.X){
                                if(boss.HP>1){
                                    boss.HP--;
                                }
                                player.Points += 500;
                            }
                        }
                    }
                }
                sound  = 5;
            }
        }
    }
    if(map[player.Pos.Y][player.Pos.X] == explosionBlock && player.Invincible == 0){
        lost_life(player, moveEnemy, timerClock);
        if(player.Item==5){
            lost_life(player, moveEnemy, timerClock);
            player.Item = 0;
            if(player.Slot==4){
                player.Slot = 0;
            }
        }
        sound = 1;
    }
    if(player.Item!=5){
        if((player.Pos.Y==boss.Pos.Y && player.Pos.X==boss.Pos.X) && player.Invincible == 0){
            lost_life(player, moveEnemy, timerClock);
            sound = 1;
        }
        for(int enemy = 0; enemy < enemysQuantity; enemy++){
            if ((player.Pos.Y == enemys[enemy].Pos.Y && player.Pos.X == enemys[enemy].Pos.X) && player.Invincible == 0){
                lost_life(player, moveEnemy, timerClock);
                sound = 1;
            }
        }
    }
    for(int box = 0; box < fragileWallQuantity; box++){
        if(player.Pos.Y == boxs[box].Pos.Y && player.Pos.X == boxs[box].Pos.X){
            if(map[player.Pos.Y][player.Pos.X] == freeBlock){
                sound = 2;
                boxs[box].Pos.Y = -1;
                boxs[box].Pos.X = -1;
                if (rand()%4!=0){
                    int item = rand() % 3;
                    if (item == 0){
                        player.MaxBombs++;
                        player.Slot = 1;
                    }
                    if (item == 1){
                        player.MaxRange++;
                        player.Slot = 2;
                    }
                    if (item == 2){
                        player.Lifes++;
                        player.Slot = 3;
                    }
                }else{
                    bool success = false;
                    while (!success){
                        int item = rand() % 5 + 1;
                        player.Slot = 4;
                        player.Item = item;
                        if(player.Item == 2){
                            sound = 4;
                        }
                        success = true;
                    }
                }
            }
        }
    }
    return sound;
}

void player_action(PlayerType &player,PlayerType &otherPlayer, PositionType &target, BombType bombs[],int map[mapSizeY][mapSizeX], bool putBomb){
    if((map[player.Pos.Y][player.Pos.X] != freeBlock && map[player.Pos.Y][player.Pos.X] != explosionBlock) || (player.Pos.Y==otherPlayer.Pos.Y && player.Pos.X==otherPlayer.Pos.X)){ // Testa colisão e cancela o movimento se verdadeiro
        if(player.Item == 2 && player.Invincible == 0 && map[player.Pos.Y][player.Pos.X] == fragileBlock && !(player.Pos.Y==otherPlayer.Pos.Y && player.Pos.X==otherPlayer.Pos.X)){
            if(rand()%10==0){
                player.Item = 0;
                player.Slot = 0;
                for(int y=-1;y<=1;y++){
                    for(int x=-1;x<=1;x++){
                        if(y==0 || x==0){
                            if(map[player.Pos.Y+y][player.Pos.X+x] == fragileBlock){
                                map[player.Pos.Y+y][player.Pos.X+x] = freeBlock;
                            }
                        }
                    }
                }
            }
        }else{
            player.Pos.Y -= target.Pos.Y;
            player.Pos.X -= target.Pos.X;
        }
    }
    else{
        player.TotalMoves++;
    }
    if(putBomb){
        player.Totalbombs++;
        player.Points -= 10;
        bombs[player.ActualBomb].Range = player.MaxRange;
        if(player.Item == 3){
            player.Item = 0;
            if(player.Slot==4){
                player.Slot = 0;
            }
            bombs[player.ActualBomb].Range = 1000;
        }
        if(player.Item == 4){
            player.Item = 0;
            if(player.Slot==4){
                player.Slot = 0;
            }
            bombs[player.ActualBomb].Range = 2000;
        }
        bombs[player.ActualBomb].Pos.Y = player.Pos.Y;
        bombs[player.ActualBomb].Pos.X = player.Pos.X;
        bombs[player.ActualBomb].Time = clock();
        player.ActualBomb++;
    }
}

void player_sync(PlayerType &player, int maximumBombs){
    if(player.ActualBomb>=maximumBombs){
        player.ActualBomb = 0;
    }
    if(player.Points<0){
        player.Points=0;
    }
    if(!player.Alive){
        player.MaxBombs=0;
        player.MaxRange=0;
        player.Lifes=0;
    }
}

int game(InfoType &info){
    //------------------------> SONS >------------------------//
    int sound1 = 0;
    int sound2 = 0;

    SoundBuffer ampulhetaBF;
    if(!ampulhetaBF.loadFromFile("Sounds/ampulheta.wav")){}
    Sound ampulhetaSD(ampulhetaBF);
    ampulhetaSD.setVolume(50);

    SoundBuffer espectroBF;
    if(!espectroBF.loadFromFile("Sounds/espectro.wav")){}
    Sound espectroSD(espectroBF);

    SoundBuffer explosaoBF;
    if(!explosaoBF.loadFromFile("Sounds/explosao.wav")){}
    Sound explosaoSD(explosaoBF);
    explosaoSD.setVolume(30);

    SoundBuffer morteInimigoBF;
    if(!morteInimigoBF.loadFromFile("Sounds/morte_inimigo.wav")){}
    Sound morteInimigoSD(morteInimigoBF);

    SoundBuffer musicaBossBF;
    if(!musicaBossBF.loadFromFile("Sounds/musica_boss.wav")){}
    Sound musicaBossSD(musicaBossBF);
    musicaBossSD.setLooping(true);
    musicaBossSD.setVolume(50);

    SoundBuffer musicaFaseBF;
    if(!musicaFaseBF.loadFromFile("Sounds/musica_fase.wav")){}
    Sound musicaFaseSD(musicaFaseBF);
    musicaFaseSD.setLooping(true);
    musicaFaseSD.setVolume(25);
    musicaFaseSD.play();

    SoundBuffer pegandoLootBF;
    if(!pegandoLootBF.loadFromFile("Sounds/pegando_loot.wav")){}
    Sound pegandoLootSD(pegandoLootBF);

    SoundBuffer perdeuVidaBF;
    if(!perdeuVidaBF.loadFromFile("Sounds/perdeu_vida.wav")){}
    Sound perdeuVidaSD(perdeuVidaBF);
    //------------------------< SONS <------------------------//

    //------------------------> VARIAVEIS GERAIS >------------------------//
    int enemysQuantity;
    if(info.difficulty==1){
        enemysQuantity = 3*info.phase;
    }
    if(info.difficulty==2){
        enemysQuantity = 5*info.phase;
    }
    if(info.difficulty==3){
        enemysQuantity = 7*info.phase;
    }
    int moveEnemy = clock();
    int freezeEnemys = 0;
    EnemyType enemys[enemysQuantity];
    BossType boss;
    if(info.phase == 3){
        if(info.difficulty==1){
            boss.MaxHP = 10;
        }
        if(info.difficulty==2){
            boss.MaxHP = 15;
        }
        if(info.difficulty==3){
            boss.MaxHP = 25;
        }
        boss.HP = boss.MaxHP;
        boss.Alive = true;
    }

    int fragileWallQuantity = 50;
    PositionType boxs[fragileWallQuantity];
    PositionType portal;

    PlayerType player1;
    PlayerType player2;
    if(info.phase>1){
        player1.Alive = info.player1.Alive;
        player1.Points = info.player1.Points;
        player1.Totalbombs = info.player1.Totalbombs;
        player1.TotalMoves = info.player1.TotalMoves;
        player2.Alive = info.player2.Alive;
        player2.Points = info.player2.Points;
        player2.Totalbombs = info.player2.Totalbombs;
        player2.TotalMoves = info.player2.TotalMoves;
    }

    if(info.players==1 || info.players==2){
        player2.Points = 0;
    }

    int maximumBombs = 25;
    BombType bombs1[maximumBombs];
    BombType explosions1[maximumBombs];
    BombType bombs2[maximumBombs];
    BombType explosions2[maximumBombs]; 

    int timerClock = clock();

    int map[mapSizeY][mapSizeX];

    if(info.phase==3){
        musicaFaseSD.stop();
        musicaBossSD.play();
    }

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
    if(info.phase!=3){
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
    }else{
        for (int y = 0; y < mapSizeY; y += 2){
            for (int x = 0; x < mapSizeX; x += 2){
                if(map[y][x]==freeBlock){
                    map[y][x] = fragileBlock;
                    bool success = true;
                    for(int i=0; i < fragileWallQuantity; i++){
                        if(boxs[i].Pos.Y==y && boxs[i].Pos.X==x){
                            success = false;
                        }
                    }
                    if(rand()%4==0 && success){
                        int idx = rand()%fragileWallQuantity;
                        boxs[idx].Pos.Y=y;
                        boxs[idx].Pos.X=x;
                    }
                }
            }
        }
    }
    for (int  i = 0; i < enemysQuantity; i++){ // Gera as posições dos enemys
        EnemyType enemy;   // Posição do inimigo
        if(info.phase!=3){
            while(true){ // Gera uma posição aleatoria para o inimigo e faz as verificações para não gerar o inimigo emcima do player1, de outros enemys ou em blocos proibidos
                bool success = false;
                enemy.Pos.Y = rand()%mapSizeY;
                enemy.Pos.X = rand()%mapSizeX;
                enemy.Move = 0;
                for (int otherEnemy = 0; otherEnemy < enemysQuantity; otherEnemy++){
                    if (enemy.Pos.X == enemys[otherEnemy].Pos.X && enemy.Pos.Y == enemys[otherEnemy].Pos.Y){
                        enemy.Pos.Y = player1.Pos.Y;
                        enemy.Pos.X = player1.Pos.X;
                    }
                }
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
                        enemys[i] = enemy;
                        break;
                    }
                }  
            }
        }else{
            while(true){ // Gera uma posição aleatoria para o inimigo e faz as verificações para não gerar o inimigo emcima do player1, de outros enemys ou em blocos proibidos
                bool success = false;
                boss.Pos.Y = rand()%mapSizeY;
                boss.Pos.X = rand()%mapSizeX;
                if(map[boss.Pos.Y][boss.Pos.X] == freeBlock){
                    success = true;
                    for (int y = -1; y < 2; y++){
                        for (int x = -1; x < 2; x++){
                            if(y==0 || x==0){
                                if (map[boss.Pos.Y+y][boss.Pos.X+x] == fragileBlock){
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
        }
    }
    if(player1.Alive){
        while(true){ // Gera a posição do jogador
            bool success = false;
            player1.Pos.Y = rand()%mapSizeY;
            player1.Pos.X = rand()%mapSizeX;
            if (map[player1.Pos.Y][player1.Pos.X] == freeBlock){ // Verifica se a posição do player1 é uma area livre, se for, ele faz uma verificação para não gerar o player1 perto de paredes frageis
                success = true;
                for (int y = -1; y < 2; y++){
                    for (int x = -1; x < 2; x++){ // Passa por uma matriz 3x3 em volta do player1
                        if(y==0 || x==0){ // Verifica se tem uma parede fragil em volta do player1, se tiver, ele gera outra posição
                            if (map[player1.Pos.Y+y][player1.Pos.X+x] == fragileBlock){
                                success = false;
                            }
                            for(int enemy = 0; enemy < enemysQuantity; enemy++){
                                if(player1.Pos.Y+y == enemys[enemy].Pos.Y && player1.Pos.X+x == enemys[enemy].Pos.X){
                                    success = false;
                                }
                            }
                        }
                    }
                }
                if(success){
                    break;
                }
            }
        }
    }else{
        player1.Pos.Y = -2;
        player1.Pos.X = -2;
        player1.Alive = false;
    }
    if(info.players==3 && player2.Alive){
        player2.Pos.Y = player1.Pos.Y;
        player2.Pos.X = player1.Pos.X;
    }else{
        player2.Pos.Y = -2;
        player2.Pos.X = -2;
        player2.Alive = false;
    }
//------------------------< GERAÇÃO DO MAPA <------------------------//

//------------------------> BORDAS LATERAIS >------------------------//
    cout<<"\e[1;"<<mapSizeX+3<<"H";
    new_line("┏","━","┓",19);
    cout<<"\e[2;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",19);
    cout<<"\e[3;"<<mapSizeX+3<<"H";
    new_line("┃"," ","┃",19);
    cout<<"\e[4;"<<mapSizeX+3<<"H";
    new_line("┗","━","┛",19);
    if(info.players==3){
        cout<<"\e[5;"<<mapSizeX+3<<"H";
        new_line("┏","━","┓",19);
        cout<<"\e[6;"<<mapSizeX+3<<"H";
        new_line("┃"," ","┃",19);
        cout<<"\e[7;"<<mapSizeX+3<<"H";
        new_line("┃"," ","┃",19);
        cout<<"\e[8;"<<mapSizeX+3<<"H";
        new_line("┗","━","┛",19);
    }

    cout << "\e[1;"<<mapSizeX+24<<"H";
    new_line("┏","━","┓",14);
    cout << "\e[2;"<<mapSizeX+24<<"H";
    cout << "┃["<<info.phase<<"] [00:00:00]┃";
    cout << "\e[3;"<<mapSizeX+24<<"H";
    new_line("┗","━","┛",14);
    if(info.phase==3){
        cout << "\e["<<mapSizeY+3<<";1H";
        new_line("┏","━","┓",boss.MaxHP);
        new_line("┃"," ","┃",boss.MaxHP);
        new_line("┗","━","┛",boss.MaxHP);
    }
//------------------------< BORDAS LATERAIS <------------------------//

//---------------------------> RENDERIZAÇÃO DO MAPA >---------------------------//
    while(true){
        player_sync(player1,maximumBombs);
        player_sync(player2,maximumBombs);

        info.player1.Alive = player1.Alive;
        info.player1.Points = player1.Points;
        info.player1.Totalbombs = player1.Totalbombs;
        info.player1.TotalMoves = player1.TotalMoves;

        info.player2.Alive = player2.Alive;
        info.player2.Points = player2.Points;
        info.player2.Totalbombs = player2.Totalbombs;
        info.player2.TotalMoves = player2.TotalMoves;

        cout << "\e[?25l\e[1;1H";
        new_line("┏","━","┓",mapSizeX);
        for (int y = 0; y < mapSizeY; y++) {
            cout<<"┃";
            for (int x = 0; x < mapSizeX; x++) {
                 bool block = true;
                 if(block){
                    if(y==boss.Pos.Y && x==boss.Pos.X){
                        block = false;
                        if(map[boss.Pos.Y][boss.Pos.X] == explosionBlock){
                            cout << "\e[0;43m\e[38;5;124m\u25A1";
                        }else{
                            cout << "\e[0;42m\e[38;5;124m\u25A0"; // BOSS
                        }
                    }
                 }
                 if(block){
                    for(int enemy=0; enemy<enemysQuantity; enemy++) {
                        if(y==enemys[enemy].Pos.Y && x==enemys[enemy].Pos.X) {
                            block = false;
                            if(map[enemys[enemy].Pos.Y][enemys[enemy].Pos.X]==explosionBlock) { // Testa se naquela posição tem um bloco de explosão
                                if(freezeEnemys==0){
                                    cout << "\e[31;43m\u25A1"; // INIMIGO MORTO NA EXPLOSÃO
                                    break;
                                }else{
                                    cout << "\e[31;43m\e[38;5;52m\u25A1"; // INIMIGO MORTO NA EXPLOSÃO CONGELADO
                                    break;
                                }
                            }else{
                                if((player1.Pos.Y==enemys[enemy].Pos.Y && player1.Pos.X==enemys[enemy].Pos.X) || (player2.Pos.Y==enemys[enemy].Pos.Y && player2.Pos.X==enemys[enemy].Pos.X)) {
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
                    if (player1.Pos.Y == y && player1.Pos.X == x) {
                        block = false;
                        if(map[player1.Pos.Y][player1.Pos.X] == explosionBlock){
                            cout << "\e[97;43m\e[38;5;255m\u25CB"; // PLAYER MORTO NA EXPLOSÃO
                        }else{
                            if(player1.Invincible==0){
                                if(map[player1.Pos.Y][player1.Pos.X] == fragileBlock){
                                    cout << "\e[48;5;245m\e[38;5;255m\u25CF"; // PLAYER
                                }else{
                                    cout << "\e[0;42m\e[38;5;255m\u25CF"; // PLAYER
                                }
                            }else{
                                if(swaper<int>(1)==1){
                                    cout << "\e[0;42m\e[38;5;255m\u25CF"; // PLAYER
                                }else{
                                    cout << "\e[32;42m ";
                                }
                            }
                        }
                    }
                }
                if(block){
                    if (player2.Pos.Y == y && player2.Pos.X == x) {
                        block = false;
                        if(map[player2.Pos.Y][player2.Pos.X] == explosionBlock){
                            cout << "\e[97;43m\e[38;5;0m\u25CB"; // PLAYER MORTO NA EXPLOSÃO
                        }else{
                            if(player2.Invincible==0){
                                if(map[player2.Pos.Y][player2.Pos.X] == fragileBlock){
                                        cout << "\e[48;5;245m\e[38;5;0m\u25CF"; // PLAYER
                                }else{
                                    cout << "\e[0;42m\e[38;5;0m\u25CF"; // PLAYER
                                }
                            }else{
                                if(swaper<double>(2)==1){
                                    cout << "\e[0;42m\e[38;5;0m\u25CF"; // PLAYER
                                }else{
                                    cout << "\e[32;42m ";
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
                                if(rand()%2){
                                    cout<<"\e[32;42m\e[38;5;3m\u25CF";
                                }else{
                                    cout<<"\e[32;42m\e[38;5;208m\u25CF";
                                }
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
        render_details(info,player1,player2,boss);
//---------------------------< RENDERIZAÇÃO DO MAPA <---------------------------//

//----------------------> SISTEMA DO PLAYER >----------------------//
        if((!player1.Alive) && (!player2.Alive)){
            return 0;
        }
        if(!player1.Alive){
            player1.Points = 0;
        }
        if(!player2.Alive){
            player2.Points = 0;
        }
        if((player1.Pos.Y == portal.Pos.Y && player1.Pos.X == portal.Pos.X) || (player2.Pos.Y == portal.Pos.Y && player2.Pos.X == portal.Pos.X)){
            info.phase++;
            return 1;
        }
        sound1 = 0;
        sound2 = 0;
        sound1 = player_verifier(enemysQuantity, player1, enemys, freezeEnemys, timerClock, map, moveEnemy, fragileWallQuantity, boxs, boss);
        sound2 = player_verifier(enemysQuantity, player2, enemys, freezeEnemys, timerClock, map, moveEnemy, fragileWallQuantity, boxs, boss);
        if(!player1.Alive){
            sound1 = 0;
        }
        if(!player2.Alive){
            sound2 = 0;
        }
        if(sound1==1 || sound2==1){
            perdeuVidaSD.play();
        }
        if(sound1==2 || sound2==2){
            pegandoLootSD.play();
        }
        if(sound1==3 || sound2==3){
            ampulhetaSD.play();
        }
        if(sound1==4 || sound2==4){
            espectroSD.play();
        }
        if(sound1 == 5 || sound2 == 5){
            morteInimigoSD.play();
        }

        if (kbhit() || info.players==1){
            PositionType target1;
            PositionType target2;
            target1.Pos.Y = 0;
            target1.Pos.X = 0;
            target2.Pos.Y = 0;
            target2.Pos.X = 0;
            int key;

            if(info.players==3){
                key = getch();
            }
            if(info.players==2){
                key = getch();
            }
//----------------------> CPU PLAYER >----------------------//
            if(info.players==1){
                key = 0;
                int danger[4] = {-1,-1,-1,-1};
                int totalEnemys = 0;
                int priority = 0;
                bool putBomb = false;
                if(boss.Alive){
                    priority = 1;
                }else{
                    priority = 2;
                }
                for(int y=-1;y<=1;y++){
                    for(int x=-1;x<=1;x++){
                        if((y==0||x==0)){
                            if(player1.Item==2 && player1.Invincible==0){
                                if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock && map[player1.Pos.Y+y][player1.Pos.X+x]!=fragileBlock){
                                    found_danger(danger,y,x);
                                }
                            }else{
                                if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock){
                                    found_danger(danger,y,x);
                                }
                            }
                            for(int enemy=0;enemy<enemysQuantity;enemy++){
                                if((player1.Pos.Y+y == enemys[enemy].Pos.Y && player1.Pos.X+x == enemys[enemy].Pos.X)){
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
                                if(enemys[enemy].Move == 4){
                                    X++;
                                }
                                if(((player1.Pos.Y+y == enemys[enemy].Pos.Y+Y && player1.Pos.X+x == enemys[enemy].Pos.X+X) && freezeEnemys==0)){
                                    putBomb = true;
                                    found_danger(danger,y,x);
                                }
                            }
                            if(boss.Alive){
                                if((player1.Pos.Y+y == boss.Pos.Y && player1.Pos.X+x == boss.Pos.X)){
                                    found_danger(danger,y,x);
                                    putBomb = true;
                                }
                                int Y = 0;
                                int X = 0;
                                if(boss.Move == 1){
                                    Y++;
                                }
                                if(boss.Move == 2){
                                    Y--;
                                }
                                if(boss.Move == 3){
                                    X++;
                                }
                                if(boss.Move == 4){
                                    X--;
                                }
                                if(((player1.Pos.Y+y == boss.Pos.Y+Y && player1.Pos.X+x == boss.Pos.X+X))){
                                    found_danger(danger,y,x);
                                }
                            }
                            int safes = 0;
                            for(int i=0;i<4;i++){
                                if(danger[i]==-1){
                                    safes++;
                                }
                            }
                            if(safes>=2){
                                for(int bomb=0;bomb<maximumBombs;bomb++){
                                    if((player1.Pos.Y+y == bombs1[bomb].Pos.Y || player1.Pos.X+x == bombs1[bomb].Pos.X)){
                                        if(map[player1.Pos.Y][player1.Pos.X]!=bombBlock){
                                            int Y = (0-player1.Pos.Y)+bombs1[bomb].Pos.Y;
                                            int X = (0-player1.Pos.X)+bombs1[bomb].Pos.X;
                                            found_danger(danger,Y,X);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(totalEnemys>0){
                    priority = 2;
                }
                if(portal.Pos.Y!=-1 && portal.Pos.X!=-1){
                    priority = 3;
                }
                for(int y=-2;y<=2;y++){
                    for(int x=-2;x<=2;x++){
                        if((player1.Pos.Y+y>0 && player1.Pos.Y+y<mapSizeY && player1.Pos.X+x>0 && player1.Pos.X+x<mapSizeX)){
                            if(((y>=-1||x>=-1) && (y<=1||x<=1)) || (y==0||x==0)){
                                for(int enemy=0;enemy<enemysQuantity;enemy++){
                                    if((player1.Pos.Y+y == enemys[enemy].Pos.Y && player1.Pos.X+x == enemys[enemy].Pos.X)){
                                        totalEnemys++;
                                    }
                                }
                            }
                            if(map[player1.Pos.Y][player1.Pos.X]==bombBlock){
                                putBomb = false;
                                if(y==-2 && x==0){
                                    if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock){
                                        int y1 = y+1;
                                        int x1 = x-1;
                                        if(map[player1.Pos.Y+y1][player1.Pos.X+x1]!=freeBlock){
                                            int y2 = y1;
                                            int x2 = x1+2;
                                            if(map[player1.Pos.Y+y2][player1.Pos.X+x2]!=freeBlock){
                                                found_danger(danger,y,x);
                                            }
                                        }
                                    }
                                }
                                if(y==0 && x==2){
                                    if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock){
                                        int y1 = y-1;
                                        int x1 = x-1;
                                        if(map[player1.Pos.Y+y1][player1.Pos.X+x1]!=freeBlock){
                                            int y2 = y1+2;
                                            int x2 = x1;
                                            if(map[player1.Pos.Y+y2][player1.Pos.X+x2]!=freeBlock){
                                                found_danger(danger,y,x);
                                            }
                                        }
                                    }
                                }
                                if(y==2 && x==0){
                                    if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock){
                                        int y1 = y-1;
                                        int x1 = x+1;
                                        if(map[player1.Pos.Y+y1][player1.Pos.X+x1]!=freeBlock){
                                            int y2 = y1;
                                            int x2 = x1-2;
                                            if(map[player1.Pos.Y+y2][player1.Pos.X+x2]!=freeBlock){
                                                found_danger(danger,y,x);
                                            }
                                        }
                                    }
                                }
                                if(y==0 && x==-2){
                                    if(map[player1.Pos.Y+y][player1.Pos.X+x]!=freeBlock){
                                        int y1 = y+1;
                                        int x1 = x+1;
                                        if(map[player1.Pos.Y+y1][player1.Pos.X+x1]!=freeBlock){
                                            int y2 = y1-2;
                                            int x2 = x1;
                                            if(map[player1.Pos.Y+y2][player1.Pos.X+x2]!=freeBlock){
                                                found_danger(danger,y,x);
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
                if(!theres_bomb(bombs1,maximumBombs) && totalEnemys<=1 && priority==1){
                    int y = player1.Pos.Y-boss.Pos.Y;
                    int x = player1.Pos.X-boss.Pos.X;
                    found_danger(danger,y,x);
                }
                if(rand()%2==0 && !theres_bomb(bombs1,maximumBombs) && totalEnemys<=1 && priority==2){
                    int enemyChoiced = -1;
                    for(int enemy=0;enemy<enemysQuantity;enemy++){
                        if(enemys[enemy].Pos.X==-1 && enemys[enemy].Pos.Y==-1){
                            continue;
                        }
                        if(enemys[enemy].Pos.X!=-1 && enemys[enemy].Pos.Y!=-1){
                            if(enemyChoiced==-1){
                                enemyChoiced = enemy;
                            }else{
                                if((VA(player1.Pos.Y-enemys[enemy].Pos.Y) < VA(player1.Pos.Y-enemys[enemyChoiced].Pos.Y)) && (VA(player1.Pos.X-enemys[enemy].Pos.X) < VA(player1.Pos.X-enemys[enemyChoiced].Pos.X))){
                                    enemyChoiced = enemy;
                                }
                            }
                        }
                    }
                    if(enemys[enemyChoiced].Pos.Y!=-1 && enemys[enemyChoiced].Pos.X!=-1){
                        int y = player1.Pos.Y-enemys[enemyChoiced].Pos.Y;
                        int x = player1.Pos.X-enemys[enemyChoiced].Pos.X;
                        found_danger(danger,y,x);
                    }
                }
                if(rand()%2==0 && !theres_bomb(bombs1,maximumBombs) && priority==3){
                    int y = player1.Pos.Y-portal.Pos.Y;
                    int x = player1.Pos.X-portal.Pos.X;
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
                        if((y==0||x==0) && (map[player1.Pos.Y+y][player1.Pos.X+x]==fragileBlock) && (rand()%4==0) && totalEnemys==0 && (((y>=-1||x>=-1) && (y<=1||x<=1)) && map[player1.Pos.Y+y][player1.Pos.X+x]!=bombBlock)){
                            key = 32;
                        }
                        if((y==0||x==0) && putBomb && (rand()%4==0) && safes>=1 && map[player1.Pos.Y+y][player1.Pos.X+x]!=bombBlock){
                            key = 32;
                        }
                    }
                }
                if(kbhit()){
                    key = 32;
                }
            }
//----------------------< CPU PLAYER <----------------------//
            if(player1.Alive){
                switch(key){
                    case 119: // Cima
                        target1.Pos.Y--;
                        break;
                    case 115: // Baixo
                        target1.Pos.Y++;
                        break;
                    case 97: // Esquerda
                        target1.Pos.X--;
                        break;
                    case 100: // Direita
                        target1.Pos.X++;
                        break;
                }
            }
            if(player2.Alive){
                switch(key){
                    case 72: // Cima
                        target2.Pos.Y--;
                        break;
                    case 80: // Baixo
                        target2.Pos.Y++;
                        break;
                    case 75: // Esquerda
                        target2.Pos.X--;
                        break;
                    case 77: // Direita
                        target2.Pos.X++;
                        break;
                }
            }
            player1.Pos.Y += target1.Pos.Y;
            player1.Pos.X += target1.Pos.X;
            player2.Pos.Y += target2.Pos.Y;
            player2.Pos.X += target2.Pos.X;
            if(key==119 || key==115 || key==97 || key==100 || key==32){
                if(player1.Alive){
                    int putBomb = false;
                    if(key==32 && (map[player1.Pos.Y][player1.Pos.X] == freeBlock) && player1.Invincible == 0 && (bombs1[player1.ActualBomb].Pos.Y == -1 && bombs1[player1.ActualBomb].Pos.X == -1)){
                        if(count_bombs(bombs1, maximumBombs) < player1.MaxBombs){
                            putBomb = true;
                        }
                    }
                    player_action(player1,player2,target1,bombs1,map,putBomb);
                }
            }
            if(key==72 || key==80 || key==75 || key==77 || key==13){
                if(player2.Alive){
                    int putBomb = false;
                    if(key==13 && (map[player2.Pos.Y][player2.Pos.X] == freeBlock) && player2.Invincible == 0 && (bombs2[player2.ActualBomb].Pos.Y == -1 && bombs2[player2.ActualBomb].Pos.X == -1)){
                        if(count_bombs(bombs2, maximumBombs) < player2.MaxBombs){
                            putBomb = true;
                        }
                    }
                    player_action(player2,player1,target2,bombs2,map,putBomb);
                }
            }
        }
//---------------------< SISTEMA DO PLAYER <----------------------//

//----------------------------> SISTEMA DOS INIMIGOS >----------------------------//
        if(((clock()-moveEnemy) >= 350) && freezeEnemys==0){
            int actualEnemys = 0;
            moveEnemy = clock();
            for(int enemy = 0; enemy<enemysQuantity; enemy++){
                if(enemys[enemy].Pos.Y == -1 && enemys[enemy].Pos.X == -1){
                    if(boss.Pos.Y == -1 && boss.Pos.X == -1){
                        if(enemy == enemysQuantity-1){
                            if(actualEnemys == 0){
                                if(portal.Pos.Y == -1 && portal.Pos.X == -1){
                                    while(true){ // Gera a posição do Portal
                                        bool success = false;
                                        portal.Pos.Y = rand()%mapSizeY,
                                        portal.Pos.X = rand()%mapSizeX;
                                        if(map[portal.Pos.Y][portal.Pos.X] == freeBlock){ // Verifica se a posição do player1 é uma area livre, se for, ele faz uma verificação para não gerar o player1 perto de paredes frageis
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
                                break;
                            }
                        }
                    }
                    continue;
                }
                actualEnemys++;
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
                for(int toTry = 0; toTry < 4; toTry++) {
                    success = true;
                    Y = 0;
                    X = 0;
                    if(info.difficulty==2){
                        if(rand()%2!=0){
                            enemyTarget = 0;
                        }else{
                            enemyTarget = rand()%4+1;
                        }
                    }
                    if(info.difficulty==3){
                        if(rand()%4!=0){
                            enemyTarget = 0;
                        }else{
                            enemyTarget = rand()%4+1;
                        }
                    }
                    if(toTry>0 || theres_bomb(bombs1,maximumBombs) || theres_bomb(bombs2,maximumBombs)){
                        enemyTarget = rand()%4+1;
                    }
                    if(enemyTarget==0){
                        int player = 0;
                        if(player1.Alive && player1.Invincible==0){
                            player = 1;
                        }
                        if(player2.Alive && player2.Invincible==0){
                            player = 2;
                        }
                        if((player1.Alive && player1.Invincible==0) && (player2.Alive && player2.Invincible==0)){
                            player = rand()%2+1;
                        }
                        if((!player1.Alive || player1.Invincible!=0) && (!player2.Alive || player2.Invincible!=0)){
                            player = 0;
                        }
                        if(player!=0){
                            int y = 0;
                            int x = 0;
                            if(player==1){
                                y = enemys[enemy].Pos.Y-player1.Pos.Y;
                                x = enemys[enemy].Pos.X-player1.Pos.X;
                            }
                            if(player==2){
                                y = enemys[enemy].Pos.Y-player2.Pos.Y;
                                x = enemys[enemy].Pos.X-player2.Pos.X;
                            }
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
                        }else{
                            enemyTarget = rand()%4+1;
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
        if(boss.Alive){
            if(clock()-boss.Clock >= 100){
                boss.Clock = clock();
                boss.Target.Y = 0;
                boss.Target.X = 0;
                if(boss.Move==1){
                    boss.Target.Y++;
                }
                if(boss.Move==2){
                    boss.Target.Y--;
                }
                if(boss.Move==3){
                    boss.Target.X++;
                }
                if(boss.Move==4){
                    boss.Target.X--;
                }
                if(map[boss.Pos.Y][boss.Pos.X]==explosionBlock){
                    boss.HP--;
                    if(boss.HP<0){
                        boss.HP = 0;
                    }
                    if(player1.Alive && player2.Alive){
                        player1.Points += 250;
                        player2.Points += 250;
                    }
                    if(player1.Alive){
                        player1.Points += 500;
                    }
                    if(player2.Alive){
                        player2.Points += 500;
                    }
                    morteInimigoSD.play();
                }
                boss.Pos.Y+=boss.Target.Y;
                boss.Pos.X+=boss.Target.X;
                
                if(rand()%4==0){
                    if(map[boss.Pos.Y][boss.Pos.X]==fragileBlock){
                        map[boss.Pos.Y][boss.Pos.X] = freeBlock;
                        boss.Pos.Y-=boss.Target.Y;
                        boss.Pos.X-=boss.Target.X;
                    }
                }
                if(map[boss.Pos.Y][boss.Pos.X]==explosionBlock || map[boss.Pos.Y][boss.Pos.X]==bombBlock || map[boss.Pos.Y][boss.Pos.X]==solidBlock || map[boss.Pos.Y][boss.Pos.X]==fragileBlock){
                    boss.Pos.Y-=boss.Target.Y;
                    boss.Pos.X-=boss.Target.X;
                }
                if(boss.HP>0){
                    if(rand()%boss.HP==0){
                        boss.Move = rand()%4+1;
                    }
                }
                if(boss.HP>1){
                    if(rand()%4==0){
                        boss.Move = 0;
                    }
                }
                if(theres_bomb(bombs1,maximumBombs) || theres_bomb(bombs2,maximumBombs)){
                    if(rand()%2==0){
                        boss.Move = rand()%4+1;
                    }
                }
                if(boss.HP>0){
                    if(boss.Move==0){
                        if(rand()%boss.HP==0){
                            int enemy = rand()%enemysQuantity;
                            enemys[enemy].Pos.Y = boss.Pos.Y;
                            enemys[enemy].Pos.X = boss.Pos.X;
                        }
                    }
                }
                if(map[boss.Pos.Y][boss.Pos.X]==explosionBlock){
                    boss.Move = 0;
                }
                int BlockY = rand()%mapSizeY;
                int BlockX = rand()%mapSizeX;
                for(int box=0;box<fragileWallQuantity;box++){
                    if(boxs[box].Pos.Y == boss.Pos.Y && boxs[box].Pos.X == boss.Pos.X){
                        boxs[box].Pos.Y = -1;
                        boxs[box].Pos.X = -1;    
                    }
                }
                if(rand()%10==0){
                    bool success = false;
                    for(int y=-1;y<=1;y++){
                        if(success){
                            break;
                        }
                        for(int x=-1;x<=1;x++){
                            if(success){
                                break;
                            }
                            if((y==0 || x==0)){
                                if(map[BlockY][BlockX]==freeBlock){
                                    if(map[BlockY+y][BlockX+x] == solidBlock || map[BlockY+y][BlockX+x] == fragileBlock){
                                        map[BlockY][BlockX] = fragileBlock;
                                        success = true;
                                        if(rand()%4==0){
                                            for(int box=0;box<fragileWallQuantity;box++){
                                                if(boxs[box].Pos.Y == BlockY && boxs[box].Pos.X == BlockX){
                                                    boxs[box].Pos.Y = -1;
                                                    boxs[box].Pos.X = -1;
                                                }
                                                if(boxs[box].Pos.Y == -1 && boxs[box].Pos.X == -1){
                                                    boxs[box].Pos.Y = BlockY;
                                                    boxs[box].Pos.X = BlockX;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(boss.HP<=0){
            boss.HP = 0;
            boss.Pos.Y = -1;
            boss.Pos.X = -1;
            boss.Alive = false;
        }
//----------------------------< SISTEMA DOS INIMIGOS <----------------------------//

//-----------------------------------> SISTEMA DAS BOMBAS >-----------------------------------//
        sound1 = 0;
        sound2 = 0;
        for(int bomb=0;bomb<maximumBombs;bomb++){
            if(sound1==0){
                sound1 = bombs_explosion(info,boss,bombs1, bomb, player1, player2, map, explosions1, enemysQuantity, enemys);
            }
            if(sound2==0){
                sound2 = bombs_explosion(info,boss,bombs2, bomb, player2, player1, map, explosions2, enemysQuantity, enemys);
            }
        }
        if(sound1 == 1 || sound2 == 1){
            explosaoSD.play();
        }
        if(sound1 == 2 || sound2 == 2){
            morteInimigoSD.play();
        }
//-----------------------------------< SISTEMA DAS BOMBAS <-----------------------------------//

//------------------------------> TIMER >------------------------------//
        if((clock()-timerClock)>=1000){
            if(portal.Pos.Y==-1 && portal.Pos.X==-1){
                if(player1.Alive){
                    player1.Points--;
                }
                if(player2.Alive){
                    player2.Points--;
                }
            }
            if(player1.Invincible>0){
                player1.Invincible--;
            }
            if(player2.Invincible>0){
                player2.Invincible--;
            }
            if(freezeEnemys>0){
                freezeEnemys--;
            }
            timerClock = clock();
            info.timer.Sec++;
            if(info.timer.Sec>=60){
                info.timer.Sec = 0;
                info.timer.Min++;
            }
            if(info.timer.Min>=60){
                info.timer.Min = 0;
                info.timer.Hour++;
            }
            if(info.timer.Hour>=60){
                return 0;
            }
        }
    }
}
//------------------------------< TIMER <------------------------------//

//-------------------------> SISTAMA DOS MENUS >-------------------------//
int main(){
    SetConsoleOutputCP(CP_UTF8);

    SoundBuffer movendoPeloMenuBF;
    if(!movendoPeloMenuBF.loadFromFile("Sounds/movendo_pelo_menu.wav")){}
    Sound movendoPeloMenuSD(movendoPeloMenuBF);
    movendoPeloMenuSD.setVolume(25);

    SoundBuffer musicaDerrotaBF;
    if(!musicaDerrotaBF.loadFromFile("Sounds/musica_derrota.wav")){}
    Sound musicaDerrotaSD(musicaDerrotaBF);
    musicaDerrotaSD.setVolume(50);
    musicaDerrotaSD.setLooping(true);

    SoundBuffer musicaMenuBF;
    if(!musicaMenuBF.loadFromFile("Sounds/musica_menu.wav")){}
    Sound musicaMenuSD(musicaMenuBF);
    musicaMenuSD.setVolume(10);
    musicaMenuSD.setLooping(true);
    musicaMenuSD.play();

    SoundBuffer musicaVitoriaBF;
    if(!musicaVitoriaBF.loadFromFile("Sounds/musica_vitoria.wav")){}
    Sound musicaVitoriaSD(musicaVitoriaBF);
    musicaVitoriaSD.setVolume(50);
    musicaVitoriaSD.setLooping(true);

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
        movendoPeloMenuSD.play();
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
                    int gameMenu = 1;
                    InfoType info;
                    cout << "\e[9;1H";
                    cout << " Nome: ";
                    cin>>info.name;
                    while(true){
                        bool kill = false;
                        cout << "\e[?25l\e[1;18H";
                        new_line("┏","━","┓",24);
                        cout << "\e[2;18H";
                        if(gameMenu == 1) {
                            if(info.difficulty==1){
                                cout << "┃\e[93m  [Dificuldade Facil]   \e[0m┃\n";
                            }
                            if(info.difficulty==2){
                                cout << "┃\e[93m  [Dificuldade Medía]   \e[0m┃\n";
                            }
                            if(info.difficulty==3){
                                cout << "┃\e[93m  [Dificuldade Dificil] \e[0m┃\n";
                            }
                        } else {
                            if(info.difficulty==1){
                                cout << "┃ [Dificuldade Facil]    ┃\n";
                            }
                            if(info.difficulty==2){
                                cout << "┃ [Dificuldade Medía]    ┃\n";
                            }
                            if(info.difficulty==3){
                                cout << "┃ [Dificuldade Dificil]  ┃\n";
                            }
                        }
                        cout << "\e[3;18H";
                        if(gameMenu == 2) {
                            if(info.players==1){
                                cout << "┃\e[93m  [CPU Player]          \e[0m┃\n";
                            }
                            if(info.players==2){
                                cout << "┃\e[93m  [Um Player]           \e[0m┃\n";
                            }
                            if(info.players==3){
                                cout << "┃\e[93m  [Dois Players]        \e[0m┃\n";
                            }
                        } else {
                            if(info.players==1){
                                cout << "┃ [CPU Player]           ┃\n";
                            }
                            if(info.players==2){
                                cout << "┃ [Um Player]            ┃\n";
                            }
                            if(info.players==3){
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
                        movendoPeloMenuSD.play();
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
                                    info.difficulty--;
                                    if(info.difficulty < 1){
                                        info.difficulty = 3;
                                    }
                                }
                                if(gameMenu==2){
                                    info.players--;
                                    if(info.players < 1){
                                        info.players = 3;
                                    }
                                }
                            break;
                            case 100: // Direira
                                if(gameMenu==1){
                                    info.difficulty++;
                                    if(info.difficulty > 3){
                                        info.difficulty = 1;
                                    }
                                }
                                if(gameMenu==2){
                                    info.players++;
                                    if(info.players > 3){
                                        info.players = 1;
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
                        musicaMenuSD.stop();
                        musicaVitoriaSD.stop();
                        musicaDerrotaSD.stop();
                        cout<<"\ec\e[?25l";
                        int deadMenu = -1;
                        if(game(info)){
                            info.maxPoints = info.player1.Points+info.player2.Points;
                            if(info.phase>3){
                                musicaVitoriaSD.play();
                                cout<<"\e[12;"<<mapSizeX+3<<"H";
                                cout << "┃ \e[38;5;46mVITORIA!\e[0m           ┃\n";
                                deadMenu = 1;
                                if(info.players>=2){
                                    fstream file;
                                    file.open("Scores.txt", ios::app);
                                    file<<info.name<<"\n";
                                    file<<info.maxPoints<<"\n";
                                    file.close();
                                }
                            }
                        }else{
                            musicaDerrotaSD.play();
                            cout<<"\e[12;"<<mapSizeX+3<<"H";
                            cout << "┃ \e[38;5;9mDERROTA!\e[0m           ┃\n";
                            deadMenu = 1;
                        }
                        cout<<"\e[9;"<<mapSizeX+3<<"H";
                        new_line("┏","━","┓",20);
                        cout<<"\e[10;"<<mapSizeX+3<<"H";
                        new_line("┃"," ","┃",20);
                        cout<<"\e[10;"<<mapSizeX+3<<"H";
                        cout<<"┃P: \e[38;5;46m"<<info.maxPoints<<"\e[0m";

                        cout<<"\e[11;"<<mapSizeX+3<<"H";
                        new_line("┣","━","┫",20);
                        cout<<"\e[13;"<<mapSizeX+3<<"H";
                        new_line("┃"," ","┃",20);

                        if(deadMenu==1){
                            while(true){
                                bool kill = false;
                                cout<<"\e[14;"<<mapSizeX+3<<"H";
                                if(deadMenu==1){
                                    cout<<"┃\e[93m  [REINICIAR]       \e[0m┃\n";
                                }else{
                                    cout<<"┃ [REINICIAR]        ┃\n";
                                }
                                cout<<"\e[15;"<<mapSizeX+3<<"H";
                                cout<<"┃                    ┃\n";
                                cout<<"\e[16;"<<mapSizeX+3<<"H";   
                                if(deadMenu==2){
                                    cout<<"┃\e[93m  [MENU]            \e[0m┃\n";
                                }else{
                                    cout<<"┃ [MENU]             ┃\n";
                                }
                                cout<<"\e[17;"<<mapSizeX+3<<"H";
                                new_line("┗","━","┛",20);
                                int key = getch();
                                movendoPeloMenuSD.play();
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
                                        if(deadMenu==3 || deadMenu==2){
                                            musicaVitoriaSD.stop();
                                            musicaDerrotaSD.stop();
                                            musicaMenuSD.play();
                                            success = false;
                                        }
                                        if(deadMenu==1 || deadMenu==3){
                                            info.timer = {0,0,0};
                                            info.phase = 1;
                                            info.player1.TotalMoves = 0;
                                            info.player1.Totalbombs = 0;
                                            info.player1.Points = 0;
                                            info.player1.Alive = true;
                                            info.player2.TotalMoves = 0;
                                            info.player2.Totalbombs = 0;
                                            info.player2.Points = 0;
                                            info.player2.Alive = true;
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
                        movendoPeloMenuSD.play();

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
                                    cout << "┃ Controles Jogador 1: WASD                    ┃\n";
                                    cout << "┃                                              ┃\n";
                                    cout << "┃                Cima                          ┃\n";
                                    cout << "┃                 ^                            ┃\n";
                                    cout << "┃                 W                            ┃\n";
                                    cout << "┃    Esquerda < A S D > Direita                ┃\n";
                                    cout << "┃                 V                            ┃\n";
                                    cout << "┃               Baixo                          ┃\n";
                                    cout << "┃                                              ┃\n";
                                    new_line("┣","━","┫",46);
                                    cout << "┃ ( Espace ) -> Colocar explosivo              ┃\n";
                                    new_line("┗","━","┛",46);
                                    new_line("┏","━","┓",46);
                                    cout << "┃ Controles Jogador 2: Setas                   ┃\n";
                                    cout << "┃                                              ┃\n";
                                    cout << "┃                Cima                          ┃\n";
                                    cout << "┃                 ^                            ┃\n";
                                    cout << "┃      Esquerda <   > Direita                  ┃\n";
                                    cout << "┃                 V                            ┃\n";
                                    cout << "┃               Baixo                          ┃\n";
                                    cout << "┃                                              ┃\n";
                                    new_line("┣","━","┫",46);
                                    cout << "┃ ( Enter ) -> Colocar explosivo               ┃\n";
                                    new_line("┗","━","┛",46);
                                    getch();
                                    cout << "\ec";
                                break;

                                case 5:
                                    new_line("┏","━","┓",80);
                                    cout << "┃ - Objetivo: Exploda todos os inimigos da fase, após isso, o portal para a      ┃\n";
                                    cout << "┃   próxima fase será aberta. São 3 fases ao total, com a última fase tendo      ┃\n";
                                    cout << "┃   um Chefão te esperando.                                                      ┃\n";
                                    cout << "┃ - Você começa apenas com uma vida, colete Vidas Extras para, bem, não morrer   ┃\n";
                                    cout << "┃   quando levar dano.                                                           ┃\n";
                                    cout << "┃ - Você pode só pode colocar um explosivo por vez, colete Multi-Detonares para  ┃\n";
                                    cout << "┃   poder colocar mais explosivos.                                               ┃\n";
                                    cout << "┃ - Ao colocar uma explosivo, não dá para passar por ela.                        ┃\n";
                                    cout << "┃ - As bombas conseguem destruir paredes frageis, mas não as solidas.            ┃\n";
                                    cout << "┃                                                                                ┃\n";
                                    cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ PONTUAÇÂO ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n";
                                    cout << "┃                                                                                ┃\n";
                                    cout << "┃ - O Jogador começa o jogo com 1000 pontos.                                     ┃\n";
                                    cout << "┃ - O Jogador perde 1 ponto por segundo.                                         ┃\n";
                                    cout << "┃ - Matar um inimigo = +250 pontos.                                              ┃\n";
                                    cout << "┃      Combo começa em 1                                                         ┃\n";
                                    cout << "┃      Matar mais de um inimigo ao mesmo tempo aumenta o combo em 0.33...        ┃\n";
                                    cout << "┃      A pontuação é multiplicada pelo combo para cada inimigo morto             ┃\n";
                                    cout << "┃      Calculo aproximado: (250*1)+(250*1,33..)+(250*1,66..)+(250*2)...          ┃\n";
                                    cout << "┃ - Causar dano ao Chefão = +500 pontos.                                         ┃\n";
                                    cout << "┃      Se 2 players estiverem vivos = +250 pontos (para ambos).                  ┃\n";
                                    cout << "┃ - Perder Vida = -250 pontos.                                                   ┃\n";
                                    cout << "┃ - Colocar uma bomba = -10 pontos.                                              ┃\n";
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
                                            paginaHorizontal = 2;
                                        }
                                        if (paginaHorizontal > 2) {
                                            paginaHorizontal = 1;
                                        }
                                        cout << "\e[?25l\e[H";
                                        new_line("┏","━","┓",26);
                                        cout << "┃       CODEX: ITENS       ┃\n";
                                        new_line("┣","━","┫",26);
                                        if (paginaVertical == 1) {
                                            if (paginaHorizontal == 1) {
                                                cout << "┃  \e[93m[Página 1 - PASSIVOS]\e[0m   ┃\n";
                                            }
                                            if (paginaHorizontal == 2) {
                                                cout << "┃  \e[93m[Página 2 - ATIVOS]\e[0m     ┃\n";
                                            }
                                            cout << "┃                          ┃\n";
                                        } else {
                                            if (paginaHorizontal == 1) {
                                                cout << "┃ [Página 1 - PASSIVOS]    ┃\n";
                                            }
                                            if (paginaHorizontal == 2) {
                                                cout << "┃ [Página 2 - ATIVOS]      ┃\n";
                                            }
                                            cout << "┃                          ┃\n";
                                        }
                                        if (paginaVertical == 2) {
                                            cout << "┃  \e[93m[Voltar]\e[0m                ┃\n";
                                        } else {
                                            cout << "┃ [Voltar]                 ┃\n";
                                        }
                                        new_line("┗","━","┛",26);
                                        cout<<"\e[?25l";
                                        switch (paginaHorizontal) {
                                            case 1:
                                                    new_line("┏","━","┓",109);
                                                    cout << "┃                                      PÁGINA 1 - PASSIVOS                                                    ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Bombas (◉): Mostra quantas bombas o jogador pode colocar ao mesmo tempo                                   ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Polvoras (◈): Mostra qual o raio da explosão                                                              ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Vidas (♥): Mostra a quantidade de vidas que o jogador tem                                                 ┃\n";
                                                    cout << "┃      Ao perder uma vida o jogador fica invencivel por 3 segundos.                                           ┃\n";
                                                    cout << "┃      E Todas as outras passivas são perdidas e resetadas para 1.                                            ┃\n";
                                                    new_line("┗","━","┛",109);
                                            break;

                                            case 2:
                                                    new_line("┏","━","┓",109);
                                                    cout << "┃                                      PÁGINA 2 - ATIVOS                                                      ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Espectro (⬘): Um traje experimental que permite o jogador atravesar paredes frágeis                       ┃\n";
                                                    cout << "┃    a cada uso em uma parede fragil o traje tem 10% de quebrar.                                              ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Crono-Hourglass (◊): Uma ampulheta capaz de congelar todos os inimigos ao chegar perto de                 ┃\n";
                                                    cout << "┃   um inimigo.                                                                                               ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Claymore (ᛟ): Um explosivo anti-pessoal, se detona quando alguma entidade passa por cima dela.            ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Aegis (☖): Um escudo que protege o jogador de ataques inimigos e ao ser utilizado mata todos ao redor    ┃\n";
                                                    cout << "┃   porém dobra o dano da bomba.                                                                              ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - ÔM3GA (Ω): Um explosivo com um grande poder de destruição, a explosão da 'ÔM3GA' irá                      ┃\n";
                                                    cout << "┃   se extender por todos os lados até chegar na borda do mapa, destruindo tudo pelo caminho: Paredes         ┃\n";
                                                    cout << "┃   fragéis, inimigos, jogadores.                                                                             ┃\n";
                                                    new_line("┣","━","┫",109);
                                                    cout << "┃ - Pandora (ᛰ): Uma bomba na qual faz uma trajetoria aleatória, e vai parar apenas ao                        ┃\n";
                                                    cout << "┃   acertar um jogador ou parede fragil podendo matar multiplos inimigos ao mesmo tempo.                      ┃\n";
                                                    new_line("┗","━","┛",109);
                                            break;
                                        }
                                        key = getch();
                                        movendoPeloMenuSD.play();
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
                                    mostrar_pontos();
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
