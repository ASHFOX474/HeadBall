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


/* Prototypes for all user functions */
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

/* For menu button */
#define PLAY_X   595
#define PLAY_Y   320
#define MODES_X  595
#define MODES_Y  235
#define HIST_X   595
#define HIST_Y   162
#define SET_X    595
#define SET_Y    90
#define QUIT_X   14
#define QUIT_Y   560
#define BTN_W    325   /* all buttons ≈ same width except Quit */
#define BTN_H     60
#define QUIT_W   135
#define QUIT_H   25
int m = 0, n = 0, x = 0, y = 0, z = 0;   /* one flag per button */
int k = 0;           /* 0 = MENU, 1 = GAME (add more states if needed) */
/*----------------------------*/


/*Setting menu*/
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
/*----------------------------*/

/*sound*/
int bgSoundIdx    = -1;      /* background music (loop)  */
int cheerSoundIdx = -1;      /* one-shot goal cheer      */
int kickSoundIdx = -1;

/* file paths (change if you store elsewhere) */
const char* BG_MUSIC_FILE  = "assets/sounds/bgm.WAV";
const char* CHEER_FILE     = "assets/sounds/Goal.WAV";
const char* kick = "assets/sounds/kick.WAV";




/*------------*/


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
/*----------------------------*/


/*ball image variable*/
int pic_ballx,pic_bally;
int ball_speed = 0;
bool ball_moving = false;
int ball_direction = 1;
int ball_width = 50;
int player1_width = 103;
int player2_width = 103;
int player1_score = 0;
int player2_score  = 0;
/*----------------------------*/

/*Motion for player 1*/
int vy1 = 0;          /* vertical speed in air*/
int vx1 = 0;          /* horizontal speed in air   */
/*Motion for player 2*/
int vy2 = 0;          /* vertical speed in air*/
int vx2 = 0;          /* horizontal speed in air*/


void gravityTick()
{
    /*Player-1 vertical drift*/
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

    /*Player-2 vertical drift*/
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
}


void move_ball()
{
    if (ball_moving)
    {
        pic_ballx += ball_speed * ball_direction;

        // Check if ball reaches left edge
        if (pic_ballx <= 0) {
            pic_ballx = 0;
            ball_moving = false;
        }
        // Check if ball reaches right goal line
        else if (pic_ballx+50 >= 873) {
            player1_score++;
            Sleep(1000);
            cheerSoundIdx = iPlaySound(CHEER_FILE, false);
            // Reset ball position to center
            pic_ballx = 475;  
            pic_bally = 85;   

            // Reset player 1 position
            pic1_x = 100;     
            pic1_y = 62;
            // Reset player 2 position
            pic2_x = 745;
            pic2_y = 62;     

            // TODO: reset player 2 here when added

            ball_moving = false;
            ball_speed = 0;
            state = IDLE;  // reset player 1 state to idle
        }
        else if (pic_ballx <= 119) {
            player2_score++;
            Sleep(3000);
            cheerSoundIdx = iPlaySound(CHEER_FILE, false);
            // Reset ball position to center
            pic_ballx = 475;  
            pic_bally = 85;   

            // Reset player 1 position
            pic1_x = 100;     
            pic1_y = 62;
            // Reset player 2 position
            pic2_x = 745;
            pic2_y = 62;     

            // TODO: reset player 2 here when added

            ball_moving = false;
            ball_speed = 0;
            state = IDLE;  // reset player 1 state to idle
        }

        // Gradually reduce speed
        if (ball_speed > 0)
            ball_speed--;

        // Stop if speed is zero
        if (ball_speed == 0)
        {
            ball_moving = false;
        }
    }
}

void new_game(){
    iShowImage(0, 40, "assets/images/bg.png");
   iShowImage(pic1_x, pic1_y, player1_image);
   iShowImage(pic2_x, pic2_y, player2_image);
   iShowImage(pic_ballx, pic_bally, "assets/images/Ball 02.png");
   
   bool player1_touch = pic1_x + player1_width >= pic_ballx && pic1_y<=pic_bally+ball_width;
   

if (!ball_moving && player1_touch)
{
    ball_moving = true;
    ball_direction = 1;    // Player 1 kicks to the right
    ball_speed = 20;
    kickSoundIdx = iPlaySound(kick, false);
    
}

bool player2_touch =  (pic2_x+46 <= pic_ballx+ball_width) &&
                          (pic2_y          <=     pic_bally+ball_width);

    if(!ball_moving && player2_touch)
    {
        ball_moving   = true;
        ball_direction= -1;
        ball_speed    = 20;
        kickSoundIdx  = iPlaySound(kick, false);
    }

}
void update_player(){
    update_player1();
    update_player2();
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
    for (int i = 0; i < 9; i++)
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
    for (int i = 0; i < 10; i++)
    {
        sprintf(player2_forward[i], "assets/Characters/Character 02 - England/PNG Sequences/Move Forward/Move Forward_%03d.png", i);
    }
    for (int i = 0; i < 9; i++)
    {
        sprintf(player2_kick[i], "assets/Characters/Character 02 - England/PNG Sequences/Kick/Kick_%03d.png", i);
    }
    for (int i = 0; i < 10; i++)
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
    iLoadImage(&bg, "assets/images/Menu.JPG");
   iLoadImage(&settings_bg, "assets/images/setting.JPG");


}
void draw_settings()
{
    /* background stadium blur remains visible underneath */
    iShowImage(0, 40, "assets/images/setting.JPG");

    iSetColor(255,255,255);
    iRectangle(290,192,355,60);
}


