#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <chrono>

using namespace std;

// Keywords : MNU for menu related functions

//***************************definitions*********************************
#define W 1280
#define H 750
#define verticalline1x  140
#define verticalline2x  1140
#define horizontalliney  600
#define Ballradius 25
#define Balldecentspeed 0.4
#define firedballspeed  20
#define Maxballseachrow 20
#define explosionradius 3                        //can only be 1 or 2 or 3
#define timeeffectduration 2
#define timeforeachframe 10
#define timeintimedmode 90
#define numberofrowsinnormalmode 40
#define droppedballinitialspeedy 5
#define droppedballinitialspeedx 5
#define numberofframesinwhichtimeeffectsareactive 600
#define g 0.5
#define P1 15
#define P2 5
#define P3 15
#define P4 20
#define P5 25
#define P6 5
#define P7 25
#define P8 75
#define P9 79
#define P10 73
#define P11 90
#define P12 18
#define P13 8
#define PB1 25


//******************mahdi  structs *****************
const int initialPowerBarAbilityCount1= 5 ,initialPowerBarAbilityCount2= 3 ;
struct MNU_Menu{
    vector <SDL_Texture *> backgroundImage;
    //vector <SDL_Texture *> buttumImage;
    vector <SDL_Rect> buttonPlace;
    //int buttonCount;
    bool buttonsAreActive;
};

struct US_user{
    string username;
    long long score , scoreInMode[3];
    int bombCount,laserCount,multicolorBallCount,slowMotionCount,stopCount;
};

//*********************poorya structs *****************************************************
struct Point2D{
    int x;
    int y;
};
struct location{
    float x;
    float y;
    int typecode=0;
    int n=0;
};
struct Ball{
    float x;
    float y;
    float vx=0;
    float vy=0;
    bool isconnected=false;
    bool isempty=true;
    bool istarget=false;
    bool isflying=false;
    bool is2colored=false;
    bool islocked=false;
    bool isstone=false;
    bool ismulticolored=false;
    bool isexplosive=false;
    int typecode=0;
    int stopeffect=0;
    int sloweffect=0;
};

Ball rowofballs[50][Maxballseachrow];
int balltypecount[7];
bool rowclear[50];
vector <Ball> unboundballs;
vector <Point2D> tobedestroyedballs;
vector <Point2D> temptobedestroyedballs;
vector <location> explosionlocation;
vector <location> laserlocation;
vector <location> balldestructionlocation;
location UIbaranimationlocation[5];
vector<Mix_Chunk *> SFXs;
Ball firedball;
Ball backupball;
Point2D clickdown;
location endline={0,0};

int rowcounter=0;
float elapsedtime;  float temptime;
bool endoftheline=false;
bool checkneeded=false;
bool newgamestarted=true;
int destroyedballs=0;
int powerballsused=0;
bool isSlow=false,isStopped=false;
float ballSpeed=Balldecentspeed;
clock_t now;
bool reversetimeSFX1=false, reversetimeSFX2=false, reversetimeSFX3=false;
int randomrowassist;
//******************************** poorya function declaration******************************
void assigntimeeffect();
void generateemptyrow(int row);
void generaterandomrow (int row);
void copyballs(Ball &ball1,Ball &ball2);
void updateflags(Ball &ball);
bool coinflip(int probablity);
int randomtype(int i,int j);
int assignedcode(int i,int j,int typecode);
int compcode(int i,int j,int colorcode);
bool isaround(int i,int j,int colorcode);
bool iscompatible(int typdecode,int colorcode);
void initializegame();
void shiftmyballs();
void calculateangle(float &angle,SDL_Event *e,SDL_Point anchorPoint);
void drawguideline(SDL_Renderer * m_renderer,SDL_Point anchorPoint,SDL_Point Point1,vector<SDL_Texture *> balls);
void drawlaserguideline(SDL_Renderer * m_renderer,SDL_Point anchorPoint,SDL_Point Point1);
void findPoint2(SDL_Point anchorPoint,SDL_Point Point1,SDL_Point &Point2);
void drawmyballs(SDL_Renderer * m_renderer,vector<SDL_Texture *> balls);
void drawanimationframes(SDL_Renderer * m_renderer,vector <vector<SDL_Texture *>> animations,int totalframes,int linger);
void drawthisball(SDL_Renderer * m_renderer,int x,int y,int typecode,vector<SDL_Texture *> balls,int &sloweffect, int &stopeffect);
void movemyballs(float dy);
void createfiredball(US_user& user);
void createbackupball(US_user& user);
void assignfiredballcode(int &result);
void updateballtypecount();
void assignvelocity(Ball &ball);
void updaterowclear();
void transformemptyball(Ball &ball1,Ball &ball2);
void examinemyballs();
void addhomiestovector(int i,int j,const int typecode);        //creates a vector of compatible balls near starting location of (i,j)
void addtemphomiestovector(int i,int j,int typecode);
void multicolored(int i,int j);
bool alreadyhere(vector<Point2D> homies,int i,int j);
void removeredundancy(vector <Point2D> &vector1);
bool comparePoint2D(Point2D Point1, Point2D Point2);
void settlefiredball(int i,int j,bool &b,int &x,int&y);         //settles the fired ball near i,j location (depending on firedball's location)
void settlenearendline(Ball &firedball);
void checktimeeffectcancellation(int i, int j);
void dropmyballs();
void destroymyballs(vector<Point2D> &targets);
void explodemyballs(int i,int j,int n);
float distancefromline(int x,int y,int x1,int y1,int x2,int y2);
void lasermyballs(int x,int y);
void identifyunboundballs();
void connectmyballs(int i,int j);
void playsound(int SFXcode);
int calculatescore(int playModeCode);
void checkvictoryorloss2(int playModeCode , MNU_Menu& win,MNU_Menu& loss);
//*********************mahdi function declarations********************************************
bool MNU_ClickOnButton (const MNU_Menu &menu , int indexOfButton, SDL_Event * e);
void MNU_MenuPresent (SDL_Renderer * renderer , const MNU_Menu &menu, int mode , int Ww , int Hh);
void MNU_ShowVictory (SDL_Renderer * renderer ,  MNU_Menu &menu , MNU_Menu &mainMenu , US_user &user, SDL_Event * e , int & mode,vector<US_user>& list, bool &b, int playMode);
void MNU_ShowGameOver (SDL_Renderer * renderer ,  MNU_Menu &menu , MNU_Menu &mainMenu , US_user user, SDL_Event * e,vector<US_user>& list , bool b, int playMode);
void MNU_DestroyMenu (MNU_Menu &menu);
bool US_sortHelper(US_user a, US_user b);
bool US_sortHelperM0(US_user a, US_user b);
bool US_sortHelperM1(US_user a, US_user b);
bool US_sortHelperM2(US_user a, US_user b);
void US_showScores(SDL_Renderer * renderer , vector<US_user>& list,int x, int y ,int colorMode , int showScoreMode=100);
void US_showPowerBarItemsCount(SDL_Renderer * renderer , const US_user& user , int playModeCode);
int US_searchForUser( vector<US_user>& list,const string& name);
void US_extractString(const string &s, vector<string> &expr);
void US_finalUpdateAfterGame (vector<US_user>& userList, US_user& newUser, int playMode);
void textRGBA2 (SDL_Renderer*m_renderer, int x, int y, const char* text,int f_size,Uint8 rr, Uint8 gg, Uint8 bb, Uint8 aa);
void loadAllanimations(SDL_Renderer *renderer ,vector <vector<SDL_Texture *>> &animations);
void DestroyAnimations (vector<vector<SDL_Texture*>> &all);
void loadBalls(SDL_Renderer * m_renderer ,vector<SDL_Texture *> &balls);

double mulW=0.67,mulH=0.695;
//**************************************************************************************************
//***************************************************************************************************

int main(int argc, char * argv[]) {
    srand(time(NULL));

    //Initialization of SDL windows
    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER ;
//    Uint32 WND_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;//SDL_WINDOW_BORDERLESS ;
    Uint32 WND_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS;//SDL_WINDOW_BORDERLESS ;

    SDL_Window * m_window;
    SDL_Renderer * m_renderer;
    SDL_Texture *bkImg = NULL;
    //Texture for loading image
    int img_w, img_h;
    SDL_Rect img_rect;

    SDL_Init( SDL_flags );
    SDL_CreateWindowAndRenderer( W, H, WND_flags, &m_window, &m_renderer );
    //Pass the focus to the drawing window
    SDL_RaiseWindow(m_window);
    //Get screen resolution
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);

    int Ww = 1280;
    int Hh = 750;
    bkImg = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, W, H);

