#include "iGraphics.h"
#include <cmath>
#include <iostream>
#include "iSound.h"
using namespace std;
enum
{
    IDLE,
    FORWARD,
    BACKWARD,
    FALLDOWN,
    JUMP,
    KICK

};



void gravityTick(void);                       /* physics timer */
void move_ball(void);                         /* ball movement timer */
void new_game(void);                          /* draws gameplay screen */
void populate_player1_images(void);           /* load P1 sprite filenames */
void populate_player2_images(void);           /* load P2 sprite filenames */
void loadresources(void);                     /* preload menu image */
void draw_settings(void);                              /* (reserved, currently empty) */
void iDraw(void);                             /* iGraphics main draw CB */
void iMouseMove(int mx, int my);              /* cursor movement */
void iMouseDrag(int mx, int my);              /* drag with button held */
void iMouse(int button, int state, int mx, int my); /* mouse click */
void iMouseWheel(int dir, int mx, int my);    /* scroll wheel */
void iKeyboard(unsigned char key);            /* ASCII key presses */
void iSpecialKeyboard(unsigned char key);     /* arrow / F-keys */
void update_player1(void);                    /* P1 animation timer */
void update_player2(void);                    /* P2 animation timer */
void update_player(void);
void goalTick(void);

/* For menu button */
#define PLAY_X   602
#define PLAY_Y   373
#define MODES_X  602
#define MODES_Y  294
#define HIST_X   602
#define HIST_Y   217
#define SET_X    602
#define SET_Y    138
#define QUIT_X   0
#define QUIT_Y   538
#define BTN_W    350
#define BTN_H     60
#define QUIT_W   214
#define QUIT_H   45
int m = 0, n = 0, x = 0, y = 0, z = 0;
int k = 0;
/*----------------------------*/


/* File save */
#define MAX_NAME_LEN 30
#define MAX_HISTORY_LINES 10
#define MAX_LINE_LEN 100

char player1Name[MAX_NAME_LEN + 1] = "";
char player2Name[MAX_NAME_LEN + 1] = "";
char currentNameInput[MAX_NAME_LEN + 1] = "";
int enteringName1 = 1; // 1 = player1 typing, 0 = player2

char historyEntries[MAX_HISTORY_LINES][MAX_LINE_LEN];
int historyCount = 0;
/*-----------------------------*/


/*<--------------------------Setting menu--------------------------->*/
Image settings_bg;
/* button hit-box constants (match the artwork coordinates) */
#define BG_MUSIC_PLUS_BUTTON_X  600   /* + for BG music */
#define BG_MUSIC_PLUS_BUTTON_Y  370
#define BG_MUSIC_MINUS_BUTTON_X 678   /* – for BG music */
#define BG_MUSIC_MINUS_BUTTON_Y 370

#define SOUND_MUSIC_PLUS_BUTTON_X  600   /* + for SFX */
#define SOUND_MUSIC_PLUS_BUTTON_Y  290
#define SOUND_MUSIC_MINUS_BUTTON_X 678   /* – for SFX */
#define SOUND_MUSIC_MINUS_BUTTON_Y 295

#define BTN_BACK_X   290   /* Back to menu rectangle */
#define BTN_BACK_Y   192
#define BTN_BACK_W   355
#define BTN_BACK_H    60
/*----------------------------------------------------------------------*/

/*sound*/
int bgSoundIdx    = -1;      /* background music (loop)  */
int cheerSoundIdx = -1;      /* one-shot goal cheer      */
int kickSoundIdx = -1;
const char* BG_MUSIC_FILE  = "assets/sounds/bgm.WAV";
const char* CHEER_FILE     = "assets/sounds/Goal.WAV";
const char* kick = "assets/sounds/kick.WAV";
/*------------*/

/*<---------------------Loading-screen--------------------------->*/
#define LOADING_TIME_MS 600
#define BALL_JUMP_MS 200
#define LOADING_RADIUS 15
#define LOADING_Y 90
const char* LOADING_BG  = "assets/images/Loading_Screen_1.png";
const char* BALL_IMG = "assets/images/Ball 01.png";
int  loadingTimer = 0;
int  currentCircle = 0;
bool loadingDone = false;
bool loadingStarted = false;
int b=0;
/*----------------------------------------------------------------*/