void iDraw()
{
    // place your drawing codes here
    iClear();
    if(k==3){
        draw_settings();
        return;
    }
    if(k==0){
        iShowImage(0,40,"assets/images/Menu.JPG");
        /* Word in normal font */
        iSetColor(255,255,255);
        if(!m)iText(PLAY_X+135,  PLAY_Y+25,  "PLAY",   GLUT_BITMAP_HELVETICA_18);
        if(!n)iText(MODES_X+130, MODES_Y+25, "MODES",   GLUT_BITMAP_HELVETICA_18);
        if(!x)iText(HIST_X+120,  HIST_Y+25,  "HISTORY", GLUT_BITMAP_HELVETICA_18);
        if(!y)iText(SET_X+120,   SET_Y+25,   "SETTING", GLUT_BITMAP_HELVETICA_18);
        if(!z)iText(QUIT_X+53,  QUIT_Y+10,  "QUIT",    GLUT_BITMAP_TIMES_ROMAN_10);

        /* Word in bold form */
        iSetColor(255,255,0);    /* Bright Yellow */
        if(m){                  /* PLAY */
            iText(PLAY_X+130, PLAY_Y+22, "PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(PLAY_X+131, PLAY_Y+23, "PLAY", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(n){                  /* MODES */
            iText(MODES_X+123, MODES_Y+22, "MODES", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(MODES_X+124, MODES_Y+23, "MODES", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(x){                  /* HISTORY */
            iText(HIST_X+115, HIST_Y+22, "HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(HIST_X+116, HIST_Y+23, "HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(y){                  /* SETTING */
            iText(SET_X+115, SET_Y+22, "SETTING", GLUT_BITMAP_TIMES_ROMAN_24);
            iText(SET_X+116, SET_Y+23, "SETTING", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        if(z){                  /* QUIT */
            iText(QUIT_X+50, QUIT_Y+8, "QUIT", GLUT_BITMAP_HELVETICA_12);
            iText(QUIT_X+51, QUIT_Y+9, "QUIT", GLUT_BITMAP_HELVETICA_12);
        }
        return;
    }
    iShowImage(0, 40, "assets/images/Menu.JPG");
    iSetColor(255, 255, 255);
    iText(10,10, "Press p for pause, r for resume, END for exit.");
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
            k = 1;
            pic1_x = 100; pic1_y = 62;
            pic2_x = 745; pic2_y = 62;
            vx1 = vy1 = vx2 = vy2 = 0;
            state = state_2 = IDLE;
            ball_moving = false;
            pic_ballx = 475; pic_bally = 85;
            return;                /* Start play game */
        }
        if(mx>=MODES_X && mx<=MODES_X+BTN_W && my>=MODES_Y && my<=MODES_Y+BTN_H){
            /* Start modes */
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
        forward_idx_2 = (forward_idx_2 + 1) % 10;
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
        backward_idx_2 = (backward_idx_2 + 1) % 10;
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
    
        
    }
}

/*
function iKeyboard() is called whenever the user hits a key in keyboard.
key- holds the ASCII value of the key pressed.
*/


void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'm':
     k=0; break;
if (k==1)
{    case 'a':                                  
    if (pic2_y == 62) {        
    pic2_x -= 10;
    state_2 = FORWARD;
    } else {                   
    vx2 = -9;
    }
    break;
    case 'd':                      
    if (pic2_y == 62) {
    pic2_x += 10;
    state_2 = BACKWARD;
    } else {
    vx2 =  9;
    }
    break;

    case 'w':                      
    if (pic2_y == 62) {
    vy2   = 30;            
    state_2 = JUMP;
    }
    break;

    case 's':                      
    if (pic2_y > 62)
    vy2 -= 3;
    break;}
    default:
        break;
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
        if (pic1_y == 62)
        {
            pic1_x -= 10;
            state   = BACKWARD;
        }
        else
        {
            vx1 = -9;
        }
    }
    if (key == GLUT_KEY_RIGHT)
    {
        if (pic1_y == 62)            /* on ground → normal walk */
        {
            pic1_x += 10;
            state   = FORWARD;
        }
        else                     
        {
            vx1 =  9;                
        }
    }
    if (key == GLUT_KEY_UP)
    {
        if (pic1_y == 62)            
        {
            vy1   = 30;              
            state = JUMP;
        }
    }
    if (key == GLUT_KEY_DOWN)
    {
        if (pic1_y > 62)             
        {
            vy1 -= 3;                
        }
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
    iSetTimer(100, move_ball); // Start moving ball
    populate_player2_images();
    iSetTimer(100, update_player);
    iSetTimer(30,  gravityTick);
    iInitialize(1000, 600, "Head Soccer");
    return 0;
}