////*****************************************************************
    //loading all animations
    vector <vector<SDL_Texture *>> animations;
    loadAllanimations(m_renderer,animations);

    //loading ball textures:
    vector<SDL_Texture *> balls;
    loadBalls(m_renderer,balls);

    //play field and power bar related:
    int playModeCode; // 0 for normal , 1 for timed , 2 for infinite;



    int victoryMode =0;
    //user related:
    bool userIsInterred = false;
    string textInput;
    vector<US_user> userList;
    US_user newUser;newUser.score=0;newUser.scoreInMode[0]=0;newUser.scoreInMode[1]=0;newUser.scoreInMode[2]=0;

    ifstream inputUsers;
    inputUsers.open("files/inputUsers.txt");

    if (inputUsers.good()){
        US_user temp;
        string inputLine;
        vector<string> parts;
        while(!inputUsers.eof()){
            inputUsers>>inputLine;
            if(inputUsers.good()) {
                US_extractString(inputLine, parts);
                temp.username = parts[0];
                temp.score = stoll(parts[1]);
                temp.bombCount = stoi(parts[2]);
                temp.laserCount = stoi(parts[3]);
                temp.multicolorBallCount = stoi(parts[4]);
                temp.slowMotionCount = stoi(parts[5]);
                temp.stopCount = stoi(parts[6]);
                temp.scoreInMode[0] = stoll(parts[7]);
                temp.scoreInMode[1] = stoll(parts[8]);
                temp.scoreInMode[2] = stoll(parts[9]);

                userList.push_back(temp);
                parts.clear();
            }
        }
        inputUsers.close();
    }else{
        cout<<"error!";
    }

    //audio related:
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,204);

    vector <Mix_Music *> bgm;

    int playBgm , playBgmOut , playSFX, playSFXOut ,  playBgmVolume , playBgmVolumeOut;
    bgm.push_back(Mix_LoadMUS("sounds/backgroundmusic1.mp3"));//playBgm=0
    bgm.push_back(Mix_LoadMUS("sounds/29-He's a Pirate (Album).mp3"));//playBgm=1
    bgm.push_back(Mix_LoadMUS("sounds/backgroundmusic3.mp3"));//playBgm=2
    bgm.push_back(Mix_LoadMUS("sounds/backgroundmusic4.mp3"));//playBgm=3
    bgm.push_back(Mix_LoadMUS("sounds/backgroundmusic5.mp3"));//playBgm=4
    bgm.push_back(Mix_LoadMUS("sounds/backgroundmusic6.mp3"));//playBgm=5

    //loading SFXs:
    SFXs.push_back(Mix_LoadWAV("sounds/laser.mp3"));//laser=0
    SFXs.push_back(Mix_LoadWAV("sounds/explosion.mp3"));//explosion=1
    SFXs.push_back(Mix_LoadWAV("sounds/ballfired.mp3"));//ballfired=2
    SFXs.push_back(Mix_LoadWAV("sounds/greetings1.mp3"));//greetings1=3
    SFXs.push_back(Mix_LoadWAV("sounds/greetings2.mp3"));//greetings2=4
    SFXs.push_back(Mix_LoadWAV("sounds/defeat1.mp3"));//defeat1=5
    SFXs.push_back(Mix_LoadWAV("sounds/defeat2.mp3"));//defeat2=6
    SFXs.push_back(Mix_LoadWAV("sounds/defeat3.mp3"));//defeat3=7
    SFXs.push_back(Mix_LoadWAV("sounds/victory1.mp3"));//victory1=8
    SFXs.push_back(Mix_LoadWAV("sounds/victory2.mp3"));//victory2=9
    SFXs.push_back(Mix_LoadWAV("sounds/victory3.mp3"));//victory2=10
    SFXs.push_back(Mix_LoadWAV("sounds/30seconds.mp3"));//30seconds=11
    SFXs.push_back(Mix_LoadWAV("sounds/10seconds.mp3"));//10seconds=12
    SFXs.push_back(Mix_LoadWAV("sounds/5seconds.mp3"));//5seconds=13
    SFXs.push_back(Mix_LoadWAV("sounds/slow.mp3"));//slow=14
    SFXs.push_back(Mix_LoadWAV("sounds/stop.mp3"));//stop=15
    SFXs.push_back(Mix_LoadWAV("sounds/meepmerp.mp3"));//meepmerp=16

    //setting related:
    ifstream inputSetting;
    inputSetting.open("files/inputSetting.txt");

    int backToWhereAfterSetting=0;//0:go back to main menu , 1:go back to pause menu;
    int colorMode ; // 0 is dark and 1 is light;
    int showScoreMode=100;
    int colorModeOfPlayField ; // 0 is dark and 1 is light and 2 is pirates theme;
    bool quitGame = false;

    if (inputSetting.good()){
        inputSetting>>colorMode>>colorModeOfPlayField>>playBgm>>playSFX>>playBgmVolume;
        inputSetting.close();
    }

        //main menu buttons:
    SDL_Rect button0; button0.w = 530*mulW ; button0.h = 152*mulH ; button0.x = 695*mulW; button0.y = 307*mulH;
    SDL_Rect button1; button1.w = 486*mulW ; button1.h = 152*mulH ; button1.x = 717*mulW; button1.y = 486*mulH;
    SDL_Rect button2; button2.w = 486*mulW ; button2.h = 152*mulH ; button2.x = 717*mulW; button2.y = 663*mulH;
    SDL_Rect button3; button3.w = 177*mulW ; button3.h = 95*mulH ; button3.x = 777*mulW; button3.y = 840*mulH;
    SDL_Rect button4; button4.w = 177*mulW ; button4.h = 95*mulH ; button4.x = 968*mulW; button4.y = 840*mulH;
        //return menu buttons:
    SDL_Rect returnButton; returnButton.w = 97*mulW ; returnButton.h = 84*mulH ; returnButton.x = 31*mulW ; returnButton.y = 38*mulH;
    SDL_Rect returnButtonGameOver; returnButtonGameOver.w = 330*mulW ; returnButtonGameOver.h = 123*mulH ; returnButtonGameOver.x = 330*mulW ; returnButtonGameOver.y = 522*mulH;
    SDL_Rect returnButtonVictory; returnButtonVictory.w = 330*mulW ; returnButtonVictory.h = 123*mulH ; returnButtonVictory.x = 493*mulW ; returnButtonVictory.y = 739*mulH;
    SDL_Rect a1; a1.w = 104*mulW ; a1.h = 104*mulH ; a1.x = 513*mulW ; a1.y = 461*mulH;
    SDL_Rect a2; a2.w = 104*mulW ; a2.h = 104*mulH ; a2.x = 616*mulW ; a2.y = 461*mulH;
    SDL_Rect a3; a3.w = 104*mulW ; a3.h = 104*mulH ; a3.x = 714*mulW ; a3.y = 461*mulH;
    //play mode menu buttons:
    SDL_Rect button1PM; button1PM.w = 486*mulW ; button1PM.h = 152*mulH ; button1PM.x = 432*mulW; button1PM.y = 304*mulH;
    SDL_Rect button2PM; button2PM.w = 486*mulW ; button2PM.h = 152*mulH ; button2PM.x = 432*mulW; button2PM.y = 482*mulH;
    SDL_Rect button3PM; button3PM.w = 486*mulW ; button3PM.h = 152*mulH ; button3PM.x = 432*mulW; button3PM.y = 660*mulH;
    //get username menu buttons:
    SDL_Rect confirmButton; confirmButton.w = 150*mulW ; confirmButton.h = 150*mulH ; confirmButton.x = 1185*mulW; confirmButton.y = 495*mulH;
        //play field buttons;
    SDL_Rect changeBallsButton;changeBallsButton.w = 84*mulW ; changeBallsButton.h = 93*mulH ; changeBallsButton.x = 1128*mulW; changeBallsButton.y = 942*mulH;
        //pause menu buttons:
    SDL_Rect Resume; Resume.w = 486*mulW ; Resume.h = 152*mulH ; Resume.x = 717*mulW; Resume.y = 465*mulH;
    SDL_Rect Exit; Exit.w = 486*mulW ; Exit.h = 152*mulH ; Exit.x = 191*mulW; Exit.y = 465*mulH;
    SDL_Rect Setting; Setting.w = 486*mulW ; Setting.h = 152*mulH ; Setting.x = 1242*mulW; Setting.y = 465*mulH;
    //scoreboard buttons:
    vector <SDL_Rect>score(4);
    score[0]; score[0].w = 248*mulW ; score[0].h = 78*mulH ; score[0].x = 426*mulW; score[0].y = 229*mulH;
    score[1]; score[1].w = 248*mulW ; score[1].h = 78*mulH ; score[1].x = 698*mulW; score[1].y = 229*mulH;
    score[2]; score[2].w = 248*mulW ; score[2].h = 78*mulH ; score[2].x = 970*mulW; score[2].y = 229*mulH;
    score[3]; score[3].w = 248*mulW ; score[3].h = 78*mulH ; score[3].x = 1242*mulW; score[3].y = 229*mulH;

    //setting menu buttons:
    SDL_Rect themes; themes.w = 486*mulW ; themes.h = 152*mulH ; themes.x = 191*mulW; themes.y = 158*mulH;
    SDL_Rect sfx; sfx.w = 486*mulW ; sfx.h = 152*mulH ; sfx.x = 717*mulW; sfx.y = 158*mulH;
    SDL_Rect soundS; soundS.w = 486*mulW ; soundS.h = 152*mulH ; soundS.x = 1242*mulW; soundS.y = 158*mulH;
            //setting menu soundS buttons:
    vector <SDL_Rect>volume(5);
    SDL_Rect soundT1; soundT1.w = 177*mulW ; soundT1.h = 111*mulH ; soundT1.x = 397*mulW; soundT1.y = 614*mulH;
    SDL_Rect soundT2; soundT2.w = 177*mulW ; soundT2.h = 111*mulH ; soundT2.x = 589*mulW; soundT2.y = 614*mulH;
    SDL_Rect soundT3; soundT3.w = 177*mulW ; soundT3.h = 111*mulH ; soundT3.x = 782*mulW; soundT3.y = 614*mulH;
    SDL_Rect soundT4; soundT4.w = 177*mulW ; soundT4.h = 111*mulH ; soundT4.x = 973*mulW; soundT4.y = 614*mulH;
    SDL_Rect soundT5; soundT5.w = 177*mulW ; soundT5.h = 111*mulH ; soundT5.x = 1167*mulW; soundT5.y = 614*mulH;
    SDL_Rect soundT6; soundT6.w = 177*mulW ; soundT6.h = 111*mulH ; soundT6.x = 1358*mulW; soundT6.y = 614*mulH;
    volume[0]; volume[0].w = 42*mulW ; volume[0].h = 42*mulH ; volume[0].x = 767*mulW; volume[0].y = 479*mulH;
    volume[1]; volume[1].w = 62*mulW ; volume[1].h = 62 *mulH; volume[1].x = 928*mulW; volume[1].y = 471*mulH;
    volume[2]; volume[2].w = 81*mulW ; volume[2].h = 81*mulH ; volume[2].x = 1087*mulW; volume[2].y = 465*mulH;
    volume[3]; volume[3].w = 101*mulW ; volume[3].h = 101*mulH ; volume[3].x = 1247*mulW; volume[3].y = 458*mulH;
    volume[4]; volume[4].w = 121*mulW ; volume[4].h = 121*mulH ; volume[4].x = 1408*mulW; volume[4].y = 452*mulH;
    SDL_Rect mute; mute.w = 356*mulW ; mute.h = 111*mulH ; mute.x = 589*mulW; mute.y = 777*mulH;
    SDL_Rect unmute; unmute.w = 356*mulW ; unmute.h = 111*mulH ; unmute.x = 974*mulW; unmute.y = 777*mulH;
            //setting menu themeS buttons:
    SDL_Rect darkTH; darkTH.w = 503*mulW ; darkTH.h = 157*mulH ; darkTH.x = 980*mulW; darkTH.y = 490*mulH;
    SDL_Rect lightTH; lightTH.w = 503*mulW ; lightTH.h = 157 *mulH; lightTH.x = 437*mulW; lightTH.y = 490*mulH;
    SDL_Rect darkTHPlay; darkTHPlay.w = 331*mulW ; darkTHPlay.h = 103*mulH ; darkTHPlay.x = 794*mulW; darkTHPlay.y = 839*mulH;
    SDL_Rect lightTHPlay; lightTHPlay.w = 331*mulW ; lightTHPlay.h = 103*mulH ; lightTHPlay.x = 437*mulW; lightTHPlay.y = 839*mulH;
    SDL_Rect piratesPlay; piratesPlay.w = 331*mulW ; piratesPlay.h = 103 *mulH; piratesPlay.x = 1152*mulW; piratesPlay.y = 839*mulH;

    //power bar buttons:
    SDL_Rect slowMotion; slowMotion.w = 160*mulW ; slowMotion.h = 160*mulH ; slowMotion.x = 1720*mulW; slowMotion.y = 197*mulH;
    SDL_Rect stopBalls; stopBalls.w = 160*mulW ; stopBalls.h = 160*mulH ; stopBalls.x = 1720*mulW; stopBalls.y = 385*mulH;
    UIbaranimationlocation[0]={19,123}; UIbaranimationlocation[1]={19,301}; UIbaranimationlocation[2]={19,479};
    UIbaranimationlocation[3]={1220,208}; UIbaranimationlocation[4]={1220,318};

    ////
    SDL_Texture * ass = IMG_LoadTexture(m_renderer,"images/ass.png");
    SDL_Rect assRect; assRect.x = -600 ; assRect.y = -600;
    SDL_Texture * ass2 = IMG_LoadTexture(m_renderer,"images/ass.png");
    SDL_Rect assRect2; assRect2.x = -600 ; assRect2.y = -600;

    SDL_Texture * box = IMG_LoadTexture(m_renderer,"images/box.png");
    SDL_Rect boxRect=score[0];

    //cannon related:
    SDL_Texture * cannon = IMG_LoadTexture(m_renderer,"images/cannon.png");
    SDL_Rect cannonRect; cannonRect.w = 108 ; cannonRect.h = 130 ; cannonRect.x = W/2 - 54 ; cannonRect.y = 600;
    SDL_Texture * cannonup = IMG_LoadTexture(m_renderer,"images/cannonUp.png");
    SDL_Rect cannonRect2; cannonRect2.w = 108 ; cannonRect2.h = 130 ; cannonRect2.x = W/2 - 54 ; cannonRect2.y = 600;
    SDL_Point anchorPoint;anchorPoint.x= W/2 ;anchorPoint.y = 665;
    SDL_Point Point1,Point2;

    MNU_Menu mainMenu , playMode , getUsername, setting , scoreboard , playField , musicSetting ,themeSetting,SFXSetting,pauseMenu, gameOver,victoryM , help;

    mainMenu.buttonsAreActive = true;
    mainMenu.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/1.png"));
    mainMenu.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/1lightMode.png"));
    mainMenu.buttonPlace.push_back(button0);
    mainMenu.buttonPlace.push_back(button1);
    mainMenu.buttonPlace.push_back(button2);
    mainMenu.buttonPlace.push_back(button3);
    mainMenu.buttonPlace.push_back(button4);


    playMode.buttonsAreActive = false;
    playMode.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/2.png"));
    playMode.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/2lightMode.png"));
    playMode.buttonPlace.push_back(returnButton);
    playMode.buttonPlace.push_back(button1PM);
    playMode.buttonPlace.push_back(button2PM);
    playMode.buttonPlace.push_back(button3PM);

    getUsername.buttonsAreActive = false;
    getUsername.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/33.png"));
    getUsername.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/3lightMode.png"));
    getUsername.buttonPlace.push_back(returnButton);
    getUsername.buttonPlace.push_back(confirmButton);

    help.buttonsAreActive = false;
    help.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/help.png"));
    help.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/helpLight.png"));
    help.buttonPlace.push_back(returnButton);

    pauseMenu.buttonsAreActive = false;
    pauseMenu.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/pauseMenu.png"));
    pauseMenu.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/pauseMenuLight.png"));
    pauseMenu.buttonPlace.push_back(Exit);
    pauseMenu.buttonPlace.push_back(Resume);
    pauseMenu.buttonPlace.push_back(Setting);

    scoreboard.buttonsAreActive = false;
    scoreboard.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/4.png"));
    scoreboard.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/4lightMode.png"));
    scoreboard.buttonPlace.push_back(returnButton);
    scoreboard.buttonPlace.push_back(score[0]);
    scoreboard.buttonPlace.push_back(score[1]);
    scoreboard.buttonPlace.push_back(score[2]);
    scoreboard.buttonPlace.push_back(score[3]);

    setting.buttonsAreActive = false;
    setting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/5.png"));
    setting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/5lightMode.png"));
    setting.buttonPlace.push_back(returnButton);
    setting.buttonPlace.push_back(themes);
    setting.buttonPlace.push_back(sfx);
    setting.buttonPlace.push_back(soundS);

    musicSetting.buttonsAreActive = false;
    musicSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/6.png"));
    musicSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/6lightMode.png"));
    musicSetting.buttonPlace.push_back(returnButton);
    musicSetting.buttonPlace.push_back(soundT1);
    musicSetting.buttonPlace.push_back(soundT2);
    musicSetting.buttonPlace.push_back(soundT3);
    musicSetting.buttonPlace.push_back(volume[0]);
    musicSetting.buttonPlace.push_back(volume[1]);
    musicSetting.buttonPlace.push_back(volume[2]);
    musicSetting.buttonPlace.push_back(volume[3]);
    musicSetting.buttonPlace.push_back(volume[4]);
    musicSetting.buttonPlace.push_back(mute);
    musicSetting.buttonPlace.push_back(unmute);
    musicSetting.buttonPlace.push_back(themes);
    musicSetting.buttonPlace.push_back(sfx);
    musicSetting.buttonPlace.push_back(soundT4);
    musicSetting.buttonPlace.push_back(soundT5);
    musicSetting.buttonPlace.push_back(soundT6);

    themeSetting.buttonsAreActive = false;
    themeSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/7.png"));
    themeSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/7lightMode.png"));
    themeSetting.buttonPlace.push_back(returnButton);
    themeSetting.buttonPlace.push_back(lightTH);
    themeSetting.buttonPlace.push_back(darkTH);
    themeSetting.buttonPlace.push_back(lightTHPlay);
    themeSetting.buttonPlace.push_back(darkTHPlay);
    themeSetting.buttonPlace.push_back(piratesPlay);
    themeSetting.buttonPlace.push_back(sfx);
    themeSetting.buttonPlace.push_back(soundS);

    SFXSetting.buttonsAreActive = false;
    SFXSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/8.png"));
    SFXSetting.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/8lightMode.png"));
    SFXSetting.buttonPlace.push_back(returnButton);
    SFXSetting.buttonPlace.push_back(volume[0]);
    SFXSetting.buttonPlace.push_back(volume[1]);
    SFXSetting.buttonPlace.push_back(volume[2]);
    SFXSetting.buttonPlace.push_back(volume[3]);
    SFXSetting.buttonPlace.push_back(volume[4]);
    SFXSetting.buttonPlace.push_back(mute);
    SFXSetting.buttonPlace.push_back(unmute);
    SFXSetting.buttonPlace.push_back(themes);
    SFXSetting.buttonPlace.push_back(soundS);

    playField.buttonsAreActive = false;
    playField.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/playLight.png"));
    playField.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/playDark.png"));
    playField.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/playfield.png"));
    playField.buttonPlace.push_back(returnButton);
    playField.buttonPlace.push_back(changeBallsButton);

    gameOver.buttonsAreActive = false;
    gameOver.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/9.png"));
    gameOver.buttonPlace.push_back(returnButtonGameOver);

    victoryM.buttonsAreActive = false;
    victoryM.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/102.png"));
    victoryM.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/a1.png"));
    victoryM.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/a2.png"));
    victoryM.backgroundImage.push_back(IMG_LoadTexture(m_renderer,"images/a3.png"));
    victoryM.buttonPlace.push_back(returnButtonVictory);
    victoryM.buttonPlace.push_back(a1);
    victoryM.buttonPlace.push_back(a2);
    victoryM.buttonPlace.push_back(a3);

    //for balls:
    for (int i=1;i<=50;i++){
        generateemptyrow(i);
        rowclear[i-1]=true;
    }
    for (int i=21;i<=50;i++){
        generaterandomrow(i);
        rowclear[i-1]=false;
    }
    createfiredball(newUser);
    createbackupball(newUser);

    SDL_Event * e = new SDL_Event ;
    SDL_StartTextInput();
    clock_t gamestarted;
    clock_t frame1,frame2;

    switch (playSFX) {
        case 20:
            assRect=volume[0];
            break;
        case 40:
            assRect=volume[1];
            break;
        case 60:
            assRect=volume[2];
            break;
        case 96:
            assRect=volume[3];
            break;
        case 128:
            assRect=volume[4];
            break;
        default:
            break;
    }
    switch (playBgmVolume) {
        case 10:
            assRect2=volume[0];
            break;
        case 30:
            assRect2=volume[1];
            break;
        case 60:
            assRect2=volume[2];
            break;
        case 96:
            assRect2=volume[3];
            break;
        case 128:
            assRect2=volume[4];
            break;
        default:
            break;
    }

    //start!:
    int framecount=0;
    chrono::system_clock::time_point time1; chrono::system_clock::time_point time2;//4
    while (!quitGame){
        frame1=clock();
        SDL_PollEvent(e);

        //sound related part:
        if(playBgm>=0) {
            Mix_PlayMusic(bgm[playBgm-1], -1);
            playBgmOut=playBgm;
        }
        playBgm=-1;

        if(playBgmVolume>=0) {
            Mix_VolumeMusic(playBgmVolume);
            playBgmVolumeOut=playBgmVolume;

        }
        playBgmVolume=-1;
        //SFX initial volume:
        if(playSFX>=0){
            for (auto & SFX : SFXs) {
                Mix_VolumeChunk(SFX,playSFX);
            }
            playSFXOut=playSFX;
        }
        playSFX=-1;


        //menu related part:
        if (mainMenu.buttonsAreActive){
            MNU_MenuPresent(m_renderer,mainMenu,colorMode,Ww,Hh);

            if (MNU_ClickOnButton(mainMenu,0,e)){
                getUsername.buttonsAreActive = true;
                mainMenu.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(mainMenu,1,e)){
                scoreboard.buttonsAreActive = true;
                mainMenu.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(mainMenu,2,e)){
                e->button.button=0;
                backToWhereAfterSetting=0;
                mainMenu.buttonsAreActive = false;
                setting.buttonsAreActive = true;

            }
            else if (MNU_ClickOnButton(mainMenu,3,e)){
                quitGame = true;
            }
            else if (MNU_ClickOnButton(mainMenu,4,e)){
                help.buttonsAreActive = true;
                mainMenu.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);
        }

        else if (getUsername.buttonsAreActive){
            MNU_MenuPresent(m_renderer,getUsername,colorMode,Ww,Hh);

            //text input;
            if (e->type == SDL_TEXTINPUT && textInput.length() < 21){///////
                system("cls");
                textInput += e->text.text;
            } else if (e->key.keysym.sym == SDLK_BACKSPACE && e->type == SDL_KEYDOWN && textInput.length() > 0 ){
                system("cls");
                textInput = textInput.substr(0,textInput.length()-1);
            }
            textRGBA2(m_renderer,210*mulW,530*mulH,textInput.c_str(),37,156,182,214,255);

            //menu buttons controller;
            if (MNU_ClickOnButton(getUsername,0,e) || e->key.keysym.sym==SDLK_ESCAPE){
                mainMenu.buttonsAreActive = true;
                getUsername.buttonsAreActive = false;
            } else if (textInput.length()!=0 && MNU_ClickOnButton(getUsername,1,e)){

                userIsInterred= false;
                newUser.username=textInput;
                newUser.score=0;newUser.scoreInMode[0]=0;newUser.scoreInMode[1]=0;newUser.scoreInMode[2]=0;

                // give initial abilities to new users:
                if (US_searchForUser(userList,textInput)==-1){
                    newUser.stopCount=newUser.slowMotionCount= initialPowerBarAbilityCount1;
                    newUser.multicolorBallCount=newUser.laserCount=newUser.bombCount= initialPowerBarAbilityCount2;
                } else // find count of abilities of old users:
                {
                    newUser.stopCount=userList[US_searchForUser(userList,textInput)].stopCount;
                    newUser.slowMotionCount=userList[US_searchForUser(userList,textInput)].slowMotionCount;
                    newUser.bombCount=userList[US_searchForUser(userList,textInput)].bombCount;
                    newUser.laserCount=userList[US_searchForUser(userList,textInput)].laserCount;
                    newUser.multicolorBallCount=userList[US_searchForUser(userList,textInput)].multicolorBallCount;
                }

                textInput="";

                getUsername.buttonsAreActive = false;
                playMode.buttonsAreActive = true;
            }
            SDL_RenderPresent(m_renderer);

        }

        else if (playMode.buttonsAreActive){
            MNU_MenuPresent(m_renderer,playMode,colorMode,Ww,Hh);

            if (MNU_ClickOnButton(playMode,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                mainMenu.buttonsAreActive = true;
                playMode.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(playMode,1,e)){
                playModeCode=0;
                time1 =chrono::high_resolution_clock::now();//3
                playField.buttonsAreActive = true;
                playMode.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(playMode,2,e)){
                playModeCode=1;
                time1 =chrono::high_resolution_clock::now();//3
                playField.buttonsAreActive = true;
                playMode.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(playMode,3,e)){
                playModeCode=2;
                time1 =chrono::high_resolution_clock::now();//3
                playField.buttonsAreActive = true;
                playMode.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);
        }
        else if (pauseMenu.buttonsAreActive){
            MNU_MenuPresent(m_renderer,pauseMenu,colorMode,Ww,Hh);

            if (MNU_ClickOnButton(pauseMenu,0,e)){
                initializegame();
                createfiredball(newUser);
                createbackupball(newUser);

                if(!userIsInterred){
                    US_finalUpdateAfterGame (userList,  newUser,playModeCode);
                    userIsInterred=true;
                }

                mainMenu.buttonsAreActive = true;
                pauseMenu.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(pauseMenu,1,e)){
                playField.buttonsAreActive = true;
                pauseMenu.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(pauseMenu,2,e)){
                backToWhereAfterSetting=1;
                setting.buttonsAreActive = true;
                pauseMenu.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);

        }

        else if (scoreboard.buttonsAreActive){
            MNU_MenuPresent(m_renderer,scoreboard,colorMode,Ww,Hh);
            SDL_RenderCopy(m_renderer,box,NULL,&boxRect);

            US_showScores(m_renderer,userList,310,215,colorMode,showScoreMode);

            if (MNU_ClickOnButton(scoreboard,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                mainMenu.buttonsAreActive = true;
                scoreboard.buttonsAreActive = false;
            }
            if (MNU_ClickOnButton(scoreboard,1,e)){
                boxRect=score[0];
                showScoreMode=100;
            }
            if (MNU_ClickOnButton(scoreboard,2,e)){
                boxRect=score[1];
                showScoreMode=0;
            }
            if (MNU_ClickOnButton(scoreboard,3,e)){
                boxRect=score[2];
                showScoreMode=1;
            }
            if (MNU_ClickOnButton(scoreboard,4,e)){
                boxRect=score[3];
                showScoreMode=2;
            }
            SDL_RenderPresent(m_renderer);

        }
        else if (help.buttonsAreActive){
            MNU_MenuPresent(m_renderer,help,colorMode,Ww,Hh);
            if (MNU_ClickOnButton(help,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                mainMenu.buttonsAreActive = true;
                help.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);
        }

        else if (setting.buttonsAreActive){

            MNU_MenuPresent(m_renderer,setting,colorMode,Ww,Hh);

            if (MNU_ClickOnButton(setting,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                if (backToWhereAfterSetting==1) {
                    pauseMenu.buttonsAreActive = true;
                    setting.buttonsAreActive = false;
                }
                else {
                    mainMenu.buttonsAreActive = true;
                    setting.buttonsAreActive = false;
                }
            }
            else if (MNU_ClickOnButton(setting,1,e)){
                themeSetting.buttonsAreActive = true;
                setting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(setting,2,e)){
                SFXSetting.buttonsAreActive = true;
                setting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(setting,3,e)){
                musicSetting.buttonsAreActive = true;
                setting.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);

        }
        else if (themeSetting.buttonsAreActive){
            MNU_MenuPresent(m_renderer,themeSetting,colorMode,Ww,Hh);

            if (MNU_ClickOnButton(themeSetting,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                if (backToWhereAfterSetting==1) {
                    pauseMenu.buttonsAreActive = true;
                    themeSetting.buttonsAreActive = false;
                }
                else {
                    mainMenu.buttonsAreActive = true;
                    themeSetting.buttonsAreActive = false;
                }
            }
            else if (MNU_ClickOnButton(themeSetting,1,e)){
                colorMode=1;
            }
            else if (MNU_ClickOnButton(themeSetting,2,e)){
                colorMode=0;
            }
            else if (MNU_ClickOnButton(themeSetting,3,e)){
                colorModeOfPlayField=0;
            }
            else if (MNU_ClickOnButton(themeSetting,4,e)){
                colorModeOfPlayField=1;
            }
            else if (MNU_ClickOnButton(themeSetting,5,e)){
                colorModeOfPlayField=2;
            }
            else if (MNU_ClickOnButton(themeSetting,6,e)){
                SFXSetting.buttonsAreActive = true;
                themeSetting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(themeSetting,7,e)){
                musicSetting.buttonsAreActive = true;
                themeSetting.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);

        }

        else if (musicSetting.buttonsAreActive){
            MNU_MenuPresent(m_renderer,musicSetting,colorMode,Ww,Hh);
            SDL_RenderCopy(m_renderer,ass2,NULL,&assRect2);

            if (MNU_ClickOnButton(musicSetting,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                if (backToWhereAfterSetting==1) {
                    pauseMenu.buttonsAreActive = true;
                    musicSetting.buttonsAreActive = false;
                }
                else {
                    mainMenu.buttonsAreActive = true;
                    musicSetting.buttonsAreActive = false;
                }
            }

            else if (MNU_ClickOnButton(musicSetting,1,e)){
                playBgm=1;
            }
            else if (MNU_ClickOnButton(musicSetting,2,e)){
                playBgm=2;
            }
            else if (MNU_ClickOnButton(musicSetting,3,e)){
                playBgm=3;
            }
            else if (MNU_ClickOnButton(musicSetting,4,e)){
                playBgmVolume=10;
                assRect2=volume[0];
            }
            else if (MNU_ClickOnButton(musicSetting,5,e)){
                playBgmVolume =30;
                assRect2=volume[1];
            }
            else if (MNU_ClickOnButton(musicSetting,6,e)){
                playBgmVolume =60;
                assRect2=volume[2];
            }
            else if (MNU_ClickOnButton(musicSetting,7,e)){
                playBgmVolume =96;
                assRect2=volume[3];
            }
            else if (MNU_ClickOnButton(musicSetting,8,e)){
                playBgmVolume =128;
                assRect2=volume[4];
            }
            else if (MNU_ClickOnButton(musicSetting,9,e)){
                playBgmVolume =0;
            }
            else if (MNU_ClickOnButton(musicSetting,10,e)){
                playBgmVolume =60;
            }
            else if (MNU_ClickOnButton(musicSetting,11,e)){
                themeSetting.buttonsAreActive = true;
                musicSetting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(musicSetting,12,e)){
                SFXSetting.buttonsAreActive = true;
                musicSetting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(musicSetting,13,e)){
                playBgm=4;
            }
            else if (MNU_ClickOnButton(musicSetting,14,e)){
                playBgm=5;
            }
            else if (MNU_ClickOnButton(musicSetting,15,e)){
                playBgm=6;
            }
            SDL_RenderPresent(m_renderer);

        }

        else if (SFXSetting.buttonsAreActive){
            MNU_MenuPresent(m_renderer,SFXSetting,colorMode,Ww,Hh);
            SDL_RenderCopy(m_renderer,ass,NULL,&assRect);

            if (MNU_ClickOnButton(SFXSetting,0,e)|| e->key.keysym.sym==SDLK_ESCAPE){
                if (backToWhereAfterSetting==1) {
                    pauseMenu.buttonsAreActive = true;
                    SFXSetting.buttonsAreActive = false;
                }
                else {
                    mainMenu.buttonsAreActive = true;
                    SFXSetting.buttonsAreActive = false;
                }
            }

            else if (MNU_ClickOnButton(SFXSetting,1,e)){
                assRect=volume[0];
                playSFX=20;
            }
            else if (MNU_ClickOnButton(SFXSetting,2,e)){
                assRect=volume[1];
                playSFX=40;
            }
            else if (MNU_ClickOnButton(SFXSetting,3,e)){
                assRect=volume[2];
                playSFX=60;
            }
            else if (MNU_ClickOnButton(SFXSetting,4,e)){
                assRect=volume[3];
                playSFX=96;
            }
            else if (MNU_ClickOnButton(SFXSetting,5,e)){
                assRect=volume[4];
                playSFX=128;
            }
            else if (MNU_ClickOnButton(SFXSetting,6,e)){
                playSFX=0;
            }
            else if (MNU_ClickOnButton(SFXSetting,7,e)){
                playSFX=60;
            }
            else if (MNU_ClickOnButton(SFXSetting,8,e)){
                themeSetting.buttonsAreActive = true;
                SFXSetting.buttonsAreActive = false;
            }
            else if (MNU_ClickOnButton(SFXSetting,9,e)){
                musicSetting.buttonsAreActive = true;
                SFXSetting.buttonsAreActive = false;
            }
            SDL_RenderPresent(m_renderer);

        }
        else if (victoryM.buttonsAreActive){
            MNU_ShowVictory(m_renderer,victoryM,mainMenu,newUser,e,victoryMode,userList,userIsInterred,playModeCode);
        }
        else if (gameOver.buttonsAreActive){
            MNU_ShowGameOver(m_renderer,gameOver,mainMenu,newUser,e,userList,userIsInterred,playModeCode);
        }

        else if (playField.buttonsAreActive){
            //******************************************************************************************
            time1 =chrono::high_resolution_clock::now();//3
            if(newgamestarted){
                newgamestarted=false;
                if(playModeCode==2){
                    playsound(4);
                }
                else{
                    playsound(3);
                }
            }

            float angle;
            MNU_MenuPresent(m_renderer,playField,colorModeOfPlayField,W,H);
            newUser.score= calculatescore(playModeCode);
            US_showPowerBarItemsCount(m_renderer,newUser,playModeCode);
            if(e->type == SDL_MOUSEMOTION){
                Point1.x=e->motion.x;    Point1.y=e->motion.y;
            }
            calculateangle(angle,e,anchorPoint);

            if(framecount++%100==0){
                assigntimeeffect();
            }




            if(e->type == SDL_KEYDOWN){
                switch(e->key.keysym.sym) {
                    case SDLK_SPACE:
                        swap(backupball,firedball);
                        firedball.x=W/2;    firedball.y=665;
                        backupball.x=848;        backupball.y=710;
                        break;
                    case SDLK_q://explosive
                        if(newUser.bombCount>0){
                            swap(firedball,backupball);
                            backupball.x=848;        backupball.y=710;
                            firedball.typecode=80;  updateflags(firedball);
                            firedball.x=W/2;    firedball.y=665;
                            newUser.bombCount--;
                        }
                        else{
                            playsound(16);
                        }
                        break;
                    case SDLK_e://multicolor
                        if(newUser.multicolorBallCount>0){
                            swap(firedball,backupball);
                            backupball.x=848;        backupball.y=710;
                            firedball.typecode=90;  updateflags(firedball);
                            firedball.x=W/2;    firedball.y=665;
                            newUser.multicolorBallCount--;
                        }
                        else{
                            playsound(16);
                        }
                        break;
                    case SDLK_w://laser
                    if(newUser.laserCount>0){
                        swap(firedball,backupball);
                        backupball.x=848;        backupball.y=710;
                        firedball.typecode=100;
                        firedball.x=W/2;    firedball.y=665;
                        newUser.laserCount--;
                    }
                    else{
                        playsound(16);
                    }
                        break;
                    case SDLK_a://slow
                        if(newUser.slowMotionCount>0){
                            isSlow=true;
                            playsound(14);
                            newUser.slowMotionCount--;
                            ballSpeed=Balldecentspeed/2;
                            now = clock();
                        }
                        else{
                            playsound(16);
                        }
                        break;
                    case SDLK_s://stop
                        if(newUser.stopCount>0){
                            isStopped=true;
                            playsound(15);
                            newUser.stopCount--;
                            ballSpeed=0;
                            now = clock();
                        }
                        else{
                            playsound(16);
                        }
                        break;
                }
            }
            else if(e->type == SDL_MOUSEBUTTONDOWN ){
                if(!firedball.isflying){
                    if(firedball.typecode==100){
                        clickdown.x=e->button.x;  clickdown.y=e->button.y;
                        findPoint2(anchorPoint,Point1,Point2);
                        laserlocation.push_back({float(Point2.x),float(Point2.y)});
                        lasermyballs(clickdown.x,clickdown.y);
                        powerballsused++;
                        clickdown.x=0;  clickdown.y=0;
                        copyballs(backupball,firedball);
                        firedball.x=W/2;        firedball.y=665;
                        firedball.istarget=false;
                        createbackupball(newUser);

                    }
                    else{
                        clickdown.x=e->button.x;  clickdown.y=e->button.y;
                        assignvelocity(firedball);
                        playsound(2);
                        clickdown.x=0;  clickdown.y=0;
                        firedball.isflying=true;
                    }
                }

            }
            SDL_RenderCopyEx(m_renderer,cannon,NULL,&cannonRect,angle,NULL,SDL_FLIP_NONE);
            if(firedball.typecode!=100){
                drawguideline(m_renderer,anchorPoint,Point1,balls);
            }
            drawmyballs(m_renderer,balls);
            drawanimationframes(m_renderer,animations,8,4);
            if(firedball.typecode==100){
                drawlaserguideline(m_renderer,anchorPoint,Point1);
            }
            SDL_RenderCopyEx(m_renderer,cannonup,NULL,&cannonRect,angle,NULL,SDL_FLIP_NONE);

            movemyballs(ballSpeed);
            updaterowclear();
            examinemyballs();
            if((!firedball.isflying)&&(firedball.istarget)){
                copyballs(backupball,firedball);
                firedball.x=W/2;        firedball.y=665;
                firedball.istarget=false;
                createbackupball(newUser);
            }

            if(int(rowofballs[9][0].y)==793){
                if(!endoftheline){
                    shiftmyballs();
                    if(playModeCode==2){
                        for (int i=41;i<=50;i++){
                            generaterandomrow(i);
                            rowcounter+=10;
                        }
                    }
                    else{
                        if(rowcounter<numberofrowsinnormalmode){
                            for (int i=41;i<=50;i++){
                                generaterandomrow(i);
                                rowcounter+=10;
                            }
                        }
                        else{
                            endoftheline=true;
                            endline.x=140; endline.y=-952;
                            for (int i=41;i<=50;i++){
                                generateemptyrow(i);
                            }
                        }
                    }
                }

            }

            updaterowclear();
            checkvictoryorloss2(playModeCode,victoryM,gameOver);

            //resetting decentspeed after duration
            if ((isSlow||isStopped) && double(clock()-now)/CLOCKS_PER_SEC > timeeffectduration ){
                ballSpeed=Balldecentspeed;
                isSlow=false;
                isStopped=false;
            }
            if (MNU_ClickOnButton(playField,0,e) || e->key.keysym.sym==SDLK_ESCAPE){
                pauseMenu.buttonsAreActive = true;
                playField.buttonsAreActive = false;
            }


            //calculating elapsed time based on activetimeeffects
            if(isStopped){
                elapsedtime+=0;
            }
            else if(isSlow){
                time2 =chrono::high_resolution_clock::now();//3
                temptime=(chrono::duration_cast<chrono::milliseconds>(time2.time_since_epoch()).count()-chrono::duration_cast<chrono::milliseconds>(time1.time_since_epoch()).count());//1
                elapsedtime+=temptime;
            }
            else{
                time2 =chrono::high_resolution_clock::now();//3
                temptime=(chrono::duration_cast<chrono::milliseconds>(time2.time_since_epoch()).count()-chrono::duration_cast<chrono::milliseconds>(time1.time_since_epoch()).count());//1
                elapsedtime+=temptime;
            }

            //show things
            SDL_RenderPresent(m_renderer);
        }
        e->button.button=0;
        e->type=0;

        frame2=clock();
        if(double(frame2-frame1)/CLOCKS_PER_SEC<timeforeachframe){
            SDL_Delay(timeforeachframe-(double(frame2-frame1))/CLOCKS_PER_SEC);
        }
    }
//********************************************************************************************************
//******************************************************************************************************
    SDL_StopTextInput();

    ofstream updateSetting;
    updateSetting.open("files/inputSetting.txt",ios::out);

    if(updateSetting.good()){
        updateSetting<<colorMode<<"\n"<<colorModeOfPlayField<<"\n"<<playBgmOut<<"\n"<<playSFXOut<<"\n"<<playBgmVolumeOut;
        updateSetting.close();
    }


    ofstream updateUsers;
    updateUsers.open("files/inputUsers.txt",ios::out);
    if(updateUsers.good()){
        for (int i = 0; i < userList.size(); i++) {

            updateUsers<<userList[i].username<<","<<userList[i].score<<","
                       <<userList[i].bombCount<<","<<userList[i].laserCount<<","<<userList[i].multicolorBallCount<<","
                       <<userList[i].slowMotionCount<<","<<userList[i].stopCount<<","
                       <<userList[i].scoreInMode[0]<<","<<userList[i].scoreInMode[1]<<","<<userList[i].scoreInMode[2]<<"\n";
        }
        updateUsers.close();
    }


////Finalize and free resources

    MNU_DestroyMenu(mainMenu);
    MNU_DestroyMenu(playMode);
    MNU_DestroyMenu(getUsername);
    MNU_DestroyMenu(setting);
    MNU_DestroyMenu(scoreboard);
    MNU_DestroyMenu(playField);
    MNU_DestroyMenu(musicSetting);
    MNU_DestroyMenu(themeSetting);
    MNU_DestroyMenu(pauseMenu);
    MNU_DestroyMenu(victoryM);
    MNU_DestroyMenu(gameOver);
    MNU_DestroyMenu(help);
    DestroyAnimations(animations);
    for (int i = 0; i < balls.size(); ++i) {
        SDL_DestroyTexture(balls[i]);
    }
    SDL_DestroyTexture(cannon);
    for (int i = 0; i < SFXs.size(); ++i) {
        Mix_FreeChunk(SFXs[i]);
    }
    for (int i = 0; i < bgm.size(); ++i) {
        Mix_FreeMusic(bgm[i]);
    }

    SDL_DestroyTexture(bkImg);
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
    return 0;
}
//************************************************************************************************************************
//***********************************************************************************************************************
//******************************mahdi functions implementation************************************************************
bool MNU_ClickOnButton (const MNU_Menu &menu , int indexOfButton , SDL_Event * e){

    if (menu.buttonsAreActive){
        if (e ->button.button == SDL_BUTTON_LEFT &&
            menu.buttonPlace[indexOfButton].x + menu.buttonPlace[indexOfButton].w > e->button.x && e->button.x > menu.buttonPlace[indexOfButton].x &&
            menu.buttonPlace[indexOfButton].y + menu.buttonPlace[indexOfButton].h > e->button.y && e->button.y > menu.buttonPlace[indexOfButton].y)
            return true;
        else
            return false;
    } else{
        return false;
    }
}

void MNU_MenuPresent (SDL_Renderer * renderer ,const MNU_Menu &menu , int mode , int Ww , int Hh){
    SDL_Rect rect1; rect1.x =0 ;rect1.y=0; rect1.w =Ww ;rect1.h=Hh;
    SDL_RenderCopy(renderer,menu.backgroundImage[mode],NULL,&rect1);
}
void MNU_ShowVictory (SDL_Renderer * renderer ,  MNU_Menu &menu , MNU_Menu &mainMenu , US_user& user, SDL_Event * e , int & mode ,vector<US_user>& list, bool &b , int playMode) {
    MNU_MenuPresent(renderer, menu, mode, 1280, 750);
    textRGBA2(renderer, 358, 433, to_string(user.score).c_str(), 50, 100, 20, 20, 255);
    initializegame();
    createfiredball(user);
    createbackupball(user);

    if (MNU_ClickOnButton(menu, 0, e) && mode!= 0) {
        mode=0;
        mainMenu.buttonsAreActive = true;
        menu.buttonsAreActive = false;
        newgamestarted=true;
        destroyedballs=0;   powerballsused=0;
    } else if (mode == 0)
    {
        if (MNU_ClickOnButton(menu, 1, e)) {
            mode = 1;
            user.bombCount++;
        }
        else if (MNU_ClickOnButton(menu, 2, e)) {
            mode = 2;
            user.laserCount++;
        }
        else if (MNU_ClickOnButton(menu, 3, e)) {
            mode = 3;
            user.multicolorBallCount++;
        }
    }
    if(!b){
        US_finalUpdateAfterGame (list,  user,playMode);
        b=true;
    }



    SDL_RenderPresent(renderer);
}

void MNU_ShowGameOver (SDL_Renderer * renderer ,  MNU_Menu &menu , MNU_Menu &mainMenu , US_user user, SDL_Event * e,vector<US_user>& list, bool b, int playMode){
    MNU_MenuPresent(renderer,menu,0,1280,750);
    textRGBA2(renderer,250,295, to_string(user.score).c_str(),50,20,20,100, 255);
    initializegame();
    createfiredball(user);
    createbackupball(user);

    if(!b){
        US_finalUpdateAfterGame (list,  user, playMode);
        b=true;
    }


    if (MNU_ClickOnButton(menu,0,e)){

        mainMenu.buttonsAreActive = true;
        menu.buttonsAreActive=false;
        newgamestarted=true;
        destroyedballs=0;   powerballsused=0;
    }
    SDL_RenderPresent(renderer);
}

void MNU_DestroyMenu (MNU_Menu &menu){
    for (int i = 0; i < menu.backgroundImage.size(); ++i) {
        SDL_DestroyTexture(menu.backgroundImage[i]);
    }
}

bool US_sortHelper(US_user a, US_user b){
    if(a.score!=b.score)
        return a.score>b.score;
    else
        return a.username>b.username;
}

bool US_sortHelperM0(US_user a, US_user b){
    if(a.scoreInMode[0]!=b.scoreInMode[0])
        return a.scoreInMode[0]>b.scoreInMode[0];
    else
        return a.username>b.username;
}
bool US_sortHelperM1(US_user a, US_user b){
    if(a.scoreInMode[1]!=b.scoreInMode[1])
        return a.scoreInMode[1]>b.scoreInMode[1];
    else
        return a.username>b.username;
}
bool US_sortHelperM2(US_user a, US_user b){
    if(a.scoreInMode[2]!=b.scoreInMode[2])
        return a.scoreInMode[2]>b.scoreInMode[2];
    else
        return a.username>b.username;
}

void US_showScores(SDL_Renderer * renderer , vector<US_user>& list ,int x, int y ,int colorMode , int showScoreMode){
    int rr,gg,bb;
    if (colorMode==0){
        rr=gg=bb=230;
    } else if (colorMode==1){
        rr=gg=bb=20;
    }

    if(showScoreMode==100) {
        sort(list.begin(),list.end(), US_sortHelper);

        for (int i = 0; i < list.size() && i < 10; i++) {
            textRGBA2(renderer, x, y, list[i].username.c_str(), 30, rr, gg, bb, 255);
            textRGBA2(renderer, x + 400, y, to_string(list[i].score).c_str(), 30, rr, gg, bb, 255);
            lineRGBA(renderer, x, y + 42, x + 660, y + 42, rr, gg, bb, 80);
            y += 50;
        }
    }
    else if(showScoreMode==0){
        sort(list.begin(),list.end(), US_sortHelperM0);
        for (int i = 0; i < list.size() && i < 10; i++) {
            textRGBA2(renderer, x, y, list[i].username.c_str(), 30, rr, gg, bb, 255);
            textRGBA2(renderer, x + 400, y, to_string(list[i].scoreInMode[0]).c_str(), 30, rr, gg, bb, 255);
            lineRGBA(renderer, x, y + 42, x + 660, y + 42, rr, gg, bb, 80);
            y += 50;
        }

    }
    else if(showScoreMode==1){
        sort(list.begin(),list.end(), US_sortHelperM1);
        for (int i = 0; i < list.size() && i < 10; i++) {
            textRGBA2(renderer, x, y, list[i].username.c_str(), 30, rr, gg, bb, 255);
            textRGBA2(renderer, x + 400, y, to_string(list[i].scoreInMode[1]).c_str(), 30, rr, gg, bb, 255);
            lineRGBA(renderer, x, y + 42, x + 660, y + 42, rr, gg, bb, 80);
            y += 50;
        }
    }
    else if(showScoreMode==2){
        sort(list.begin(),list.end(), US_sortHelperM2);
        for (int i = 0; i < list.size() && i < 10; i++) {
            textRGBA2(renderer, x, y, list[i].username.c_str(), 30, rr, gg, bb, 255);
            textRGBA2(renderer, x + 400, y, to_string(list[i].scoreInMode[2]).c_str(), 30, rr, gg, bb, 255);
            lineRGBA(renderer, x, y + 42, x + 660, y + 42, rr, gg, bb, 80);
            y += 50;
        }
    }
}
void US_showPowerBarItemsCount(SDL_Renderer * renderer , const US_user& user , int playModeCode){
    textRGBA2(renderer, 60 , 237 , to_string(user.bombCount).c_str(),27,100,35,200,255);
    textRGBA2(renderer, 60 , 414 , to_string(user.laserCount).c_str(),27,100,35,200,255);
    textRGBA2(renderer, 60 , 591 , to_string(user.multicolorBallCount).c_str(),27,100,35,200,255);
    textRGBA2(renderer, 1232 , 211 , to_string(user.slowMotionCount).c_str(),35,20,20,20,255);
    textRGBA2(renderer, 1232 , 320 , to_string(user.stopCount).c_str(),35,20,20,20,255);

    textRGBA2(renderer, 305 , 640 , to_string(user.score).c_str(),50,100,70,70,255);

    //show time here
    if(playModeCode==1){
        textRGBA2(renderer, 918 , 645 , to_string(timeintimedmode-2*double(elapsedtime/1000)).substr(0,5).c_str(),35,50,20,20,255);
        if((timeintimedmode-2*double(elapsedtime/1000)<5)&&(!reversetimeSFX1)){
            playsound(13);
            reversetimeSFX1=true;
        }
        else if((timeintimedmode-2*double(elapsedtime/1000)<10)&&(!reversetimeSFX2)){
            playsound(12);
            reversetimeSFX2=true;
        }
        else if((timeintimedmode-2*double(elapsedtime/1000)<30)&&(!reversetimeSFX3)){
            playsound(11);
            reversetimeSFX3=true;
        }
    }
    else{
        textRGBA2(renderer, 918 , 645 , to_string(2*double(elapsedtime/1000)).substr(0,5).c_str(),35,50,20,20,255);
    }
}

int US_searchForUser( vector<US_user>& list,const string& name){
    for (int i = 0; i < list.size(); i++) {
        if(list[i].username==name){
            return i;
        }
    }
    return -1;
}

void US_extractString(const string &s, vector<string> &expr)
{
    string opRef=",";
    int indexStart=0;
    int indexEnd=0;
    while(indexEnd!=-1)
    {
        indexEnd= s.find_first_of(opRef,indexStart);
        if(indexEnd!=-1)
        {
            expr.push_back(s.substr(indexStart,indexEnd-indexStart));
        }
        else
        {
            expr.push_back(s.substr(indexStart));
        }
        indexStart = indexEnd + 1;
    }
}

void US_finalUpdateAfterGame (vector<US_user>& userList, US_user& newUser, int playMode){
    if(US_searchForUser(userList,newUser.username)==-1){
        newUser.scoreInMode[playMode] = newUser.score;
        userList.push_back(newUser);
    } else {
        if(newUser.score>userList[US_searchForUser(userList,newUser.username)].score) {
            userList[US_searchForUser(userList, newUser.username)].score = newUser.score;
            userList[US_searchForUser(userList, newUser.username)].scoreInMode[playMode] = newUser.score;
            userList[US_searchForUser(userList,newUser.username)].bombCount=newUser.bombCount;
            userList[US_searchForUser(userList,newUser.username)].multicolorBallCount=newUser.multicolorBallCount;
            userList[US_searchForUser(userList,newUser.username)].laserCount=newUser.laserCount;
            userList[US_searchForUser(userList,newUser.username)].slowMotionCount=newUser.slowMotionCount;
            userList[US_searchForUser(userList,newUser.username)].stopCount=newUser.stopCount;
        }
        else if(newUser.score>userList[US_searchForUser(userList,newUser.username)].scoreInMode[playMode]){
            userList[US_searchForUser(userList, newUser.username)].scoreInMode[playMode] = newUser.score;
            userList[US_searchForUser(userList,newUser.username)].bombCount=newUser.bombCount;
            userList[US_searchForUser(userList,newUser.username)].multicolorBallCount=newUser.multicolorBallCount;
            userList[US_searchForUser(userList,newUser.username)].laserCount=newUser.laserCount;
            userList[US_searchForUser(userList,newUser.username)].slowMotionCount=newUser.slowMotionCount;
            userList[US_searchForUser(userList,newUser.username)].stopCount=newUser.stopCount;
        }
        else{
            userList[US_searchForUser(userList,newUser.username)].bombCount=newUser.bombCount;
            userList[US_searchForUser(userList,newUser.username)].multicolorBallCount=newUser.multicolorBallCount;
            userList[US_searchForUser(userList,newUser.username)].laserCount=newUser.laserCount;
            userList[US_searchForUser(userList,newUser.username)].slowMotionCount=newUser.slowMotionCount;
            userList[US_searchForUser(userList,newUser.username)].stopCount=newUser.stopCount;
        }
    }
}


void textRGBA2(SDL_Renderer*m_renderer, int x, int y, const char* text,int f_size,Uint8 rr, Uint8 gg, Uint8 bb, Uint8 aa)
{
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("Dosis-SemiBold.ttf", f_size);
    int textWidth, textHeight;
    TTF_SizeText(font, text, &textWidth, &textHeight);

    SDL_Rect rectText{x, y, 0, 0};
    SDL_Color color{rr,gg,bb,aa};

    SDL_Surface *textSur = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *textTex = SDL_CreateTextureFromSurface(m_renderer, textSur);

    SDL_QueryTexture(textTex, nullptr, nullptr, &rectText.w, &rectText.h);
    SDL_RenderCopy(m_renderer, textTex, nullptr, &rectText);

    SDL_FreeSurface(textSur);
    SDL_DestroyTexture(textTex);
    TTF_CloseFont(font);
}

void DestroyAnimations (vector<vector<SDL_Texture*>> &all){
    for (int i = 0; i < all.size(); ++i) {
        for (int j = 0; j < all[i].size(); ++j) {
            SDL_DestroyTexture(all[i][j]);
        }
    }
}

void loadBalls(SDL_Renderer * m_renderer ,vector<SDL_Texture *> &balls){
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/1.png"));// 0
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/2.png"));// 1
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/3.png"));// 2
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/4.png"));// 3
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/5.png"));// 4
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/6.png"));// 5
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-1.png"));// 6
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-2.png"));// 7
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-3.png"));// 8
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-4.png"));// 9
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-5.png"));// 10
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/-6.png"));// 11
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/stone.png"));// 12
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/12.png"));// 13
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/13.png"));// 14
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/14.png"));// 15
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/15.png"));// 16
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/16.png"));// 17
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/23.png"));// 18
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/24.png"));// 19
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/25.png"));
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/26.png"));
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/34.png"));// 22
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/35.png"));
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/36.png"));
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/45.png"));// 25
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/46.png"));// 26
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/56.png"));// 27
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/multiColor.png"));// 28
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/bomb2.png"));// 29
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/100.png"));// 30
    balls.push_back(IMG_LoadTexture(m_renderer,"images/endline.png"));//31
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/smallCS.png"));//32
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/smallCB.png"));//33
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/mudslow.png"));//34
    balls.push_back(IMG_LoadTexture(m_renderer,"images/balls/stopice.png"));//35
}


//******************************mahdi functions implementation************************************************************
//******************************poorya function implementation*************************************************************
void copyballs(Ball &ball1,Ball &ball2){
    //copies ball1 into ball2
    ball2.typecode=ball1.typecode;
    ball2.istarget=ball1.istarget;          ball2.isempty=ball1.isempty;    ball2.isstone=ball1.isstone;
    ball2.isflying=ball1.isflying;          ball2.isexplosive=ball1.isexplosive;    ball2.islocked=ball1.islocked;
    ball2.ismulticolored=ball1.ismulticolored;  ball2.is2colored=ball1.is2colored;  ball2.isconnected=ball1.isconnected;
    ball2.vx=ball1.vx;  ball2.vy=ball1.vy;
}
void updateflags(Ball &ball){
    if (ball.typecode==0){
        ball.isempty=true;
        ball.istarget=false;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=false;
        ball.is2colored=false;
        ball.islocked=false;
        ball.isstone=false;
        ball.isconnected=false;
        ball.stopeffect=0;
        ball.sloweffect=0;
    }
    else if(ball.typecode==7){
        ball.isempty=false;
        ball.istarget=true;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=false;
        ball.is2colored=false;
        ball.islocked=false;
        ball.isstone=true;
        ball.isconnected=false;
    }
    else if(ball.typecode<0){
        ball.isempty=false;
        ball.istarget=true;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=false;
        ball.is2colored=false;
        ball.islocked=true;
        ball.isstone=false;
        ball.isconnected=false;
    }
    else if((ball.typecode>10)&&(ball.typecode<80)){
        ball.isempty=false;
        ball.istarget=true;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=false;
        ball.is2colored=true;
        ball.islocked=false;
        ball.isstone=false;
        ball.isconnected=false;
    }
    else if(ball.typecode==90){
        ball.isempty=false;
        ball.istarget=false;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=true;
        ball.is2colored=false;
        ball.islocked=false;
        ball.isstone=false;
        ball.isconnected=false;
    }
    else if(ball.typecode==80){
        ball.isempty=false;
        ball.istarget=false;
        ball.isflying=false;
        ball.isexplosive=true;
        ball.ismulticolored=false;
        ball.is2colored=false;
        ball.islocked=false;
        ball.isstone=false;
        ball.isconnected=false;
    }
    else{
        ball.isempty=false;
        ball.istarget=true;
        ball.isflying=false;
        ball.isexplosive=false;
        ball.ismulticolored=false;
        ball.is2colored=false;
        ball.islocked=false;
        ball.isstone=false;
        ball.isconnected=false;
    }
}
void assigntimeeffect(){

    for(int i=0;i<50;i++){
        for(int j=0;j<Maxballseachrow;j++){
            if((!rowofballs[i][j].isempty)&&(rowofballs[i][j].y>0)&&(rowofballs[i][j].sloweffect==0)&&(rowofballs[i][j].stopeffect==0)){
                if(rand()%100<3){
                        if(rand()%2){
                            rowofballs[i][j].sloweffect=numberofframesinwhichtimeeffectsareactive;
                        }
                        else{
                            rowofballs[i][j].stopeffect=numberofframesinwhichtimeeffectsareactive;
                        }
                }

            }
        }
    }
}
void generateemptyrow(int row){
    if (row%2==1) {
        for (int i = 0; i < Maxballseachrow; i++) {
            rowofballs[row - 1][i].x = 190 + (2 * i - 1) * Ballradius;
            rowofballs[row - 1][i].y = 750-((row-1)*43);
            rowofballs[row - 1][i].typecode=0;
            updateflags(rowofballs[row-1][i]);

        }
    }
    else if (row%2==0){
        for (int i = 0; i < Maxballseachrow; i++) {
            rowofballs[row - 1][i].x = 190 + Ballradius + (2 * i - 1) * Ballradius;
            rowofballs[row - 1][i].y = 750-((row-1)*43);
            rowofballs[row - 1][i].typecode=0;
            updateflags(rowofballs[row-1][i]);
        }
    }
}

void generaterandomrow (int row){
//    srand(time(NULL));
    int random;
    if (row%2==1) {
        for (int i = 0; i < Maxballseachrow; i++) {
            rowofballs[row - 1][i].x = 190 + (2 * i - 1) * Ballradius;
            rowofballs[row - 1][i].y = 750-((row-1)*43);
            rowofballs[row - 1][i].typecode=randomtype(row - 1,i);
            updateflags(rowofballs[row-1][i]);
        }
    }
    else if (row%2==0){
        for (int i = 0; i < Maxballseachrow; i++) {
            rowofballs[row - 1][i].x = 190 + Ballradius + (2 * i - 1) * Ballradius;
            rowofballs[row - 1][i].y = 750-((row-1)*43);
            rowofballs[row - 1][i].typecode=randomtype(row - 1,i);
            updateflags(rowofballs[row-1][i]);
        }
    }
}

bool coinflip(int probablity){
    int random;
    random=rand()%100+1;
    if(random>probablity)
        return false;
    else
        return true;
}

int randomtype(int i,int j){

    if((j==0)||(j==1)||(j==18)||(j==19)){
        return 0;
    }
    if(i%10==9){
        if(j==2){
            randomrowassist=rand()%6+1;
        }
        if(coinflip(P11)){
            return compcode(i,j,randomrowassist);
        }
        else{
            return 0;
        }

    }
    int result;
    int random;
    bool success=false,b=false;
    for(int k=1;k<7;k++){
        if(isaround(i,j,k))
            b=true;
    }
    if(!b){
        return rand()%6+1;
    }
    else{
        int n=rand()%6+1;
        while(1){
            if(isaround(i,j,n)){
                if(coinflip(P12)){
                    return 0;
                }
                else{
                    if(coinflip(P13)){
                        return 7;
                    }
                    else{
                        return compcode(i,j,n);
                    }
                }

//                if(coinflip(P9)){
//                    return compcode(i,j,n);
//                }
//                else{
//                    if(coinflip(P10)){
//                        return 0;
//                    }
//                    else{
//                        return 7;
//                    }
//                }
            }
            if(n==6){
                n=1;
            }
            else{
                n++;
            }
        }

    }

//    for (int k=0;k<8;k++){
//        result=assignedcode(i,j,(random+k)%8);
//    }
//    return result;
}

int assignedcode(int i,int j,int typecode){
    int random2,result;
    if(isaround(i,j,typecode)){

        switch (typecode){
            case 0:
                if(coinflip(P1))
                    result=0;
                else{
                    if(coinflip(P2)){
                        result=7;
                    }
                    else{
                        random2=(rand()%6)+1;
                        result=compcode(i,j,random2);
                    }
                }
                break;
            case 7:
                if(coinflip(P3))
                    result=7;
                else {
                    if(coinflip(P4)){
                        result=0;
                    }
                    else{
                        random2=(rand()%6)+1;
                        result=compcode(i,j,random2);
                    }
                }
                break;
            default:
                if(coinflip(P5))
                    result=0;
                else {
                    if(coinflip(P6)){
                        result=7;
                    }
                    else{
                        result=compcode(i,j,typecode);
                    }
                }
                break;
        }
    }
    return result;
}

int compcode(int i,int j,int colorcode){
    int result;
    int secondcolor=(rand()%6)+1;
    if(coinflip(P7)){
        result=(-colorcode);
    }
    else{
        if(coinflip(P8)){
            result=colorcode;
        }
        else{
            while(secondcolor==colorcode){
                secondcolor=(rand()%6)+1;
            }
            result=(10*colorcode)+secondcolor;
        }
    }
    return result;
}

bool isaround(int i,int j,int colorcode){
    if (i%2==0){
        if(j==0){
            if(iscompatible(rowofballs[i][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            return false;
        }
        else if(j==Maxballseachrow-1){
            if(iscompatible(rowofballs[i][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j-1].typecode,colorcode))
                return true;
            return false;
        }
        else{
            if(iscompatible(rowofballs[i][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j-1].typecode,colorcode))
                return true;
            return false;
        }
    }
    else if(i%2==1){
        if(j==0){
            if(iscompatible(rowofballs[i][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j+1].typecode,colorcode))
                return true;
            return false;
        }
        else if(j==Maxballseachrow-1){
            if(iscompatible(rowofballs[i][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            return false;
        }
        else{
            if(iscompatible(rowofballs[i][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i][j-1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i-1][j+1].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j].typecode,colorcode))
                return true;
            if(iscompatible(rowofballs[i+1][j+1].typecode,colorcode))
                return true;
            return false;
        }
    }
}

bool iscompatible(int typecode,int colorcode){
    if(typecode==colorcode){
        return true;
    }
    if((colorcode==90)&&(typecode!=0)&&(typecode!=7)){
        return true;
    }
    if((typecode==90)&&(colorcode!=0)&&(colorcode!=7)){
        return true;
    }
    if(colorcode>10){
        int a1=colorcode%10;
        int a2=colorcode/10;
        if((iscompatible(typecode,a1))||(iscompatible(typecode,a1))){
            return true;
        }
    }
    else if(colorcode<0){
        if(iscompatible(typecode,(-colorcode))){
            return true;
        }
    }
    else{
        int a=0,b=0;

        if (typecode>10){
            b=typecode%10;
            a=typecode/10;
        }
        else if(typecode<0){
            a=-typecode;
        }
        else{
            a=typecode;
        }

        if (colorcode==0){
            if(a==0)
                return true;
        }
        else{
            if((a==colorcode)||(b==colorcode))
                return true;
        }
    }
    return false;
}
void calculateangle(float &angle,SDL_Event *e,SDL_Point anchorPoint){
    if(e->type == SDL_MOUSEMOTION){
        float tanAngle;
        tanAngle = float((e->motion.x)-anchorPoint.x) / float(-(e->motion.y)+anchorPoint.y);
        angle = atan( tanAngle ) * 180 / M_PI;
    }
}
void findPoint2(SDL_Point anchorPoint,SDL_Point Point1,SDL_Point &Point2){
    float tanteta=-(float(Point1.x-anchorPoint.x)/float(Point1.y-anchorPoint.y));
    if((endoftheline)&&(endline.y>0)){
        float l1=(float(verticalline1x-W/2)/float(anchorPoint.y-endline.y)); float l2=(float(verticalline2x-W/2)/float(anchorPoint.y-endline.y));
        if((tanteta<l2)&&(tanteta>l1)){
            Point2.x=tanteta*(anchorPoint.y-endline.y)+W/2;       Point2.y=endline.y;
        }
        else if(tanteta<l1){
            Point2.x=verticalline1x; Point2.y=(anchorPoint.y)+(W/2-verticalline1x)*((1/tanteta));
        }
        else if(tanteta>l2){
            Point2.x=verticalline2x; Point2.y=(anchorPoint.y)+(W/2-verticalline2x)*((1/tanteta));

        }
    }
    else{
        float l1=(float(verticalline1x-W/2)/float(anchorPoint.y)); float l2=(float(verticalline2x-W/2)/float(anchorPoint.y));
        if((tanteta<l2)&&(tanteta>l1)){
            Point2.x=tanteta*(anchorPoint.y)+W/2;       Point2.y=0;
        }
        else if(tanteta<l1){
            Point2.x=verticalline1x; Point2.y=anchorPoint.y+(W/2-verticalline1x)*((1/tanteta));
        }
        else if(tanteta>l2){
            Point2.x=verticalline2x; Point2.y=anchorPoint.y+(W/2-verticalline2x)*((1/tanteta));

        }
    }
}
void drawlaserguideline(SDL_Renderer * m_renderer,SDL_Point anchorPoint,SDL_Point Point1){
    SDL_Point Point2;
    float tanteta=-(float(Point1.x-anchorPoint.x)/float(Point1.y-anchorPoint.y));
    if((endoftheline)&&(endline.y>0)){
        float l1=(float(verticalline1x-W/2)/float(anchorPoint.y-endline.y)); float l2=(float(verticalline2x-W/2)/float(anchorPoint.y-endline.y));
        if((tanteta<l2)&&(tanteta>l1)){
            Point2.x=tanteta*(anchorPoint.y-endline.y)+W/2;       Point2.y=endline.y;
        }
        else if(tanteta<l1){
            Point2.x=verticalline1x; Point2.y=(anchorPoint.y)+(W/2-verticalline1x)*((1/tanteta));
        }
        else if(tanteta>l2){
            Point2.x=verticalline2x; Point2.y=(anchorPoint.y)+(W/2-verticalline2x)*((1/tanteta));

        }
    }
    else{
        float l1=(float(verticalline1x-W/2)/float(anchorPoint.y)); float l2=(float(verticalline2x-W/2)/float(anchorPoint.y));
        if((tanteta<l2)&&(tanteta>l1)){
            Point2.x=tanteta*(anchorPoint.y)+W/2;       Point2.y=0;
        }
        else if(tanteta<l1){
            Point2.x=verticalline1x; Point2.y=anchorPoint.y+(W/2-verticalline1x)*((1/tanteta));
        }
        else if(tanteta>l2){
            Point2.x=verticalline2x; Point2.y=anchorPoint.y+(W/2-verticalline2x)*((1/tanteta));

        }
    }
    thickLineRGBA(m_renderer,anchorPoint.x,anchorPoint.y,Point2.x,Point2.y,2,255,0,0,255);
    filledCircleRGBA(m_renderer,Point2.x,Point2.y,5,255,0,0,255);
}
void drawguideline(SDL_Renderer * m_renderer,SDL_Point anchorPoint,SDL_Point Point1,vector<SDL_Texture *> balls){
    location Point2;   Point2.x=anchorPoint.x; Point2.y=anchorPoint.y;
    float tanteta=-(float(Point1.x-anchorPoint.x)/float(Point1.y-anchorPoint.y));
    int limit;
    if(endline.y>0){
        limit=int(anchorPoint.y-endline.y)/10;
    }
    else{
        limit=int(anchorPoint.y)/10;
    }
    float dy=10;  float dx=tanteta*10;
    for(int k=0;k<limit;k++){
        //replace with texture
        if(k%2){
            SDL_Rect rect55; rect55.x =int(Point2.x)-2 ;rect55.y=int(Point2.y)-2 ; rect55.w =4 ;rect55.h=4;
            SDL_RenderCopy(m_renderer,balls[33],NULL,&rect55);
        }
        else{
            SDL_Rect rect56; rect56.x =int(Point2.x)-3 ;rect56.y=int(Point2.y)-3 ; rect56.w =6 ;rect56.h=6;
            SDL_RenderCopy(m_renderer,balls[33],NULL,&rect56);
        }
        Point2.y-=dy;
        Point2.x+=dx;
        if((Point2.x>verticalline2x)||((Point2.x<verticalline1x))){
            dx=-dx; Point2.x+=dx;
        }
    }

}
void initializegame(){
    reversetimeSFX3=false;   reversetimeSFX2=false;   reversetimeSFX1=false;
    ballSpeed=Balldecentspeed;
    isSlow=false;   isStopped=false;
    endline={0,0};
    tobedestroyedballs.clear();
    temptobedestroyedballs.clear();
    explosionlocation.clear();
    laserlocation.clear();
    balldestructionlocation.clear();
    unboundballs.clear();
    endoftheline=false;
    rowcounter=0;
    elapsedtime=0;
    newgamestarted=true;
    destroyedballs=0;   powerballsused=0;
    for (int i=1;i<=50;i++){
        generateemptyrow(i);
        rowclear[i-1]=true;
    }
    for (int i=21;i<=50;i++){
        generaterandomrow(i);
        rowclear[i-1]=false;
    }
    rowcounter+=30;

}
void shiftmyballs(){
    for(int i=0;i<40;i++) {
        for (int j = 0; j < Maxballseachrow; j++) {
            rowofballs[i][j].x=rowofballs[i+10][j].x;
            rowofballs[i][j].y=rowofballs[i+10][j].y;
            rowofballs[i][j].typecode=rowofballs[i+10][j].typecode;
            rowofballs[i][j].isempty=rowofballs[i+10][j].isempty;
            rowofballs[i][j].is2colored=rowofballs[i+10][j].is2colored;
            rowofballs[i][j].isstone=rowofballs[i+10][j].isstone;
            rowofballs[i][j].islocked=rowofballs[i+10][j].islocked;
            rowofballs[i][j].istarget=rowofballs[i+10][j].istarget;
            rowofballs[i][j].isexplosive=rowofballs[i+10][j].isexplosive;
            rowofballs[i][j].ismulticolored=rowofballs[i+10][j].ismulticolored;
            rowofballs[i][j].isflying=rowofballs[i+10][j].isflying;
            rowofballs[i][j].isconnected=rowofballs[i+10][j].isconnected;
            rowofballs[i][j].sloweffect=rowofballs[i+10][j].sloweffect;
            rowofballs[i][j].stopeffect=rowofballs[i+10][j].stopeffect;
        }
    }
}
void drawthisball(SDL_Renderer * m_renderer,int x,int y,int typecode,vector<SDL_Texture *> balls,int &sloweffect, int &stopeffect){
    if(typecode<10){
        switch (typecode) {
            case 1:
                SDL_Rect rect03; rect03.x =x-Ballradius ;rect03.y=y-Ballradius ; rect03.w =2*Ballradius ;rect03.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[0],NULL,&rect03);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -1:
                SDL_Rect rect04; rect04.x =x-Ballradius ;rect04.y=y-Ballradius ; rect04.w =2*Ballradius ;rect04.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[6],NULL,&rect04);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;

            case 2:
                SDL_Rect rect05; rect05.x =x-Ballradius ;rect05.y=y-Ballradius ; rect05.w =2*Ballradius ;rect05.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[1],NULL,&rect05);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -2:
                SDL_Rect rect06; rect06.x =x-Ballradius ;rect06.y=y-Ballradius ; rect06.w =2*Ballradius ;rect06.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[7],NULL,&rect06);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 3:
                SDL_Rect rect07; rect07.x =x-Ballradius ;rect07.y=y-Ballradius ; rect07.w =2*Ballradius ;rect07.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[2],NULL,&rect07);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -3:
                SDL_Rect rect08; rect08.x =x-Ballradius ;rect08.y=y-Ballradius ; rect08.w =2*Ballradius ;rect08.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[8],NULL,&rect08);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 4:
                SDL_Rect rect09; rect09.x =x-Ballradius ;rect09.y=y-Ballradius ; rect09.w =2*Ballradius ;rect09.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[3],NULL,&rect09);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -4:
                SDL_Rect rect10; rect10.x =x-Ballradius ;rect10.y=y-Ballradius ; rect10.w =2*Ballradius ;rect10.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[9],NULL,&rect10);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 5:
                SDL_Rect rect11; rect11.x =x-Ballradius ;rect11.y=y-Ballradius ; rect11.w =2*Ballradius ;rect11.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[4],NULL,&rect11);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -5:
                SDL_Rect rect12; rect12.x =x-Ballradius ;rect12.y=y-Ballradius ; rect12.w =2*Ballradius ;rect12.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[10],NULL,&rect12);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 6:
                SDL_Rect rect13; rect13.x =x-Ballradius ;rect13.y=y-Ballradius ; rect13.w =2*Ballradius ;rect13.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[5],NULL,&rect13);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case -6:
                SDL_Rect rect14; rect14.x =x-Ballradius ;rect14.y=y-Ballradius ; rect14.w =2*Ballradius ;rect14.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[11],NULL,&rect14);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 7:
                SDL_Rect rect02; rect02.x =x-Ballradius ;rect02.y=y-Ballradius ; rect02.w =2*Ballradius ;rect02.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[12],NULL,&rect02);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
        }
    }
    else if(typecode<80){
        if ((typecode==12)||(typecode==21)){
            SDL_Rect rect15; rect15.x =x-Ballradius ;rect15.y=y-Ballradius ; rect15.w =2*Ballradius ;rect15.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[13],NULL,&rect15);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==13)||(typecode==31)){
            SDL_Rect rect16; rect16.x =x-Ballradius ;rect16.y=y-Ballradius ; rect16.w =2*Ballradius ;rect16.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[14],NULL,&rect16);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==14)||(typecode==41)){
            SDL_Rect rect17; rect17.x =x-Ballradius ;rect17.y=y-Ballradius ; rect17.w =2*Ballradius ;rect17.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[15],NULL,&rect17);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==15)||(typecode==51)){
            SDL_Rect rect18; rect18.x =x-Ballradius ;rect18.y=y-Ballradius ; rect18.w =2*Ballradius ;rect18.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[16],NULL,&rect18);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==16)||(typecode==61)){
            SDL_Rect rect19; rect19.x =x-Ballradius ;rect19.y=y-Ballradius ; rect19.w =2*Ballradius ;rect19.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[17],NULL,&rect19);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==23)||(typecode==32)){
            SDL_Rect rect20; rect20.x =x-Ballradius ;rect20.y=y-Ballradius ; rect20.w =2*Ballradius ;rect20.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[18],NULL,&rect20);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==24)||(typecode==42)){
            SDL_Rect rect21; rect21.x =x-Ballradius ;rect21.y=y-Ballradius ; rect21.w =2*Ballradius ;rect21.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[19],NULL,&rect21);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==25)||(typecode==52)){
            SDL_Rect rect22; rect22.x =x-Ballradius ;rect22.y=y-Ballradius ; rect22.w =2*Ballradius ;rect22.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[20],NULL,&rect22);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==26)||(typecode==62)){
            SDL_Rect rect23; rect23.x =x-Ballradius ;rect23.y=y-Ballradius ; rect23.w =2*Ballradius ;rect23.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[21],NULL,&rect23);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==34)||(typecode==43)){
            SDL_Rect rect24; rect24.x =x-Ballradius ;rect24.y=y-Ballradius ; rect24.w =2*Ballradius ;rect24.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[22],NULL,&rect24);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==35)||(typecode==53)){
            SDL_Rect rect25; rect25.x =x-Ballradius ;rect25.y=y-Ballradius ; rect25.w =2*Ballradius ;rect25.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[23],NULL,&rect25);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==36)||(typecode==63)){
            SDL_Rect rect26; rect26.x =x-Ballradius ;rect26.y=y-Ballradius ; rect26.w =2*Ballradius ;rect26.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[24],NULL,&rect26);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==45)||(typecode==54)){
            SDL_Rect rect27; rect27.x =x-Ballradius ;rect27.y=y-Ballradius ; rect27.w =2*Ballradius ;rect27.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[25],NULL,&rect27);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==46)||(typecode==64)){
            SDL_Rect rect28; rect28.x =x-Ballradius ;rect28.y=y-Ballradius ; rect28.w =2*Ballradius ;rect28.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[26],NULL,&rect28);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
        else if ((typecode==56)||(typecode==65)){
            SDL_Rect rect29; rect29.x =x-Ballradius ;rect29.y=y-Ballradius ; rect29.w =2*Ballradius ;rect29.h=2*Ballradius;
            SDL_RenderCopy(m_renderer,balls[27],NULL,&rect29);
            if(sloweffect){
                SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                sloweffect--;
            }
            else if(stopeffect){
                SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                stopeffect--;
            }
        }
    }
    else{
        switch(typecode){
            case 80:
                SDL_Rect rect00; rect00.x =x-Ballradius ;rect00.y=y-Ballradius ; rect00.w =2*Ballradius ;rect00.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[29],NULL,&rect00);
                break;
            case 90:
                SDL_Rect rect01; rect01.x =x-Ballradius ;rect01.y=y-Ballradius ; rect01.w =2*Ballradius ;rect01.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[28],NULL,&rect01);
                if(sloweffect){
                    SDL_Rect rect60; rect60.x =x-Ballradius ;rect60.y=y-Ballradius ; rect60.w =2*Ballradius ;rect60.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[34],NULL,&rect60);
                    sloweffect--;
                }
                else if(stopeffect){
                    SDL_Rect rect61; rect61.x =x-Ballradius ;rect61.y=y-Ballradius ; rect61.w =2*Ballradius ;rect61.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,balls[35],NULL,&rect61);
                    stopeffect--;
                }
                break;
            case 100:
                SDL_Rect rect30; rect30.x =x-Ballradius ;rect30.y=y-Ballradius ; rect30.w =2*Ballradius ;rect30.h=2*Ballradius;
                SDL_RenderCopy(m_renderer,balls[30],NULL,&rect30);
                break;
        }

    }

}
void drawmyballs(SDL_Renderer * m_renderer,vector<SDL_Texture *> balls){
    if((endoftheline)){
        SDL_Rect rect31; rect31.x =endline.x;rect31.y=endline.y-50;rect31.w =1000;rect31.h=50;
        SDL_RenderCopy(m_renderer,balls[31],NULL,&rect31);
    }
    if(!firedball.isempty) {
        drawthisball(m_renderer,int(firedball.x),int(firedball.y),firedball.typecode,balls,firedball.sloweffect,firedball.stopeffect);
    }
    if(!backupball.isempty) {
        drawthisball(m_renderer,int(backupball.x),int(backupball.y),backupball.typecode,balls,backupball.sloweffect,backupball.stopeffect);
    }
    for(int i=0;i<50;i++) {
        for (int j = 0; j < Maxballseachrow; j++) {
            drawthisball(m_renderer,int(rowofballs[i][j].x),int(rowofballs[i][j].y),rowofballs[i][j].typecode,balls,rowofballs[i][j].sloweffect,rowofballs[i][j].stopeffect);
        }
    }
    for(int i=0;i<unboundballs.size();i++){
        drawthisball(m_renderer,int(unboundballs[i].x),int(unboundballs[i].y),unboundballs[i].typecode,balls,unboundballs[i].sloweffect,unboundballs[i].stopeffect);
    }
}