/*<----------------------Goal animation constants------------------------->*/
#define GOAL_FRAMES 21
#define GOAL_FRAME_MS 100
#define GOAL_CX 500
#define GOAL_CY 300

const int GOAL_SIZE[GOAL_FRAMES] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 150, 150, 150, 150, 150, 150};
const char* GOAL_FILES[GOAL_FRAMES] = {
    "assets/images/Goal/Goal_001.png",
    "assets/images/Goal/Goal_002.png",
    "assets/images/Goal/Goal_003.png",
    "assets/images/Goal/Goal_004.png",
    "assets/images/Goal/Goal_005.png",
    "assets/images/Goal/Goal_006.png",
    "assets/images/Goal/Goal_007.png",
    "assets/images/Goal/Goal_008.png",
    "assets/images/Goal/Goal_009.png",
    "assets/images/Goal/Goal_010.png",
    "assets/images/Goal/Goal_011.png",
    "assets/images/Goal/Goal_012.png",
    "assets/images/Goal/Goal_013.png",
    "assets/images/Goal/Goal_014.png",
    "assets/images/Goal/Goal_015.png",
    "assets/images/Goal/Goal_016.png",
    "assets/images/Goal/Goal_017.png",
    "assets/images/Goal/Goal_018.png",
    "assets/images/Goal/Goal_019.png",
    "assets/images/Goal/Goal_020.png",
    "assets/images/Goal/Goal_021.png"
};
Image goalImg[GOAL_FRAMES];
int goalFrame = 0;
bool goalAnimPlaying = false;
/*-----------------------------------------------------*/

/*Player 1 image variable*/
int pic1_x, pic1_y;
int idle_idx = 0;
int jump_idx = 0;
int forward_idx = 0;
int backward_idx = 0;
int falldown_idx = 0;
int state = IDLE;
char player1_idle[18][106];
char player1_jump[5][106];
char player1_falldown[5][122];
char player1_forward[10][122];
char player1_backward[10][124];
char player1_kick[9][106];
char *player1_image;
int kick_idx = 0;
/*----------------------------*/


/*Player 2 image variable*/
int pic2_x,pic2_y;
int idle_idx_2 = 0;
int jump_idx_2 = 0;
int forward_idx_2 = 0;
int backward_idx_2 = 0;
int falldown_idx_2 = 0;
int state_2 = IDLE;
char player2_idle[18][106];
char player2_jump[5][106];
char player2_falldown[5][122];
char player2_forward[10][122];
char player2_backward[10][124];
char player2_kick[9][106];
char *player2_image;
int kick_idx_2 = 0;
/*----------------------------*/


/*ball image variable*/
int pic_ballx,pic_bally;
int ball_speed = 0;
bool ball_moving = false;
int ball_dir = 1;
int ball_width = 50;
int player1_width = 103;
int player2_width = 103;
int player1_score = 0;
int player2_score  = 0;
int bvx = 0, bvy = 0;     /* ball-velocity X/Y */
const int BALL_GRAV = -2; /* change of Y*/
const int GROUND_Y  = 85; /* initial y position of ball */
const float RESTITUTION = 0.60f;   // Energy loss after every bounce
const float FRICTION    = 0.97f;   // Friction
/*----------------------------*/

/*Motion for player 1*/
int vy1 = 0;          /* vertical speed in air*/
int vx1 = 0;          /* horizontal speed in air   */
/*Motion for player 2*/
int vy2 = 0;          /* vertical speed in air*/
int vx2 = 0;          /* horizontal speed in air*/