void drawanimationframes(SDL_Renderer * m_renderer,vector <vector<SDL_Texture *>> animations,int totalframes,int linger){
    for(int k=0;k<3;k++){
        if((UIbaranimationlocation[k].n)){
            SDL_Rect rect62; rect62.x =UIbaranimationlocation[k].x;rect62.y=UIbaranimationlocation[k].y;rect62.w =104;rect62.h=155;
            SDL_RenderCopy(m_renderer,animations[30][0],NULL,&rect62);
            UIbaranimationlocation[k].n--;
        }
    }

    for(int k=3;k<5;k++){
        if((UIbaranimationlocation[k].n)){
            SDL_Rect rect63; rect63.x =UIbaranimationlocation[k].x;rect63.y=UIbaranimationlocation[k].y;rect63.w =54;rect63.h=56;
            SDL_RenderCopy(m_renderer,animations[30][1],NULL,&rect63);
            UIbaranimationlocation[k].n--;
        }
    }

    if(laserlocation.size()>0){
        if(laserlocation[0].n<4*linger){
            thickLineRGBA(m_renderer,W/2,665,laserlocation[0].x,laserlocation[0].y,3*(laserlocation[0].n/linger+1),255,0,0,255);
        }
        else{
            thickLineRGBA(m_renderer,W/2,665,laserlocation[0].x,laserlocation[0].y,24-3*(laserlocation[0].n/linger+1),255,0,0,255);
        }
        laserlocation[0].n++;
        if(laserlocation[0].n==totalframes*linger){
            laserlocation.clear();
        }
    }
    if(explosionlocation.size()>0){
        SDL_Rect rect32; rect32.x =explosionlocation[0].x-175;rect32.y=explosionlocation[0].y-175;rect32.w =350;rect32.h=350;
        SDL_RenderCopy(m_renderer,animations[0][int(explosionlocation[0].n/linger)],NULL,&rect32);
        explosionlocation[0].n++;
        if(explosionlocation[0].n==totalframes*linger){
            explosionlocation.clear();
        }
    }
    if(balldestructionlocation.size()>0){
        for(int k=0;k<balldestructionlocation.size();k++){
            if(balldestructionlocation[k].typecode<10){
                switch(balldestructionlocation[k].typecode){
                    case 1:
                        SDL_Rect rect33; rect33.x=balldestructionlocation[k].x-Ballradius;rect33.y=balldestructionlocation[k].y-Ballradius;rect33.w =2*Ballradius;rect33.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[1][int(balldestructionlocation[k].n/linger)],NULL,&rect33);
                        balldestructionlocation[k].n++;
                        break;
                    case 2:
                        SDL_Rect rect34; rect34.x=balldestructionlocation[k].x-Ballradius;rect34.y=balldestructionlocation[k].y-Ballradius;rect34.w =2*Ballradius;rect34.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[2][int(balldestructionlocation[k].n/linger)],NULL,&rect34);
                        balldestructionlocation[k].n++;
                        break;
                    case 3:
                        SDL_Rect rect35; rect35.x=balldestructionlocation[k].x-Ballradius;rect35.y=balldestructionlocation[k].y-Ballradius;rect35.w =2*Ballradius;rect35.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[3][int(balldestructionlocation[k].n/linger)],NULL,&rect35);
                        balldestructionlocation[k].n++;
                        break;
                    case 4:
                        SDL_Rect rect36; rect36.x=balldestructionlocation[k].x-Ballradius;rect36.y=balldestructionlocation[k].y-Ballradius;rect36.w =2*Ballradius;rect36.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[4][int(balldestructionlocation[k].n/linger)],NULL,&rect36);
                        balldestructionlocation[k].n++;
                        break;
                    case 5:
                        SDL_Rect rect37; rect37.x=balldestructionlocation[k].x-Ballradius;rect37.y=balldestructionlocation[k].y-Ballradius;rect37.w =2*Ballradius;rect37.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[5][int(balldestructionlocation[k].n/linger)],NULL,&rect37);
                        balldestructionlocation[k].n++;
                        break;
                    case 6:
                        SDL_Rect rect38; rect38.x=balldestructionlocation[k].x-Ballradius;rect38.y=balldestructionlocation[k].y-Ballradius;rect38.w =2*Ballradius;rect38.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[6][int(balldestructionlocation[k].n/linger)],NULL,&rect38);
                        balldestructionlocation[k].n++;
                        break;
                    case 7:
                        SDL_Rect rect39; rect39.x=balldestructionlocation[k].x-Ballradius;rect39.y=balldestructionlocation[k].y-Ballradius;rect39.w =2*Ballradius;rect39.h=2*Ballradius;
                        SDL_RenderCopy(m_renderer,animations[22][int(balldestructionlocation[k].n/linger)],NULL,&rect39);
                        balldestructionlocation[k].n++;
                        break;
//                    case -1:
//                        SDL_Rect rect58; rect58.x=balldestructionlocation[k].x-Ballradius;rect58.y=balldestructionlocation[k].y-Ballradius;rect58.w =2*Ballradius;rect58.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[24][int(balldestructionlocation[k].n/linger)],NULL,&rect58);
//                        balldestructionlocation[k].n++;
//                        break;
//                    case -2:
//                        SDL_Rect rect59; rect59.x=balldestructionlocation[k].x-Ballradius;rect59.y=balldestructionlocation[k].y-Ballradius;rect59.w =2*Ballradius;rect59.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[25][int(balldestructionlocation[k].n/linger)],NULL,&rect59);
//                        balldestructionlocation[k].n++;
//                        break;
//                    case -3:
//                        SDL_Rect rect60; rect60.x=balldestructionlocation[k].x-Ballradius;rect60.y=balldestructionlocation[k].y-Ballradius;rect60.w =2*Ballradius;rect60.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[26][int(balldestructionlocation[k].n/linger)],NULL,&rect60);
//                        balldestructionlocation[k].n++;
//                        break;
//                    case -4:
//                        SDL_Rect rect61; rect61.x=balldestructionlocation[k].x-Ballradius;rect61.y=balldestructionlocation[k].y-Ballradius;rect61.w =2*Ballradius;rect61.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[27][int(balldestructionlocation[k].n/linger)],NULL,&rect61);
//                        balldestructionlocation[k].n++;
//                        break;
//                    case -5:
//                        SDL_Rect rect62; rect62.x=balldestructionlocation[k].x-Ballradius;rect62.y=balldestructionlocation[k].y-Ballradius;rect62.w =2*Ballradius;rect62.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[28][int(balldestructionlocation[k].n/linger)],NULL,&rect62);
//                        balldestructionlocation[k].n++;
//                        break;
//                    case -6:
//                        SDL_Rect rect63; rect63.x=balldestructionlocation[k].x-Ballradius;rect63.y=balldestructionlocation[k].y-Ballradius;rect63.w =2*Ballradius;rect63.h=2*Ballradius;
//                        SDL_RenderCopy(m_renderer,animations[29][int(balldestructionlocation[k].n/linger)],NULL,&rect63);
//                        balldestructionlocation[k].n++;
//                        break;
                }
            }
            else{
                if((balldestructionlocation[k].typecode==12)||(balldestructionlocation[k].typecode==21)){
                    SDL_Rect rect40; rect40.x=balldestructionlocation[k].x-Ballradius;rect40.y=balldestructionlocation[k].y-Ballradius;rect40.w =2*Ballradius;rect40.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[7][int(balldestructionlocation[k].n/linger)],NULL,&rect40);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==13)||(balldestructionlocation[k].typecode==31)){
                    SDL_Rect rect41; rect41.x=balldestructionlocation[k].x-Ballradius;rect41.y=balldestructionlocation[k].y-Ballradius;rect41.w =2*Ballradius;rect41.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[8][int(balldestructionlocation[k].n/linger)],NULL,&rect41);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==14)||(balldestructionlocation[k].typecode==41)){
                    SDL_Rect rect42; rect42.x=balldestructionlocation[k].x-Ballradius;rect42.y=balldestructionlocation[k].y-Ballradius;rect42.w =2*Ballradius;rect42.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[9][int(balldestructionlocation[k].n/linger)],NULL,&rect42);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==15)||(balldestructionlocation[k].typecode==51)){
                    SDL_Rect rect43; rect43.x=balldestructionlocation[k].x-Ballradius;rect43.y=balldestructionlocation[k].y-Ballradius;rect43.w =2*Ballradius;rect43.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[10][int(balldestructionlocation[k].n/linger)],NULL,&rect43);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==16)||(balldestructionlocation[k].typecode==61)){
                    SDL_Rect rect44; rect44.x=balldestructionlocation[k].x-Ballradius;rect44.y=balldestructionlocation[k].y-Ballradius;rect44.w =2*Ballradius;rect44.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[11][int(balldestructionlocation[k].n/linger)],NULL,&rect44);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==23)||(balldestructionlocation[k].typecode==32)){
                    SDL_Rect rect45; rect45.x=balldestructionlocation[k].x-Ballradius;rect45.y=balldestructionlocation[k].y-Ballradius;rect45.w =2*Ballradius;rect45.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[12][int(balldestructionlocation[k].n/linger)],NULL,&rect45);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==24)||(balldestructionlocation[k].typecode==42)){
                    SDL_Rect rect46; rect46.x=balldestructionlocation[k].x-Ballradius;rect46.y=balldestructionlocation[k].y-Ballradius;rect46.w =2*Ballradius;rect46.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[13][int(balldestructionlocation[k].n/linger)],NULL,&rect46);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==25)||(balldestructionlocation[k].typecode==52)){
                    SDL_Rect rect47; rect47.x=balldestructionlocation[k].x-Ballradius;rect47.y=balldestructionlocation[k].y-Ballradius;rect47.w =2*Ballradius;rect47.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[14][int(balldestructionlocation[k].n/linger)],NULL,&rect47);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==26)||(balldestructionlocation[k].typecode==62)){
                    SDL_Rect rect48; rect48.x=balldestructionlocation[k].x-Ballradius;rect48.y=balldestructionlocation[k].y-Ballradius;rect48.w =2*Ballradius;rect48.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[15][int(balldestructionlocation[k].n/linger)],NULL,&rect48);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==34)||(balldestructionlocation[k].typecode==43)){
                    SDL_Rect rect49; rect49.x=balldestructionlocation[k].x-Ballradius;rect49.y=balldestructionlocation[k].y-Ballradius;rect49.w =2*Ballradius;rect49.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[16][int(balldestructionlocation[k].n/linger)],NULL,&rect49);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==35)||(balldestructionlocation[k].typecode==53)){
                    SDL_Rect rect50; rect50.x=balldestructionlocation[k].x-Ballradius;rect50.y=balldestructionlocation[k].y-Ballradius;rect50.w =2*Ballradius;rect50.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[17][int(balldestructionlocation[k].n/linger)],NULL,&rect50);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==36)||(balldestructionlocation[k].typecode==63)){
                    SDL_Rect rect51; rect51.x=balldestructionlocation[k].x-Ballradius;rect51.y=balldestructionlocation[k].y-Ballradius;rect51.w =2*Ballradius;rect51.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[18][int(balldestructionlocation[k].n/linger)],NULL,&rect51);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==45)||(balldestructionlocation[k].typecode==54)){
                    SDL_Rect rect52; rect52.x=balldestructionlocation[k].x-Ballradius;rect52.y=balldestructionlocation[k].y-Ballradius;rect52.w =2*Ballradius;rect52.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[19][int(balldestructionlocation[k].n/linger)],NULL,&rect52);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==46)||(balldestructionlocation[k].typecode==64)){
                    SDL_Rect rect53; rect53.x=balldestructionlocation[k].x-Ballradius;rect53.y=balldestructionlocation[k].y-Ballradius;rect53.w =2*Ballradius;rect53.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[20][int(balldestructionlocation[k].n/linger)],NULL,&rect53);
                    balldestructionlocation[k].n++;
                }
                else if((balldestructionlocation[k].typecode==56)||(balldestructionlocation[k].typecode==65)){
                    SDL_Rect rect54; rect54.x=balldestructionlocation[k].x-Ballradius;rect54.y=balldestructionlocation[k].y-Ballradius;rect54.w =2*Ballradius;rect54.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[21][int(balldestructionlocation[k].n/linger)],NULL,&rect54);
                    balldestructionlocation[k].n++;
                }
                else if(balldestructionlocation[k].typecode==90){
                    SDL_Rect rect57; rect57.x=balldestructionlocation[k].x-Ballradius;rect57.y=balldestructionlocation[k].y-Ballradius;rect57.w =2*Ballradius;rect57.h=2*Ballradius;
                    SDL_RenderCopy(m_renderer,animations[23][int(balldestructionlocation[k].n/linger)],NULL,&rect57);
                    balldestructionlocation[k].n++;
                }



            }

        }
        if(balldestructionlocation[0].n==totalframes*linger){
            balldestructionlocation.clear();
        }
    }




}
void movemyballs(float dy){
    if(explosionlocation.size()>0){
        explosionlocation[0].y+=dy;
    }
    if(balldestructionlocation.size()>0){
        for(int k=0;k<balldestructionlocation.size();k++){
            balldestructionlocation[k].y+=dy;
        }
    }
    if(endoftheline){
        endline.y+=dy;
    }
    if((firedball.isflying)&&(endoftheline)&&(firedball.y<(endline.y+Ballradius))){
        firedball.x-=firedball.vx;  firedball.y-=firedball.vy;
        settlenearendline(firedball);
    }


    if((firedball.isflying)&&(!firedball.istarget)){

        if((firedball.x>verticalline1x+Ballradius)&&(firedball.x<verticalline2x-Ballradius)){
            firedball.x+=firedball.vx;  firedball.y+=firedball.vy;
        }
        else if(firedball.x<verticalline1x+Ballradius){
            firedball.vx=-firedball.vx;
            firedball.x+=firedball.vx;  firedball.y+=firedball.vy;
        }
        else if(firedball.x>verticalline2x-Ballradius){
            firedball.vx=-firedball.vx;
            firedball.x+=firedball.vx;  firedball.y+=firedball.vy;
        }

    }

    for(int i=0;i<50;i++) {
        for (int j = 0; j < Maxballseachrow; j++) {
            rowofballs[i][j].y += dy;
        }
    }
    for(int k=0;k<unboundballs.size();k++){
        unboundballs[k].vy+=g;
        unboundballs[k].y+=unboundballs[k].vy;
        unboundballs[k].x+=unboundballs[k].vx;
    }
    bool clear=false;
    if(unboundballs.size()>0){
        if(unboundballs[0].y>1000){
            clear=true;
        }
    }
    if(clear){
        unboundballs.clear();
    }

}