inline int pLeft (int px){ return px + 53; }
inline int pRight(int px){ return px + 103; }
void resolvePlayerCollision()
{
    if( pRight(pic1_x)  > pLeft(pic2_x) && pRight(pic2_x)  > pLeft(pic1_x) )  
    {
        int overlap1 = pRight(pic1_x) - pLeft(pic2_x);
        int overlap2 = pRight(pic2_x) - pLeft(pic1_x);

        if(pic1_x < pic2_x){               
            pic1_x -= overlap1;            
            pic2_x += overlap1;            
        }else{                             
            pic1_x += overlap2;
            pic2_x -= overlap2;
        }
        vx1 = vx2 = 0;
    }
}

void gravityTick()
{
  
    if (pic1_y > 62 || vy1 > 0)
    {
        vy1 -= 2;
        pic1_y += vy1;

        if (pic1_y <= 62)  
        {
            pic1_y = 62;
            vy1 = vx1 = 0;
            state = IDLE;
        }
    }
    pic1_x += vx1;

    
    if (pic2_y > 62 || vy2 > 0)
    {
        vy2 -= 2;
        pic2_y += vy2;

        if (pic2_y <= 62)
        {
            pic2_y = 62;
            vy2 = vx2 = 0;
            state_2 = IDLE;
        }
    }
    pic2_x += vx2;
     resolvePlayerCollision(); 
}

bool hit(int px, int py, int pw, int ph,
         int bx, int by, int bw, int bh)
{
    return !( px+pw < bx || bx+bw < px+53 ||
              py+ph < by || by+bh < py+27 );
}

void resetField()
{   
    goalAnimPlaying = true;
    goalFrame = 0; 
    cheerSoundIdx = iPlaySound(CHEER_FILE, false);
    // Center the ball
    bvx = bvy = 0;
    pic_ballx = 475;
    pic_bally = GROUND_Y;

    // Reset ball motion
    ball_speed = 0;
    ball_dir = 0; // Direction = 0 means stationary
    ball_moving = false;

    // Reset player 1
    pic1_x = 100;
    pic1_y = 62;
    vx1 = vy1 = 0;
    state = IDLE;

    // Reset player 2
    pic2_x = 745;
    pic2_y = 62;
    vx2 = vy2 = 0;
    state_2 = IDLE;

}

void move_ball()
{
    if(bvx==0 && bvy==0) return;
    /* New position of ball */
    pic_ballx += bvx;
    pic_bally += bvy;
    /* Adding gravity */
    bvy += BALL_GRAV;
    /* Check if the ball touch the ground or not? */
    if(pic_bally <= GROUND_Y){
        pic_bally = GROUND_Y;
        /*  Bounce  */
        bvy = (-bvy) * RESTITUTION;   // Direction change, Energy loss.
        bvx *= FRICTION;              // Ball velocity in X in decreasing due to friction.
        /*  if the velocity is in X and Y axix is very very small then stop the ball  */
        if(fabs(bvy) < 2)  bvy = 0;
        if(fabs(bvx) < 1)  bvx = 0;
    }
    /* Check goalpost */
    if(pic_ballx <= 119){
        ++player2_score;  resetField();  return;
    }
    if(pic_ballx + ball_width >= 873){
        ++player1_score;  resetField();  return;
    }
}
int count_timer=0;
int second,minute;
char timeText[20];

void Watch(){
    if(k==1){count_timer++;
    
    second = count_timer % 60;
    minute = count_timer / 60;}

    
    sprintf(timeText, "%02d:%02d", minute, second); // 2-digit format

    
}

void new_game(){
    iShowImage(0, 40, "assets/images/bg.png");
   iShowImage(pic1_x, pic1_y, player1_image);
   iShowImage(pic2_x, pic2_y, player2_image);
   iShowImage(pic_ballx, pic_bally, "assets/images/Ball 02.png");
    int ex1 = (state==KICK ? 15 : 0);      // extra reach P1
    int ex2 = (state_2==KICK ? 15 : 0);    // extra reach P2

   if( hit(pic1_x,pic1_y,player1_width+ex1,123,
        pic_ballx,pic_bally,ball_width,ball_width) )
{
    bool strong = (state == KICK);
    bvx = (pic_ballx >= pic1_x + player1_width/2) ?  25 : -25; // 25 px for normal pass
    bvy = strong ? 25 : 0;                      // Projectile motion for only kick
    iPlaySound(kick,false);
}

if( hit(pic2_x-ex2,pic2_y,player2_width,123,
        pic_ballx,pic_bally,ball_width,ball_width) )
{
    bool strong = (state_2 == KICK);
    bvx = (pic_ballx >= pic2_x + player2_width/2) ?  20 : -20;
    bvy = strong ? 25 : 0;
    iPlaySound(kick,false);
}

ball_moving = (ball_speed>0);


}
void update_player(){
    update_player1();
    update_player2();
    goalTick();
}