void createfiredball(US_user& user){
    firedball.isempty=false;
    if(coinflip(PB1)){
        int random=rand()%15+1;
        if(random<7){
            user.multicolorBallCount++;
        }
        else if(random<10){
            user.laserCount++;
        }
        else if(random<14){
            user.bombCount++;
        }
        else if(random<15){
            user.stopCount++;
        }
        else if(random<16){
            user.slowMotionCount++;
        }
    }
    firedball.typecode=rand()%6+1;
//    assignfiredballcode(firedball.typecode);
    firedball.x=W/2;           firedball.y=665;
    firedball.istarget=false;   firedball.isflying=false;
}
void createbackupball(US_user& user){
    backupball.isempty=false;
    if(coinflip(PB1)){
        int random=rand()%15+1;
        if(random<3){
            user.laserCount++;
            UIbaranimationlocation[1].n=50;
        }
        else if(random<6){
            user.multicolorBallCount++;
            UIbaranimationlocation[2].n=50;
        }
        else if(random<11){
            user.bombCount++;
            UIbaranimationlocation[0].n=50;
        }
        else if(random<13){
            user.stopCount++;
            UIbaranimationlocation[4].n=50;
        }
        else if(random<16){
            user.slowMotionCount++;
            UIbaranimationlocation[3].n=50;
        }
    }
//    backupball.typecode=rand()%6+1;
    assignfiredballcode(backupball.typecode);
    backupball.x=848;        backupball.y=710;
    backupball.istarget=false;   backupball.isflying=false;
}
void assignfiredballcode(int &result){
    srand(time(NULL));
    updateballtypecount();
    int totalcount=0;
    for(int k=1;k<7;k++){
        totalcount+=balltypecount[k];
    }
    int randomroll;
    if(totalcount){
        randomroll=(rand()%totalcount)+1;
    }

    if(randomroll<balltypecount[1]+1){
        result=1;
    }
    else if(randomroll-balltypecount[1]<balltypecount[2]+1){
        result=2;
    }
    else if(randomroll-balltypecount[1]-balltypecount[2]<balltypecount[3]+1){
        result=3;
    }
    else if(randomroll-balltypecount[1]-balltypecount[2]-balltypecount[3]<balltypecount[4]+1){
        result=4;
    }
    else if(randomroll-balltypecount[1]-balltypecount[2]-balltypecount[3]-balltypecount[4]<balltypecount[5]+1){
        result=5;
    }
    else if(randomroll-balltypecount[1]-balltypecount[2]-balltypecount[3]-balltypecount[4]-balltypecount[5]<balltypecount[6]+1){
        result=6;
    }
    else{
        result=(rand()%6+1);
    }
}
void updateballtypecount(){
    for(int k=0;k<7;k++){
        balltypecount[k]=0;
    }
    for(int i=0;i<50;i++){
        for(int j=0;j<Maxballseachrow;j++){
            if(rowofballs[i][j].y>0){
                if((!rowofballs[i][j].isempty)&&(!rowofballs[i][j].ismulticolored)&&(!rowofballs[i][j].isstone)){
                    for(int k=1;k<7;k++){
                        if(iscompatible(rowofballs[i][j].typecode,k)){
                            balltypecount[k]++;
                        }
                    }
                }
            }
        }
    }
}

void assignvelocity(Ball &ball){
    float d=sqrt(((clickdown.x-ball.x)*(clickdown.x-ball.x)+(clickdown.y-ball.y)*(clickdown.y-ball.y)));
    ball.vx=firedballspeed*((clickdown.x-ball.x)/d);  ball.vy=firedballspeed*((clickdown.y-ball.y)/d);
}


void updaterowclear(){
    for(int i=0;i<50;i++){
        rowclear[i]=true;
        for (int j=0;j<Maxballseachrow;j++){
            if(!rowofballs[i][j].isempty){
                rowclear[i]=false;
            }
        }
    }
}

void examinemyballs(){
    int x,y;
    bool successfulsettlement=false;
    if(firedball.isflying){
        for (int i=0;i<50;i++){
            if((!rowclear[i])&&(firedball.y<rowofballs[i][0].y+2*Ballradius)){
                for(int j=0;j<Maxballseachrow;j++){
                    float distance=sqrt(((firedball.x-rowofballs[i][j].x)*(firedball.x-rowofballs[i][j].x))+((firedball.y-rowofballs[i][j].y)*(firedball.y-rowofballs[i][j].y)));
                    if((distance<2*Ballradius-7)&&(!rowofballs[i][j].isempty)&&(firedball.isflying)){
                        settlefiredball(i,j,successfulsettlement,x,y);
                        if(successfulsettlement) {
                            checktimeeffectcancellation(x,y);
                            firedball.isflying = false; firedball.istarget = true;
                            updateflags(rowofballs[x][y]);
                            checkneeded=true;

                        }
                    }
                }
            }
        }
    }

    if(checkneeded){
        if(rowofballs[x][y].typecode==80){
            powerballsused++;
            explodemyballs(x,y,explosionradius);
        }
        else if(rowofballs[x][y].typecode==90){
            powerballsused++;
            multicolored(x,y);
            tobedestroyedballs.push_back({x,y});
            //remove redundant elements in vector

            if(tobedestroyedballs.size()>2){
                destroymyballs(tobedestroyedballs);
            }
            tobedestroyedballs.clear();
        }
        else{
            tobedestroyedballs.clear();
            tobedestroyedballs.push_back({x,y});
            addhomiestovector(x,y,rowofballs[x][y].typecode);
            if(tobedestroyedballs.size()>2){
                destroymyballs(tobedestroyedballs);
            }
            tobedestroyedballs.clear();
        }
        identifyunboundballs();
        dropmyballs();
        checkneeded=false;
    }
}
void dropmyballs(){
    for(int i=0;i<39;i++){
        for(int j=0;j<Maxballseachrow;j++){
            if((!rowofballs[i][j].isempty)&&(!rowofballs[i][j].isconnected)){
                if(rowofballs[i][j].y>-50){
                    if((rowofballs[i][j].sloweffect)&&(!isStopped)){
                        isSlow=true;
                        playsound(14);
                        ballSpeed=Balldecentspeed/2;
                        now=clock();
                    }
                    else if(rowofballs[i][j].stopeffect){
                        isStopped=true;
                        playsound(15);
                        ballSpeed=0;
                        now=clock();
                    }
                    unboundballs.push_back(rowofballs[i][j]);
                    destroyedballs++;

                }
                rowofballs[i][j].typecode=0;
                updateflags(rowofballs[i][j]);

            }
        }
    }
    for(int i=0;i<unboundballs.size();i++){
        unboundballs[i].vy=rand()%droppedballinitialspeedy-droppedballinitialspeedy/2;
        unboundballs[i].vx=rand()%droppedballinitialspeedx-droppedballinitialspeedx/2;
    }
}