void populate_player1_images()
{
    for (int i = 0; i < 18; i++)
    {
        sprintf(player1_idle[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Idle/Idle_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player1_jump[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Jump/Jump_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player1_falldown[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Falling Down/Falling Down_%03d.png", i);
    }   
    for (int i = 0; i < 5; i++)
    {
        sprintf(player1_forward[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Move Forward/Move Forward_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player1_kick[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Kick/Kick_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player1_backward[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Move Backward/Move Backward_%03d.png", i);
    }

    player1_image = player1_idle[0];
}

void populate_player2_images(void)
{
    for (int i = 0; i < 18; i++)
    {
        sprintf(player2_idle[i], "assets/Characters/Character 02 - England/PNG Sequences/Idle/Idle_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player2_jump[i], "assets/Characters/Character 02 - England/PNG Sequences/Jump/Jump_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player2_falldown[i], "assets/Characters/Character 02 - England/PNG Sequences/Falling Down/Falling Down_%03d.png", i);
    }   
    for (int i = 0; i < 5; i++)
    {
        sprintf(player2_forward[i], "assets/Characters/Character 02 - England/PNG Sequences/Move Forward/Move Forward_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player2_kick[i], "assets/Characters/Character 02 - England/PNG Sequences/Kick/Kick_%03d.png", i);
    }
    for (int i = 0; i < 5; i++)
    {
        sprintf(player2_backward[i], "assets/Characters/Character 02 - England/PNG Sequences/Move Backward/Move Backward_%03d.png", i);
    }

    player2_image = player2_idle[0];
}


Image bg;
/*
function iDraw() is called again and again by the system.
*/
void loadresources()
{
    iLoadImage(&bg, "assets/images/Menu.png");
        for (int i = 0; i < GOAL_FRAMES; i++) {
        iLoadImage(&goalImg[i], GOAL_FILES[i]);
    }
   iLoadImage(&settings_bg, "assets/images/setting.JPG");


}
void draw_settings()
{
    /* background stadium blur remains visible underneath */
    iShowImage(0, 40, "assets/images/setting.JPG");

    iSetColor(255,255,255);
    iRectangle(290,192,355,60);
}

void blink(){
    b=!b;
}

void drawLoadingScreen()
{
    iShowImage(0, 40, LOADING_BG);
    if(!loadingStarted){
        iSetColor(255,255,255);
        if(b==0){iText(430, 70, "Click to continue", GLUT_BITMAP_9_BY_15);
        iText(431, 71, "Click to continue", GLUT_BITMAP_9_BY_15);
}        return;
    }
    int cx[3] = { 440, 500, 560 };
    iSetColor(255,255,255);
    for(int i=0;i<3;i++)
        iFilledCircle(cx[i], LOADING_Y, LOADING_RADIUS);
    iShowImage(cx[currentCircle]-LOADING_RADIUS,
               LOADING_Y-LOADING_RADIUS,
               BALL_IMG);
}

/*<--Loading Screen Timer-->*/
void loadingTick()
{
    if(!loadingStarted || loadingDone) return;
    loadingTimer += 20;
    if( loadingTimer % BALL_JUMP_MS == 0 ){
        currentCircle = (currentCircle+1)%3;
    }
    if( loadingTimer >= LOADING_TIME_MS ){
        loadingDone = true;
        k = 0;
        iIncreaseVolume(bgSoundIdx, 100);
    }
}
/*--------------------------*/

void drawGoalOverlay()
{
    if (!goalAnimPlaying) return;
    int w = GOAL_SIZE[goalFrame];
    int h = w;
    int x = GOAL_CX - w / 2;
    int y = GOAL_CY - h / 2;
    iShowImage(x, y, GOAL_FILES[goalFrame]);
}

void goalTick()
{
    if (!goalAnimPlaying) return;

    goalFrame++;
    if (goalFrame >= GOAL_FRAMES)
    {
        goalAnimPlaying = false;
        goalFrame = 0;
    }
}

void iDraw()
{
    // place your drawing codes here
    iClear();
    if(!loadingDone){
        drawLoadingScreen();
        return;
    }
    if(k==3){
        draw_settings();
        return;
    }
    if(k==4){
        iShowImage(0,40,"assets/images/Mode.png");
        iRectangle(227,150,428,67);
        return;
    }
    if(k==0){
        iShowImage(0,40,"assets/images/Menu.png");
        /* Word in normal font */
        iSetColor(255,255,255);
        if(!m)iText(PLAY_X+145,  PLAY_Y+25,  "PLAY",   GLUT_BITMAP_HELVETICA_18);
        if(!n)iText(MODES_X+140, MODES_Y+25, "MODES",   GLUT_BITMAP_HELVETICA_18);
        if(!x)iText(HIST_X+130,  HIST_Y+25,  "HISTORY", GLUT_BITMAP_HELVETICA_18);
        if(!y)iText(SET_X+130,   SET_Y+25,   "SETTING", GLUT_BITMAP_HELVETICA_18);
        if(!z)iText(QUIT_X+80,  QUIT_Y+18,  "QUIT",    GLUT_BITMAP_HELVETICA_18);

        /* Word in bold form */
        iSetColor(255,255,0);    /* Bright Yellow */
        if(m){                  /* PLAY */
            iText(PLAY_X+140, PLAY_Y+22, "PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(PLAY_X+141, PLAY_Y+23, "PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(n){                  /* MODES */
            iText(MODES_X+133, MODES_Y+22, "MODES", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(MODES_X+134, MODES_Y+23, "MODES", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(x){                  /* HISTORY */
            iText(HIST_X+125, HIST_Y+22, "HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(HIST_X+126, HIST_Y+23, "HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(y){                  /* SETTING */
            iText(SET_X+125, SET_Y+22, "SETTING", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(SET_X+126, SET_Y+23, "SETTING", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(z){                  /* QUIT */
            iText(QUIT_X+75, QUIT_Y+15, "QUIT", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(QUIT_X+76, QUIT_Y+16, "QUIT", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        return;
    }
    iShowImage(0, 40, "assets/images/Menu.png");
    iSetColor(255, 255, 255);

if (k == 6)
{
    iSetColor(30, 30, 30);
    iFilledRectangle(0, 0, 1000, 600);
    iSetColor(255, 255, 255);
    if (enteringName1)
        iText(100, 500, "Enter Player 1 Name:", GLUT_BITMAP_HELVETICA_18);
    else
        iText(100, 500, "Enter Player 2 Name:", GLUT_BITMAP_HELVETICA_18);

    iText(100, 460, currentNameInput, GLUT_BITMAP_HELVETICA_18);
    return;
}

if(k==1){
         new_game();
        }
    
    char p1_score[20];
    char p2_score[20];
    sprintf(p1_score, " %d ", player1_score);
    sprintf(p2_score, " %d ", player2_score);
    iSetColor(250,250,250);
    iFilledRectangle(32,513,30,25);
    iSetColor(0,0,0);
    iFilledRectangle(62,513,30,25);
    iSetColor(0,0,0);
    iText(35, 520, p1_score);  // Draw at top-left corner (adjust coords as needed)
    iSetColor(250,250,250);
    iText(65, 520, p2_score);
    iSetColor(255, 255, 255);  // White color (change if needed)
    iText(460, 560, timeText, GLUT_BITMAP_HELVETICA_18); // Top-center-ish position
    drawGoalOverlay();
    //iSetColor(237,106,94);
    //iFilledRectangle(238,513,444,565);
    //iText(240, 515, score_str);
}


   // iText(140, 180, "Head soccer");


/*
function iMouseMove() is called when the user moves the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseMove(int mx, int my)
{
    // place your codes here
    if(k) return;          /* Only MENU*/

    m = (mx>=PLAY_X  && mx<=PLAY_X+BTN_W  && my>=PLAY_Y  && my<=PLAY_Y+BTN_H);
    n = (mx>=MODES_X && mx<=MODES_X+BTN_W && my>=MODES_Y && my<=MODES_Y+BTN_H);
    x = (mx>=HIST_X  && mx<=HIST_X+BTN_W  && my>=HIST_Y && my<=HIST_Y+BTN_H);
    y = (mx>=SET_X   && mx<=SET_X+BTN_W   && my>=SET_Y  && my<=SET_Y+BTN_H);
    z = (mx>=QUIT_X  && mx<=QUIT_X+QUIT_W  && my>=QUIT_Y && my<=QUIT_Y+QUIT_H);
}

/*
function iMouseDrag() is called when the user presses and drags the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouseDrag(int mx, int my)
{
    // place your codes here
}


/*
function iMouse() is called when the user presses/releases the mouse.
(mx, my) is the position where the mouse pointer is.
*/
void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && k==0)
    {
        // place your codes here
        if(mx>=PLAY_X && mx<=PLAY_X+BTN_W && my>=PLAY_Y && my<=PLAY_Y+BTN_H){
            k = 6;
            //bijoy add
            currentNameInput[0] = '\0';
            enteringName1 = 1;
            //

            pic1_x = 100; pic1_y = 62;
            pic2_x = 745; pic2_y = 62;
            vx1 = vy1 = vx2 = vy2 = 0;
            state = state_2 = IDLE;
            ball_moving = false;
            pic_ballx = 475; pic_bally = 85;
            player2_score  = 0;
            player1_score  = 0;
            iDecreaseVolume(bgSoundIdx, 80);


            
            return;                /* Start play game */
        }
        if(mx>=MODES_X && mx<=MODES_X+BTN_W && my>=MODES_Y && my<=MODES_Y+BTN_H){
            k=4;
            return;
        }
        if(mx>=HIST_X && mx<=HIST_X+BTN_W && my>=HIST_Y && my<=HIST_Y+BTN_H){
            return;
        }
        if(mx>=SET_X && mx<=SET_X+BTN_W && my>=SET_Y && my<=SET_Y+BTN_H){
           k=3;
           return;
        }
        if(mx>=QUIT_X && mx<=QUIT_X+BTN_W && my>=QUIT_Y && my<=QUIT_Y+BTN_H){
            exit(0);
        }
    }
    if(k==3){
      if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){

        /* --- BG music + --- */
        if(mx>=BG_MUSIC_PLUS_BUTTON_X && mx<=BG_MUSIC_PLUS_BUTTON_X+60 &&
           my>=BG_MUSIC_PLUS_BUTTON_Y && my<=BG_MUSIC_PLUS_BUTTON_Y+45){
            iIncreaseVolume(bgSoundIdx, 10);
        }
        /* --- BG music - --- */
        if(mx>=BG_MUSIC_MINUS_BUTTON_X && mx<=BG_MUSIC_MINUS_BUTTON_X+60 &&
           my>=BG_MUSIC_MINUS_BUTTON_Y && my<=BG_MUSIC_MINUS_BUTTON_Y+45){
            iDecreaseVolume(bgSoundIdx, 10);
        }

        /* --- SFX + --- */
        if(mx>=SOUND_MUSIC_PLUS_BUTTON_X && mx<=SOUND_MUSIC_PLUS_BUTTON_X+60 &&
           my>=SOUND_MUSIC_PLUS_BUTTON_Y && my<=SOUND_MUSIC_PLUS_BUTTON_Y+45){
                iIncreaseVolume(cheerSoundIdx, 10);
                iIncreaseVolume(kickSoundIdx, 10);
        }
        /* --- SFX - --- */
        if(mx>=SOUND_MUSIC_MINUS_BUTTON_X && mx<=SOUND_MUSIC_MINUS_BUTTON_X+60 &&
           my>=SOUND_MUSIC_MINUS_BUTTON_Y && my<=SOUND_MUSIC_MINUS_BUTTON_Y+45){
                iDecreaseVolume(cheerSoundIdx, 10);
                iDecreaseVolume(kickSoundIdx, 10);
        }

        /* --- Back button --- */
        if(mx>=BTN_BACK_X && mx<=BTN_BACK_X+BTN_BACK_W &&
           my>=BTN_BACK_Y && my<=BTN_BACK_Y+BTN_BACK_H){
            k = 0;      /* leave settings → back to main menu */
        }
    }
    return;   /* don’t let other menu/game clicks run while in settings */
}

    if(k == -1 && !loadingStarted && button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        loadingStarted = true;
        loadingTimer   = 0;
        currentCircle  = 0;
        return;
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    {
        // place your codes here
    }
}

/*
function iMouseWheel() is called when the user scrolls the mouse wheel.
dir = 1 for up, -1 for down.
*/
void iMouseWheel(int dir, int mx, int my)
{
    // place your code here
}
void update_player1()
{
    switch (state)
    {
    case IDLE:
        player1_image = player1_idle[idle_idx];
        idle_idx = (idle_idx + 1) % 18;

        break;
    case FORWARD:
        player1_image = player1_forward[forward_idx];
        forward_idx = (forward_idx + 1) % 5;
        if (forward_idx == 0)
        {
            state = IDLE;
        }
        break;
    case JUMP:
        player1_image = player1_jump[jump_idx];
        jump_idx ++;
        if (jump_idx == 4)
    {
        vy1           = 0;         
        falldown_idx  = 0;         
        state         = FALLDOWN;  
        break;                     
    }

    if (jump_idx == 5) jump_idx = 0;
        break;
    case BACKWARD:
        player1_image = player1_backward[backward_idx];
        backward_idx = (backward_idx + 1) % 5;
        if (backward_idx == 0)
        {
            state = IDLE;
        }
        break;
    case FALLDOWN:
        player1_image = player1_falldown[falldown_idx];
        falldown_idx = (falldown_idx + 1) % 5;
        if (falldown_idx == 0 && pic1_y == 62)
        {
            state = IDLE;
        }
        break;
    case KICK:
    player1_image = player1_kick[kick_idx];
    kick_idx = (kick_idx + 1) % 5;
    if(kick_idx == 0)
        state = IDLE;
    break;
        
    }
}


void update_player2()
{
    switch (state_2)
    {
    case IDLE:
        player2_image = player2_idle[idle_idx_2];
        idle_idx_2 = (idle_idx_2 + 1) % 18;

        break;
    case FORWARD:
        player2_image = player2_forward[forward_idx_2];
        forward_idx_2 = (forward_idx_2 + 1) % 5;
        if (forward_idx_2 == 0)
        {
            state_2 = IDLE;
        }
        break;
    case JUMP:
        player2_image = player2_jump[jump_idx_2];
        jump_idx_2++;
    if (jump_idx_2 == 4)
    {
        vy2 = 0;
        falldown_idx_2 = 0;
        state_2 = FALLDOWN;
        break;
    }
    if (jump_idx_2 == 5) jump_idx_2 = 0;
        break;
    case BACKWARD:
        player2_image = player2_backward[backward_idx_2];
        backward_idx_2 = (backward_idx_2 + 1) % 5;
        if (backward_idx_2 == 0)
        {
            state_2 = IDLE;
        }
        break;
    case FALLDOWN:
        player2_image = player2_falldown[falldown_idx];
        falldown_idx_2 = (falldown_idx_2 + 1) % 5;
        if (falldown_idx_2 == 0 && pic2_y == 62)
        {
            state_2 = IDLE;
        }
        break;
    case KICK:
        player2_image = player2_kick[kick_idx_2];
        kick_idx_2 = (kick_idx_2 + 1) % 5;
        if(kick_idx_2 == 0)
            state_2 = IDLE;
        break;
        
    }
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/


void iKeyboard(unsigned char key)
{
    if (k == 6) // Name input mode
    {
        if (key == '\r') // Enter key
        {
            if (enteringName1)
            {
                strcpy(player1Name, currentNameInput);
                currentNameInput[0] = '\0';
                enteringName1 = 0;
            }
            else
            {
                strcpy(player2Name, currentNameInput);
                currentNameInput[0] = '\0';
                enteringName1 = 1;
                k = 1; // Now start the game
            }
        }
        else if (key == '\b') // Backspace
        {
            int len = strlen(currentNameInput);
            if (len > 0)
                currentNameInput[len - 1] = '\0';
        }
        else if (strlen(currentNameInput) < MAX_NAME_LEN - 1)
        {
            int len = strlen(currentNameInput);
            currentNameInput[len] = key;
            currentNameInput[len + 1] = '\0';
        }

        return; // prevent game control when entering name
    }

    // Your usual game controls below (only when k == 1)
    if (k == 1)
    {
        if (key == 'a')
        {
            // Move player left
        }
        else if (key == 'd')
        {
            // Move player right
        }
        // add other controls here...
    }
}


/*
function iSpecialKeyboard() is called whenver user hits special keys likefunction
keys, home, end, pg up, pg down, arraows etc. you have to use
appropriate constants to detect them. A list is:
GLUT_KEY_F1, GLUT_KEY_F2, GLUT_KEY_F3, GLUT_KEY_F4, GLUT_KEY_F5, GLUT_KEY_F6,
GLUT_KEY_F7, GLUT_KEY_F8, GLUT_KEY_F9, GLUT_KEY_F10, GLUT_KEY_F11,
GLUT_KEY_F12, GLUT_KEY_LEFT, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_DOWN,
GLUT_KEY_PAGE_UP, GLUT_KEY_PAGE_DOWN, GLUT_KEY_HOME, GLUT_KEY_END,
GLUT_KEY_INSERT */

void iSpecialKeyboard(unsigned char key)
{

    if (key == GLUT_KEY_END)
    {
        exit(0);
    }
    if (k==1)
    {if (key == GLUT_KEY_LEFT)
    {
       if (pic2_y == 62) {        
    pic2_x -= 15;
    state_2 = FORWARD;
    } else {                   
    vx2 = -12;
    } 
    }
    if (key == GLUT_KEY_RIGHT)
    {
       if (pic2_y == 62) {
    pic2_x += 15;
    state_2 = BACKWARD;
    } else {
    vx2 =  12;
    } 
    }
    if (key == GLUT_KEY_UP)
    {
          if (pic2_y == 62) {
    vy2   = 40;            
    state_2 = JUMP;
    }
    }
    if (key == GLUT_KEY_DOWN)
    {
        if (pic2_y > 62)
    vy2 -= 3;
    }}
    // place your codes for other keys here
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // place your own initialization codes here.
    iInitializeSound();                               /* init audio engine */
    bgSoundIdx = iPlaySound(BG_MUSIC_FILE, true);  /* loop */
    if(k==1)iDecreaseVolume(bgSoundIdx, 40);
    if(k!=1)iIncreaseVolume(bgSoundIdx, 40);

    pic1_x = 100;
    pic1_y = 62;
    pic2_x = 745;
    pic2_y = 62;
    pic_ballx = 475;
    pic_bally = 85;
    populate_player1_images();
    iSetTimer(16, move_ball);
    populate_player2_images();
    iSetTimer(60, update_player);
    iSetTimer(30,  gravityTick);
    iSetTimer(20, loadingTick);
    iSetTimer(400, blink);
    iSetTimer(1000,Watch);
    loadingDone = false;
    k = -1;
    iDecreaseVolume(bgSoundIdx, 100);
    iInitialize(1000, 600, "Head Soccer");
    return 0;  
}