void addhomiestovector(int i,int j,const int typecode){
    if(i%2==1){    //odd i
        if((iscompatible(rowofballs[i][j+1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i,j+1))){
            tobedestroyedballs.push_back({i,j+1});
            addhomiestovector(i,j+1,typecode);
        }
        if((iscompatible(rowofballs[i][j-1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i,j-1))){
            tobedestroyedballs.push_back({i,j-1});
            addhomiestovector(i,j-1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j+1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i+1,j+1))){
            tobedestroyedballs.push_back({i+1,j+1});
            addhomiestovector(i+1,j+1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i+1,j))){
            tobedestroyedballs.push_back({i+1,j});
            addhomiestovector(i+1,j,typecode);
        }
        if((iscompatible(rowofballs[i-1][j+1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i-1,j+1))){
            tobedestroyedballs.push_back({i-1,j+1});
            addhomiestovector(i-1,j+1,typecode);
        }
        if((iscompatible(rowofballs[i-1][j].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i-1,j))){
            tobedestroyedballs.push_back({i-1,j});
            addhomiestovector(i-1,j,typecode);
        }
    }
    else{   //even i
        if((iscompatible(rowofballs[i][j+1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i,j+1))){
            tobedestroyedballs.push_back({i,j+1});
            addhomiestovector(i,j+1,typecode);
        }
        if((iscompatible(rowofballs[i][j-1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i,j-1))){
            tobedestroyedballs.push_back({i,j-1});
            addhomiestovector(i,j-1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i+1,j))){
            tobedestroyedballs.push_back({i+1,j});
            addhomiestovector(i+1,j,typecode);
        }
        if((iscompatible(rowofballs[i+1][j-1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i+1,j-1))){
            tobedestroyedballs.push_back({i+1,j-1});
            addhomiestovector(i+1,j-1,typecode);
        }
        if((iscompatible(rowofballs[i-1][j].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i-1,j))){
            tobedestroyedballs.push_back({i-1,j});
            addhomiestovector(i-1,j,typecode);
        }
        if((iscompatible(rowofballs[i-1][j-1].typecode,typecode))&&(!alreadyhere(tobedestroyedballs,i-1,j-1))){
            tobedestroyedballs.push_back({i-1,j-1});
            addhomiestovector(i-1,j-1,typecode);
        }
    }
}
void addtemphomiestovector(int i,int j,int typecode){
    if(i%2==1){    //odd i
        if((iscompatible(rowofballs[i][j+1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i,j+1))){
            temptobedestroyedballs.push_back({i,j+1});
            addtemphomiestovector(i,j+1,typecode);
        }
        if((iscompatible(rowofballs[i][j-1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i,j-1))){
            temptobedestroyedballs.push_back({i,j-1});
            addtemphomiestovector(i,j-1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j+1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i+1,j+1))){
            temptobedestroyedballs.push_back({i+1,j+1});
            addtemphomiestovector(i+1,j+1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i+1,j))){
            temptobedestroyedballs.push_back({i+1,j});
            addtemphomiestovector(i+1,j,typecode);
        }
        if((iscompatible(rowofballs[i-1][j+1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i-1,j+1))){
            temptobedestroyedballs.push_back({i-1,j+1});
            addtemphomiestovector(i-1,j+1,typecode);
        }
        if((iscompatible(rowofballs[i-1][j].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i-1,j))){
            temptobedestroyedballs.push_back({i-1,j});
            addtemphomiestovector(i-1,j,typecode);
        }
    }
    else{   //even i
        if((iscompatible(rowofballs[i][j+1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i,j+1))){
            temptobedestroyedballs.push_back({i,j+1});
            addtemphomiestovector(i,j+1,typecode);
        }
        if((iscompatible(rowofballs[i][j-1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i,j-1))){
            temptobedestroyedballs.push_back({i,j-1});
            addtemphomiestovector(i,j-1,typecode);
        }
        if((iscompatible(rowofballs[i+1][j].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i+1,j))){
            temptobedestroyedballs.push_back({i+1,j});
            addtemphomiestovector(i+1,j,typecode);
        }
        if((iscompatible(rowofballs[i+1][j-1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i+1,j-1))){
            temptobedestroyedballs.push_back({i+1,j-1});
            addtemphomiestovector(i+1,j-1,typecode);
        }
        if((iscompatible(rowofballs[i-1][j].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i-1,j))){
            temptobedestroyedballs.push_back({i-1,j});
            addtemphomiestovector(i-1,j,typecode);
        }
        if((iscompatible(rowofballs[i-1][j-1].typecode,typecode))&&(!alreadyhere(temptobedestroyedballs,i-1,j-1))){
            temptobedestroyedballs.push_back({i-1,j-1});
            addtemphomiestovector(i-1,j-1,typecode);
        }
    }
}
void multicolored(int i,int j){
    temptobedestroyedballs.clear();
    tobedestroyedballs.clear();
    for(int k=1;k<7;k++){
        if(isaround(i,j,k)){
            addtemphomiestovector(i,j,k);
            if(temptobedestroyedballs.size()>2){
                tobedestroyedballs.insert(tobedestroyedballs.end(),temptobedestroyedballs.begin(),temptobedestroyedballs.end());
            }
            temptobedestroyedballs.clear();
        }
    }
    removeredundancy(tobedestroyedballs);
}
bool comparePoint2D(Point2D Point1, Point2D Point2){
    if(Point1.x<Point2.x){
        return true;
    }
    else if(Point1.x==Point2.x){
        if(Point1.y<Point2.y){
            return true;
        }
    }
    return false;

}
void removeredundancy(vector <Point2D> &vector1){
    sort(vector1.begin(),vector1.end(), comparePoint2D);

    for(int k=1;k<vector1.size();k++){
        if((vector1[k].x==vector1[k-1].x)&&(vector1[k].y==vector1[k-1].y)){
            vector1.erase(vector1.begin()+k);
            k--;

        }
    }

}

bool alreadyhere(vector<Point2D> homies,int i,int j){
    for(int k=0;k<homies.size();k++){
        if((homies[k].x==i)&&(homies[k].y==j)){
            return true;
        }
    }
    return false;
}
void settlefiredball(int i,int j,bool &b,int &x,int&y){
    int dx=firedball.x-rowofballs[i][j].x,dy=firedball.y-rowofballs[i][j].y;

    if(i%2==0) {
        //evenrows
        if(dy>10){
            if(dx>0){
                if(rowofballs[i-1][j].isempty) {
                    transformemptyball(rowofballs[i - 1][j], firedball);
                    b = true;
                    x=i-1; y=j;
                }
            }
            else{
                if(rowofballs[i-1][j-1].isempty) {
                    transformemptyball(rowofballs[i - 1][j - 1], firedball);
                    b = true;
                    x=i-1; y=j-1;
                }
            }
        }
        else if(dy<-10){
            if(dx>0){
                if(rowofballs[i+1][j].isempty) {
                    transformemptyball(rowofballs[i + 1][j], firedball);
                    b = true;
                    x=i+1; y=j;
                }
            }
            else{
                if(rowofballs[i+1][j-1].isempty) {
                    transformemptyball(rowofballs[i + 1][j - 1], firedball);
                    b = true;
                    x=i+1; y=j-1;
                }
            }
        }
        else{
            if(dx>0){
                if(rowofballs[i][j+1].isempty) {
                    transformemptyball(rowofballs[i][j + 1], firedball);
                    b = true;
                    x=i; y=j+1;
                }
            }
            else{
                if(rowofballs[i][j-1].isempty) {
                    transformemptyball(rowofballs[i][j - 1], firedball);
                    b = true;
                    x=i; y=j-1;
                }
            }
        }

    }
    else{
        //oddrows
        if(dy>10){
            if(dx>0){
                if(rowofballs[i-1][j+1].isempty) {
                    transformemptyball(rowofballs[i - 1][j + 1], firedball);
                    b = true;
                    x=i-1; y=j+1;
                }
            }
            else{
                if(rowofballs[i-1][j].isempty) {
                    transformemptyball(rowofballs[i - 1][j], firedball);
                    b = true;
                    x=i-1; y=j;
                }
            }
        }
        else if(dy<-10){
            if(dx>0){
                if(rowofballs[i+1][j+1].isempty) {
                    transformemptyball(rowofballs[i + 1][j + 1], firedball);
                    b = true;
                    x=i+1; y=j+1;
                }
            }
            else {
                if(rowofballs[i+1][j].isempty) {
                    transformemptyball(rowofballs[i + 1][j], firedball);
                    b = true;
                    x=i+1; y=j;
                }
            }
        }
        else{
            if(dx>0){
                if(rowofballs[i][j+1].isempty) {
                    transformemptyball(rowofballs[i][j + 1], firedball);
                    b = true;
                    x=i; y=j+1;
                }
            }
            else {
                if(rowofballs[i][j-1].isempty) {
                    transformemptyball(rowofballs[i][j - 1], firedball);
                    b = true;
                    x=i; y=j-1;
                }
            }
        }

    }
}
void checktimeeffectcancellation(int i, int j){
    if(i%2){
        if((!iscompatible(rowofballs[i][j-1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i][j-1].sloweffect>0||rowofballs[i][j-1].stopeffect>0)){
            rowofballs[i][j-1].sloweffect=0;    rowofballs[i][j-1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i][j+1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i][j+1].sloweffect>0||rowofballs[i][j+1].stopeffect>0)){
            rowofballs[i][j+1].sloweffect=0;    rowofballs[i][j+1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i+1][j+1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i+1][j+1].sloweffect>0||rowofballs[i+1][j+1].stopeffect>0)){
            rowofballs[i+1][j+1].sloweffect=0;    rowofballs[i+1][j+1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i+1][j].typecode,rowofballs[i][j].typecode))&&(rowofballs[i+1][j].sloweffect>0||rowofballs[i+1][j].stopeffect>0)){
            rowofballs[i+1][j].sloweffect=0;    rowofballs[i+1][j].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i-1][j+1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i-1][j+1].sloweffect>0||rowofballs[i-1][j+1].stopeffect>0)){
            rowofballs[i-1][j+1].sloweffect=0;    rowofballs[i-1][j+1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i-1][j].typecode,rowofballs[i][j].typecode))&&(rowofballs[i-1][j].sloweffect>0||rowofballs[i-1][j].stopeffect>0)){
            rowofballs[i-1][j].sloweffect=0;    rowofballs[i-1][j].stopeffect=0;
        }

    }
    else{//even i
        if((!iscompatible(rowofballs[i][j-1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i][j-1].sloweffect>0||rowofballs[i][j-1].stopeffect>0)){
            rowofballs[i][j-1].sloweffect=0;    rowofballs[i][j-1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i][j+1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i][j+1].sloweffect>0||rowofballs[i][j+1].stopeffect>0)){
            rowofballs[i][j+1].sloweffect=0;    rowofballs[i][j+1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i+1][j-1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i+1][j-1].sloweffect>0||rowofballs[i+1][j-1].stopeffect>0)){
            rowofballs[i+1][j-1].sloweffect=0;    rowofballs[i+1][j-1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i+1][j].typecode,rowofballs[i][j].typecode))&&(rowofballs[i+1][j].sloweffect>0||rowofballs[i+1][j].stopeffect>0)){
            rowofballs[i+1][j].sloweffect=0;    rowofballs[i+1][j].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i-1][j-1].typecode,rowofballs[i][j].typecode))&&(rowofballs[i-1][j-1].sloweffect>0||rowofballs[i-1][j-1].stopeffect>0)){
            rowofballs[i-1][j-1].sloweffect=0;    rowofballs[i-1][j-1].stopeffect=0;
        }
        if((!iscompatible(rowofballs[i-1][j].typecode,rowofballs[i][j].typecode))&&(rowofballs[i-1][j].sloweffect>0||rowofballs[i-1][j].stopeffect>0)){
            rowofballs[i-1][j].sloweffect=0;    rowofballs[i-1][j].stopeffect=0;
        }
    }
}
void settlenearendline(Ball &firedball){
    float d; bool done=false;
    for(int j=0;j<Maxballseachrow;j++){
        d=sqrt((firedball.x-rowofballs[39][j].x)*(firedball.x-rowofballs[39][j].x)+(firedball.y-rowofballs[39][j].y)*(firedball.y-rowofballs[39][j].y));
        if((!done)&&(rowofballs[39][j].isempty)&&(d<=Ballradius+5)){
            rowofballs[39][j].typecode=firedball.typecode;  updateflags(rowofballs[39][j]);
            checktimeeffectcancellation(39,j);
            if(rowofballs[39][j].typecode==80){
                explodemyballs(39,j,explosionradius);
                rowofballs[39][j].typecode=0;   updateflags(rowofballs[39][j]);
                explosionlocation.push_back({rowofballs[39][j].x,rowofballs[39][j].y});
            }
            firedball.isflying=false;   firedball.istarget=true;
            done=true;
            checkneeded=true;
            tobedestroyedballs.clear();
            tobedestroyedballs.push_back({39,j});
            addhomiestovector(39,j,rowofballs[39][j].typecode);
            if(tobedestroyedballs.size()>2){
                destroymyballs(tobedestroyedballs);
            }
            tobedestroyedballs.clear();
        }
    }
}
void transformemptyball(Ball &ball1,Ball &ball2){
    ball1.typecode=ball2.typecode;  ball1.isempty=false;
    ball1.isflying=false;   ball1.istarget=true;
    ball1.is2colored=ball2.is2colored;  ball1.ismulticolored=ball2.ismulticolored;
    ball1.isstone=ball2.isstone;    ball1.isexplosive=ball2.isexplosive;
    ball1.islocked=ball2.islocked;  ball1.isconnected=ball2.isconnected;
}
void identifyunboundballs(){
    for(int i=39;i>=0;i--){
        for(int j=0;j<Maxballseachrow;j++){
            rowofballs[i][j].isconnected=false;
        }
    }
    for(int j=0;j<Maxballseachrow;j++){
        if(!rowofballs[39][j].isempty){
            rowofballs[39][j].isconnected=true;
        }
    }
    for(int j=0;j<Maxballseachrow;j++){
        if(rowofballs[39][j].isconnected){
            connectmyballs(39,j);
        }
    }
}
void connectmyballs(int i,int j){
    if(i%2){//i odd
        if((!rowofballs[i][j-1].isempty)&&(!rowofballs[i][j-1].isconnected)&&(i<39)){
            rowofballs[i][j-1].isconnected=true;
            connectmyballs(i,j-1);
        }
        if((!rowofballs[i][j+1].isempty)&&(!rowofballs[i][j+1].isconnected)&&(i<39)){
            rowofballs[i][j+1].isconnected=true;
            connectmyballs(i,j+1);
        }
        if((!rowofballs[i+1][j+1].isempty)&&(!rowofballs[i+1][j+1].isconnected)&&(i+1<39)){
            rowofballs[i+1][j+1].isconnected=true;
            connectmyballs(i+1,j+1);
        }
        if((!rowofballs[i+1][j].isempty)&&(!rowofballs[i+1][j].isconnected)&&(i+1<39)){
            rowofballs[i+1][j].isconnected=true;
            connectmyballs(i+1,j);
        }
        if((!rowofballs[i-1][j+1].isempty)&&(!rowofballs[i-1][j+1].isconnected)&&(i-1<39)){
            rowofballs[i-1][j+1].isconnected=true;
            connectmyballs(i-1,j+1);
        }
        if((!rowofballs[i-1][j].isempty)&&(!rowofballs[i-1][j].isconnected)&&(i-1<39)){
            rowofballs[i-1][j].isconnected=true;
            connectmyballs(i-1,j);
        }
    }
    else{//i even
        if((!rowofballs[i][j-1].isempty)&&(!rowofballs[i][j-1].isconnected)&&(i<39)){
            rowofballs[i][j-1].isconnected=true;
            connectmyballs(i,j-1);
        }
        if((!rowofballs[i][j+1].isempty)&&(!rowofballs[i][j+1].isconnected)&&(i<39)){
            rowofballs[i][j+1].isconnected=true;
            connectmyballs(i,j+1);
        }
        if((!rowofballs[i+1][j-1].isempty)&&(!rowofballs[i+1][j-1].isconnected)&&(i+1<39)){
            rowofballs[i+1][j-1].isconnected=true;
            connectmyballs(i+1,j-1);
        }
        if((!rowofballs[i+1][j].isempty)&&(!rowofballs[i+1][j].isconnected)&&(i+1<39)){
            rowofballs[i+1][j].isconnected=true;
            connectmyballs(i+1,j);
        }
        if((!rowofballs[i-1][j-1].isempty)&&(!rowofballs[i-1][j-1].isconnected)&&(i-1<39)){
            rowofballs[i-1][j-1].isconnected=true;
            connectmyballs(i-1,j-1);
        }
        if((!rowofballs[i-1][j].isempty)&&(!rowofballs[i-1][j].isconnected)&&(i-1<39)){
            rowofballs[i-1][j].isconnected=true;
            connectmyballs(i-1,j);
        }
    }
}

void destroymyballs(vector<Point2D> &targets){

    for (int k=0;k<targets.size();k++){
        if((rowofballs[(targets[k].x)][(targets[k].y)].typecode>0)&&(!rowofballs[(targets[k].x)][(targets[k].y)].isempty)){
            if((rowofballs[(targets[k].x)][(targets[k].y)].y>-50)){
                if((rowofballs[(targets[k].x)][(targets[k].y)].sloweffect)&&(!isStopped)){
                    isSlow=true;
                    playsound(14);
                    ballSpeed=Balldecentspeed/2;
                    now=clock();
                }
                else if(rowofballs[(targets[k].x)][(targets[k].y)].stopeffect){
                    isStopped=true;
                    playsound(15);
                    ballSpeed=0;
                    now=clock();
                }
                balldestructionlocation.push_back({rowofballs[(targets[k].x)][(targets[k].y)].x,rowofballs[(targets[k].x)][(targets[k].y)].y,rowofballs[(targets[k].x)][(targets[k].y)].typecode});
                rowofballs[(targets[k].x)][(targets[k].y)].typecode=0;  destroyedballs++;
                updateflags(rowofballs[(targets[k].x)][(targets[k].y)]);
            }

        }
        else if((rowofballs[(targets[k].x)][(targets[k].y)].typecode<0)&&(!rowofballs[(targets[k].x)][(targets[k].y)].isempty)){
            if(rowofballs[(targets[k].x)][(targets[k].y)].y>-50){
//                balldestructionlocation.push_back({rowofballs[(targets[k].x)][(targets[k].y)].x,rowofballs[(targets[k].x)][(targets[k].y)].y,rowofballs[(targets[k].x)][(targets[k].y)].typecode});
                rowofballs[(targets[k].x)][(targets[k].y)].typecode*=-1;
                updateflags(rowofballs[(targets[k].x)][(targets[k].y)]);
            }
        }

    }
}
float distancefromline(int x,int y,int x1,int y1,int x2,int y2){
    float d,a,b,c,m;
    m=float((y2-y1))/float((x2-x1));
    a=m;
    b=-1.000;
    c=float(y1)-float(m*x1);
    d=(abs(a*x+b*y+c))/(sqrt(a*a+b*b));
    return d;
}
void lasermyballs(int x,int y){
    playsound(0);
    float d;
    for(int i=0;i<50;i++){
        for(int j=2;j<Maxballseachrow-2;j++){
            d=distancefromline(rowofballs[i][j].x,rowofballs[i][j].y,x,y,W/2,665);
            if((!rowofballs[i][j].isempty)&&(rowofballs[i][j].y>(-150))&&(d<Ballradius+10)){//change the number for more powerful laser
                if(rowofballs[i][j].typecode<0){
                    balldestructionlocation.push_back({rowofballs[i][j].x,rowofballs[i][j].y,-rowofballs[i][j].typecode});
                }
                else{
                    balldestructionlocation.push_back({rowofballs[i][j].x,rowofballs[i][j].y,rowofballs[i][j].typecode});
                }
                if((rowofballs[i][j].sloweffect)&&(!isStopped)){
                    isSlow=true;
                    playsound(14);
                    ballSpeed=Balldecentspeed/2;
                    now=clock();
                }
                else if(rowofballs[i][j].stopeffect){
                    isStopped=true;
                    playsound(15);
                    isSlow=false;
                    ballSpeed=0;
                    now=clock();
                }
                rowofballs[i][j].typecode=0;    updateflags(rowofballs[i][j]);  destroyedballs++;
            }
        }
    }
    identifyunboundballs();
    dropmyballs();
}


void explodemyballs(int i,int j,int n){
    explosionlocation.clear();
    explosionlocation.push_back({rowofballs[i][j].x,rowofballs[i][j].y});
    playsound(1);
    switch(n){
        case 1:
            if(i%2){//odd i
                rowofballs[i][j].typecode=0;  updateflags(rowofballs[i][j]);
                rowofballs[i][j+1].typecode=0;  updateflags(rowofballs[i][j+1]);
                rowofballs[i][j-1].typecode=0;  updateflags(rowofballs[i][j-1]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i+1][j]);
                rowofballs[i+1][j+1].typecode=0;  updateflags(rowofballs[i+1][j+1]);
                rowofballs[i-1][j].typecode=0;  updateflags(rowofballs[i-1][j]);
                rowofballs[i-1][j+1].typecode=0;  updateflags(rowofballs[i-1][j+1]);
            }
            else{//even i
                rowofballs[i][j].typecode=0;  updateflags(rowofballs[i][j]);
                rowofballs[i][j+1].typecode=0;  updateflags(rowofballs[i][j+1]);
                rowofballs[i][j-1].typecode=0;  updateflags(rowofballs[i][j-1]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i+1][j]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i+1][j-1]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i-1][j]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i-1][j-1]);
            }
            break;
        case 2:
            if(i%2){//odd i
                rowofballs[i][j].typecode=0;  updateflags(rowofballs[i][j]);
                rowofballs[i][j+1].typecode=0;  updateflags(rowofballs[i][j+1]);
                rowofballs[i][j-1].typecode=0;  updateflags(rowofballs[i][j-1]);
                rowofballs[i][j+2].typecode=0;  updateflags(rowofballs[i][j+2]);
                rowofballs[i][j-2].typecode=0;  updateflags(rowofballs[i][j-2]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i+1][j-1]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i+1][j]);
                rowofballs[i+1][j+1].typecode=0;  updateflags(rowofballs[i+1][j+1]);
                rowofballs[i+1][j+2].typecode=0;  updateflags(rowofballs[i+1][j+2]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i-1][j-1]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i-1][j]);
                rowofballs[i+1][j+1].typecode=0;  updateflags(rowofballs[i-1][j+1]);
                rowofballs[i+1][j+2].typecode=0;  updateflags(rowofballs[i-1][j+2]);
                rowofballs[i+2][j-1].typecode=0;  updateflags(rowofballs[i+2][j-1]);
                rowofballs[i+2][j].typecode=0;  updateflags(rowofballs[i+2][j]);
                rowofballs[i+2][j+1].typecode=0;  updateflags(rowofballs[i+2][j+1]);
                rowofballs[i+2][j-1].typecode=0;  updateflags(rowofballs[i-2][j-1]);
                rowofballs[i+2][j].typecode=0;  updateflags(rowofballs[i-2][j]);
                rowofballs[i+2][j+1].typecode=0;  updateflags(rowofballs[i-2][j+1]);

            }
            else{//even i
                rowofballs[i][j].typecode=0;  updateflags(rowofballs[i][j]);
                rowofballs[i][j+1].typecode=0;  updateflags(rowofballs[i][j+1]);
                rowofballs[i][j-1].typecode=0;  updateflags(rowofballs[i][j-1]);
                rowofballs[i][j+2].typecode=0;  updateflags(rowofballs[i][j+2]);
                rowofballs[i][j-2].typecode=0;  updateflags(rowofballs[i][j-2]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i+1][j-2]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i+1][j-1]);
                rowofballs[i+1][j+1].typecode=0;  updateflags(rowofballs[i+1][j]);
                rowofballs[i+1][j+2].typecode=0;  updateflags(rowofballs[i+1][j+1]);
                rowofballs[i+1][j-1].typecode=0;  updateflags(rowofballs[i-1][j-2]);
                rowofballs[i+1][j].typecode=0;  updateflags(rowofballs[i-1][j-1]);
                rowofballs[i+1][j+1].typecode=0;  updateflags(rowofballs[i-1][j]);
                rowofballs[i+1][j+2].typecode=0;  updateflags(rowofballs[i-1][j+1]);
                rowofballs[i+2][j-1].typecode=0;  updateflags(rowofballs[i+2][j-1]);
                rowofballs[i+2][j].typecode=0;  updateflags(rowofballs[i+2][j]);
                rowofballs[i+2][j+1].typecode=0;  updateflags(rowofballs[i+2][j+1]);
                rowofballs[i+2][j-1].typecode=0;  updateflags(rowofballs[i-2][j-1]);
                rowofballs[i+2][j].typecode=0;  updateflags(rowofballs[i-2][j]);
                rowofballs[i+2][j+1].typecode=0;  updateflags(rowofballs[i-2][j+1]);
            }
            break;
        case 3:
            if(i%2){//odd i
                for(int k=-3;k<4;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i][j+k].isempty){
                            if((rowofballs[i][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i][j+k]);
                        }
                    }
                }
                for(int k=-2;k<4;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+1][j+k].isempty){
                            if((rowofballs[i+1][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+1][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+1][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+1][j+k]);
                        }
                    }
                }
                for(int k=-2;k<4;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-1][j+k].isempty){
                            if((rowofballs[i-1][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-1][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-1][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-1][j+k]);
                        }
                    }
                }
                for(int k=-2;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+2][j+k].isempty){
                            if((rowofballs[i+2][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+2][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+2][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+2][j+k]);
                        }
                    }
                }
                for(int k=-2;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-2][j+k].isempty){
                            if((rowofballs[i-2][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-2][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-2][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-2][j+k]);
                        }
                    }
                }
                for(int k=-1;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+3][j+k].isempty){
                            if((rowofballs[i+3][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+3][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+3][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+3][j+k]);
                        }
                    }
                }
                for(int k=-1;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-3][j+k].isempty){
                            if((rowofballs[i-3][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-3][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-3][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-3][j+k]);
                        }
                    }
                }
            }
            else{//even i
                for(int k=-3;k<4;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i][j+k].isempty){
                            if((rowofballs[i][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i][j+k]);
                        }
                    }
                }
                for(int k=-3;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+1][j+k].isempty){
                            if((rowofballs[i+1][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+1][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+1][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+1][j+k]);
                        }
                    }
                }
                for(int k=-3;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-1][j+k].isempty){
                            if((rowofballs[i-1][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-1][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-1][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-1][j+k]);
                        }
                    }
                }
                for(int k=-2;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+2][j+k].isempty){
                            if((rowofballs[i+2][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+2][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+2][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+2][j+k]);
                        }
                    }
                }
                for(int k=-2;k<3;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-2][j+k].isempty){
                            if((rowofballs[i-2][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-2][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-2][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-2][j+k]);
                        }
                    }
                }
                for(int k=-2;k<2;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i+3][j+k].isempty){
                            if((rowofballs[i+3][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i+3][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i+3][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i+3][j+k]);
                        }
                    }
                }
                for(int k=-2;k<2;k++){
                    if((j+k>-1)&&(j+k<Maxballseachrow+1)){
                        if(!rowofballs[i-3][j+k].isempty){
                            if((rowofballs[i-3][j+k].sloweffect)&&(!isStopped)){
                                isSlow=true;
                                playsound(14);
                                ballSpeed=Balldecentspeed/2;
                                now=clock();
                            }
                            else if(rowofballs[i-3][j+k].stopeffect){
                                isStopped=true;
                                playsound(15);
                                ballSpeed=0;
                                now=clock();
                            }
                            rowofballs[i-3][j+k].typecode=0;  destroyedballs++;   updateflags(rowofballs[i-3][j+k]);
                        }
                    }
                }

            }
            break;
    }
}

int calculatescore(int playModeCode){
    return (destroyedballs*20-powerballsused*100);
}

void checkvictoryorloss2(int playModeCode , MNU_Menu& win,MNU_Menu& loss){
    updaterowclear();
    srand(time(NULL));
    bool allclear=true;
    switch(playModeCode){
        case 0://normal mode: victory possible
            for(int i=0;i<50;i++){
                if((!rowclear[i])&&(rowofballs[i][0].y>590+Ballradius)){
                    loss.buttonsAreActive=true;
                    playsound(5+(rand()%3));
                }
            }
            for (int i=0;i<50;i++){
                for(int j=0;j<Maxballseachrow;j++){
                    if(!rowofballs[i][j].isempty){
                        allclear=false;
                    }
                }
            }
            if(allclear){
                win.buttonsAreActive=true;
                playsound(8+(rand()%3));
            }
            break;
        case 1: //timedmode : victory possible

            for (int i=0;i<50;i++){
                for(int j=0;j<Maxballseachrow;j++){
                    if(!rowofballs[i][j].isempty){
                        allclear=false;
                    }
                }
            }
            if(allclear){
                win.buttonsAreActive=true;
                playsound(8+(rand()%3));
            }
            if((elapsedtime/500)>timeintimedmode){
                loss.buttonsAreActive=true;
                playsound(5+(rand()%3));
            }
            else{
                for(int i=0;i<50;i++){
                    if((!rowclear[i])&&(rowofballs[i][0].y>590+Ballradius)){
                        loss.buttonsAreActive=true;
                        playsound(5+(rand()%3));
                    }
                }
            }
            break;
        case 2: //infinitemode : victory not possible
            for(int i=0;i<50;i++){
                if((!rowclear[i])&&(rowofballs[i][0].y>590+Ballradius)){
                    loss.buttonsAreActive=true;
                    playsound(5+(rand()%3));
                }
            }
            break;
    }

}

void playsound(int SFXcode){
    Mix_PlayChannel(-1,SFXs[SFXcode], 0);
}
void loadAllanimations(SDL_Renderer *renderer ,vector <vector<SDL_Texture *>> &animations){
    vector<SDL_Texture *> temptexture;
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/explosion/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/1/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/2/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/3/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/4/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/5/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/6/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/12/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/13/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/14/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/15/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/16/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/23/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/24/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/25/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/26/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/34/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/35/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/36/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/45/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/46/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/56/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/stone/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/90/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-1/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-2/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-3/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-4/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-5/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/2.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/3.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/4.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/5.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/6.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/7.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/-6/8.png"));
    animations.push_back(temptexture);
    temptexture.clear();
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/UI1.png"));
    temptexture.push_back(IMG_LoadTexture(renderer,"frames/UI2.png"));
    animations.push_back(temptexture);
    temptexture.clear();
}
//*************************************************************************************************
//****************************************************************************************************