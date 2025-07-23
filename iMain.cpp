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
    KICK,
    HISTORY,
    P1_NAME_INPUT,
    P2_NAME_INPUT

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
void saveHistory();
void loadHistory();
void addResultToHistory();
void cursorBlink();

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


/*<---------Player Naming and history Variables----------->*/
char player1_name[100] = "Player 1"; // Default name
char player2_name[100] = "Player 2"; // Default name
char name_input_str[100];
int name_input_len = 0;
bool show_cursor = true;
// Game History Structure and Variables
struct GameResult {
    char p1_name[100];
    char p2_name[100];
    int p1_score;
    int p2_score;
};
GameResult history_log[5]; // Store up to 10 past games
int history_count = 0;
/*<------------------------------------------------------>*/


/*<------------------Goal Detection------------------>*/
const int WINDOW_WIDTH   = 1000;
const int WINDOW_HEIGHT  = 600;
const int LEFT_GOAL_X    = 119;   // X-pos of the left goal line
const int RIGHT_GOAL_X   = 873;   // X-pos of the right goal line
const int GOAL_TOP_Y     = 250;   // Y-pos of the bottom of the top goal bar
const int LEFT_BOUNDARY  = 20;    // X-pos of the back of the left goal net
const int RIGHT_BOUNDARY = 960;   // X-pos of the back of the right goal net
/*----------------------------------------------------*/


/*<-----------------------Mode page------------------------>*/
#define GAME_TIME_SECONDS 180
bool mode_3min_selected = true;
bool gameEnded = false;
bool mode_score_selected = false;
#define WIN_SCORE 5

/*----------------------------------------------------------*/


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
const char* LOADING_BG  = "assets/images/Loading_Screen_1.jpg";
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


/*Watch variable*/
int count_timer=0;
int second,minute;
char timeText[20];
/*---------------------------*/


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

    bool p1_is_grounded = (pic1_y < 70);
    bool p2_is_grounded = (pic2_y < 70);

    // Check for x-axis overlap AND if both players are on the ground
    if( pRight(pic1_x) > pLeft(pic2_x) && pRight(pic2_x) > pLeft(pic1_x) && p1_is_grounded && p2_is_grounded)
    {
        // Calculate the amount of horizontal overlap.
        int overlap = (pic1_x < pic2_x) ? (pRight(pic1_x) - pLeft(pic2_x)) : (pRight(pic2_x) - pLeft(pic1_x));

        // Push each player back by half of the overlap to resolve the collision.
        // The +1 ensures they separate completely and don't get stuck.
        int push_back = (overlap / 2) + 1;

        if(pic1_x < pic2_x) {
            // Player 1 is on the left, push them left. Player 2 is on the right, push them right.
            pic1_x -= push_back;
            pic2_x += push_back;
        } else {
            // Player 1 is on the right, push them right. Player 2 is on the left, push them left.
            pic1_x += push_back;
            pic2_x -= push_back;
        }

        // Stop their horizontal movement to prevent them from pushing through each other.
        vx1 = vx2 = 0;
    }
}

void gravityTick()
{
    // Player 1 vertical movement
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
    pic1_x += vx1; // Player 1 horizontal movement

    // Player 2 vertical movement
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
    pic2_x += vx2; // Player 2 horizontal movement

    // Resolve collision between players
    resolvePlayerCollision();

    /* --- Player Boundary Collision --- */
    // Player 1 boundary check
    if (pic1_x < LEFT_BOUNDARY) {
        pic1_x = LEFT_BOUNDARY;
        vx1 = 0; // Stop horizontal movement
    }
    if (pic1_x + player1_width > WINDOW_WIDTH) {
        pic1_x = WINDOW_WIDTH - player1_width;
        vx1 = 0;
    }

    // Player 2 boundary check
    if (pic2_x < 0) {
        pic2_x = 0;
        vx2 = 0;
    }
    if (pic2_x + player2_width > RIGHT_BOUNDARY) {
        pic2_x = RIGHT_BOUNDARY - player2_width;
        vx2 = 0;
    }
}

bool hit(int px, int py, int pw, int ph, int bx, int by, int bw, int bh)
{
    return !( px+pw < bx || bx+bw < px+53 || py+ph < by || by+bh < py+27 );
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

void reset_game(){
    pic1_x = 100; pic1_y = 62;
    pic2_x = 745; pic2_y = 62;
    vx1 = vy1 = vx2 = vy2 = 0;
    state = state_2 = IDLE;
    ball_moving = false;
    pic_ballx = 475; pic_bally = 85;
    player2_score  = 0;
    player1_score  = 0;
    iDecreaseVolume(bgSoundIdx, 80);
}

void move_ball()
{
    if(bvx==0 && bvy==0) return;

    /* New position of ball */
    pic_ballx += bvx;
    pic_bally += bvy;

    /* Adding gravity */
    bvy += BALL_GRAV;

    /* --- Ground and Wall Collision --- */
    if(pic_bally <= GROUND_Y){
        pic_bally = GROUND_Y;
        bvy = (-bvy) * RESTITUTION; // Bounce
        bvx *= FRICTION;            // Friction
        if(fabs(bvy) < 2)  bvy = 0;
        if(fabs(bvx) < 1)  bvx = 0;
    }

    // Top wall collision
    if (pic_bally + ball_width > WINDOW_HEIGHT) {
        pic_bally = WINDOW_HEIGHT - ball_width;
        bvy *= -RESTITUTION;
    }

    // Left wall collision
    if (pic_ballx < 0) {
        pic_ballx = 0;
        bvx *= -RESTITUTION;
    }

    // Right wall collision
    if (pic_ballx + ball_width > WINDOW_WIDTH) {
        pic_ballx = WINDOW_WIDTH - ball_width;
        bvx *= -RESTITUTION;
    }

    /* --- CORRECTED: Goal Detection & Goal Bar Collision --- */
    const int GOAL_BAR_THICKNESS = 15; // A small thickness for the top bar

    // Check Left Goal (Player 2 scores)
    if (pic_ballx <= LEFT_GOAL_X && pic_ballx > LEFT_BOUNDARY) {
        // CASE 1: GOAL
        // The *entire ball* is below the bar.
        if (pic_bally + ball_width < GOAL_TOP_Y) {
            ++player2_score;
            resetField();
            return;
        }
        // CASE 2: COLLISION WITH BAR (The fix is here!)
        // The ball's Y-position overlaps with the bar's Y-position.
        else if (pic_bally < GOAL_TOP_Y + GOAL_BAR_THICKNESS) {
            bvy *= -RESTITUTION; // Bounce it vertically
            // If ball is now moving up, place it just on top of the bar to prevent getting stuck
            if (bvy > 0) {
                pic_bally = GOAL_TOP_Y + GOAL_BAR_THICKNESS;
            }
        }
        // If neither case is met, the ball is flying clear over the top and no action is needed.
    }

    // Check Right Goal (Player 1 scores)
    if (pic_ballx + ball_width >= RIGHT_GOAL_X && pic_ballx + ball_width < RIGHT_BOUNDARY) {
        // CASE 1: GOAL
        if (pic_bally + ball_width < GOAL_TOP_Y) {
            ++player1_score;
            resetField();
            return;
        }
        // CASE 2: COLLISION WITH BAR
        else if (pic_bally < GOAL_TOP_Y + GOAL_BAR_THICKNESS) {
            bvy *= -RESTITUTION;
            if (bvy > 0) {
                pic_bally = GOAL_TOP_Y + GOAL_BAR_THICKNESS;
            }
        }
    }
}


/* Replace your old handlePlayerInput function with this one */
void handlePlayerInput()
{
    if (k != 1) return; // Only handle game input if the game is running

    // --- Player 1 Controls (WASD + F) ---
    if (isKeyPressed('d')) {
        if (pic1_y == 62) { pic1_x += 15; state = FORWARD; }
        else { vx1 = 12; }
    }
    if (isKeyPressed('a')) {
        if (pic1_y == 62) { pic1_x -= 15; state = BACKWARD; }
        else { vx1 = -12; }
    }
    if (isKeyPressed('w')) {
        if (pic1_y == 62) { vy1 = 40; state = JUMP; }
    }
    if (isKeyPressed('s')) {
        if (pic1_y > 62) { vy1 -= 3; }
    }
    if (isKeyPressed('f')) {
        if (pic1_y == 62 && state != KICK) { state = KICK; kick_idx = 0; }
    }

    // --- Player 2 Controls (Arrow Keys + /) ---
    if (isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
        if (pic2_y == 62) { pic2_x += 15; state_2 = BACKWARD; }
        else { vx2 = 12; }
    }
    if (isSpecialKeyPressed(GLUT_KEY_LEFT)) {
        if (pic2_y == 62) { pic2_x -= 15; state_2 = FORWARD; }
        else { vx2 = -12; }
    }
    if (isSpecialKeyPressed(GLUT_KEY_UP)) {
        if (pic2_y == 62) { vy2 = 40; state_2 = JUMP; }
    }
    if (isSpecialKeyPressed(GLUT_KEY_DOWN)) {
        if (pic2_y > 62) { vy2 -= 3; }
    }
    if (isKeyPressed('/')) {
        if (pic2_y == 62 && state_2 != KICK) { state_2 = KICK; kick_idx_2 = 0; }
    }
}


void Watch(){
    if(k==1){
        count_timer++;
        second = count_timer % 60;
        minute = count_timer / 60;
        if(count_timer >= GAME_TIME_SECONDS){
            gameEnded = true;
            k = 5;
            addResultToHistory(); // Add result to log
            saveHistory();        // Save to file
        }
    }    
    sprintf(timeText, "%02d:%02d", minute, second); // 2-digit format    
}

void new_game(){
    iShowImage(0, 40, "assets/images/bg.png");
    iShowImage(300, 500, "assets/images/score_board1.png");
    iShowImage(pic1_x, pic1_y, player1_image);
    iShowImage(pic2_x, pic2_y, player2_image);
    iShowImage(pic_ballx, pic_bally, "assets/images/Ball 02.png");
    int ex1 = (state==KICK ? 15 : 0);      // extra reach P1
    int ex2 = (state_2==KICK ? 15 : 0);    // extra reach P2

    if( hit(pic1_x,pic1_y,player1_width+ex1,123, pic_ballx,pic_bally,ball_width,ball_width) ){
        bool strong = (state == KICK);
        bvx = (pic_ballx >= pic1_x + player1_width/2) ?  20 : -20;
        bvy = strong ? 20 : 0;                      // Projectile motion for only kick
        iPlaySound(kick,false);
    }

    if( hit(pic2_x-ex2,pic2_y,player2_width,123, pic_ballx,pic_bally,ball_width,ball_width) ){
        bool strong = (state_2 == KICK);
        bvx = (pic_ballx >= pic2_x + player2_width/2) ?  20 : -20;
        bvy = strong ? 20 : 0;
        iPlaySound(kick,false);
    }

    ball_moving = (ball_speed>0);
    if(mode_score_selected && !gameEnded){
        if(player1_score >= WIN_SCORE || player2_score >= WIN_SCORE){
            gameEnded = true;
            k = 5;
            addResultToHistory(); // Add result to log
            saveHistory();        // Save to file
        }
    }
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


// Makes the cursor blink during name input
void cursorBlink() {
    show_cursor = !show_cursor;
}

// Adds the most recent game result to our history log
void addResultToHistory() {
    if (history_count < 5) { // Don't exceed array size
        strcpy(history_log[history_count].p1_name, player1_name);
        strcpy(history_log[history_count].p2_name, player2_name);
        history_log[history_count].p1_score = player1_score;
        history_log[history_count].p2_score = player2_score;
        history_count++;
    }
}

// Saves the entire history log to a binary file
void saveHistory() {
    FILE *fp = fopen("history.bin", "wb");
    if (fp != NULL) {
        fwrite(history_log, sizeof(GameResult), history_count, fp);
        fclose(fp);
    }
}

// Loads the history log from the file when the game starts
void loadHistory() {
    FILE *fp = fopen("history.bin", "rb");
    if (fp != NULL) {
        // Read file contents one struct at a time until the end
        while(fread(&history_log[history_count], sizeof(GameResult), 1, fp) == 1) {
            history_count++;
            if (history_count >= 50) break; // Stop if array is full
        }
        fclose(fp);
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
        // NEW: Player 1 Name Input Screen
    if (k == P1_NAME_INPUT) {
        iShowImage(0, 40, "assets/images/bg.png"); // Use any background
        iSetColor(255, 255, 0);
        iText(400, 400, "Enter Player 1 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
        iRectangle(350, 300, 300, 50);
        iSetColor(255, 255, 255);
        iText(360, 320, name_input_str, GLUT_BITMAP_TIMES_ROMAN_24);
        // Blinking cursor
        if (show_cursor) {
            iText(360 + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)name_input_str), 320, "_", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        return;
    }

    // NEW: Player 2 Name Input Screen
    if (k == P2_NAME_INPUT) {
        iShowImage(0, 40, "assets/images/bg.png"); // Use any background
        iSetColor(255, 255, 0);
        iText(400, 400, "Enter Player 2 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
        iRectangle(350, 300, 300, 50);
        iSetColor(255, 255, 255);
        iText(360, 320, name_input_str, GLUT_BITMAP_TIMES_ROMAN_24);
        // Blinking cursor
        if (show_cursor) {
            iText(360 + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)name_input_str), 320, "_", GLUT_BITMAP_TIMES_ROMAN_24);
        }
        return;
    }
    
    // NEW: History Screen
    if (k == HISTORY) {
        iShowImage(0, 40, "assets/images/HIS.jpg");
        iSetColor(255, 255, 0);
        iText(450, 520, "GAME HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);
        iSetColor(255, 0, 0);
        for (int i = 0; i < history_count; i++) {
            char history_text[250];
            // Display latest game at the top
            int index = history_count - 1 - i;
            sprintf(history_text, "%s [%d] - [%d] %s", history_log[index].p1_name, history_log[index].p1_score, history_log[index].p2_score, history_log[index].p2_name);
            iText(350, 450 - (i) * 50, history_text, GLUT_BITMAP_HELVETICA_18);
        }
        // Draw a Back button
        iSetColor(255, 0, 0);
        iFilledRectangle(400, 50, 200, 50);
        iSetColor(255, 255, 255);
        iText(470, 70, "BACK", GLUT_BITMAP_HELVETICA_18);
        return;
    }
    
    if(k==5){ // UPDATED: Result Screen
        iShowImage(0, 40, "assets/images/result.png");
        iSetColor(255,255,255);
        char result_text[200];
        if(player1_score > player2_score)
            sprintf(result_text, "%s WINS!", player1_name);
        else if(player2_score > player1_score)
            sprintf(result_text, "%s WINS!", player2_name);
        else
            sprintf(result_text, "IT'S A DRAW!");
            
        iText(450, 300, result_text, GLUT_BITMAP_TIMES_ROMAN_24);
        return;
    }
    if(k==3){
        draw_settings();
        return;
    }
    if(k==4){
        iShowImage(0,40,"assets/images/Mode.png");
        iSetColor(255, 255, 0); // Yellow for the highlight
    
        // If score mode is selected, draw a rectangle around it
        if (mode_score_selected) {
            iRectangle(228, 241, 428, 71); 
        } 
        // If 3-minute mode is selected, draw a rectangle around it
        else if (mode_3min_selected) {
            iRectangle(227, 340, 428, 68);
        }
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
    if(k==1){
         new_game();
         handlePlayerInput();
        }
    if(k==5){
        iShowImage(0, 40, "assets/images/result.png");
        iSetColor(255,255,255);
        if(player1_score > player2_score)
            iText(450, 300, "PLAYER 1 WINS", GLUT_BITMAP_TIMES_ROMAN_24);
        else if(player2_score > player1_score)
            iText(450, 300, "PLAYER 2 WINS", GLUT_BITMAP_TIMES_ROMAN_24);
        else
            iText(450, 300, "DRAW!", GLUT_BITMAP_TIMES_ROMAN_24);
        return;
    }
    
    char p1_score[20];
    char p2_score[20];
    sprintf(p1_score, " %d ", player1_score);
    sprintf(p2_score, " %d ", player2_score);
    iSetColor(255,255,255);
    iText(385, 541, p1_score,GLUT_BITMAP_TIMES_ROMAN_24);  // Draw at top-left corner (adjust coords as needed)
    iSetColor(255,255,255);
    iText(587, 541, p2_score,GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 255, 255);  // White color (change if needed)
    iText(478, 507, timeText, GLUT_BITMAP_9_BY_15); // Top-center-ish position
    drawGoalOverlay();
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
            k = P1_NAME_INPUT;      // Go to Player 1 Name Input screen
            name_input_len = 0;     // Reset input string
            name_input_str[0] = '\0';
            return;
                /* Start play game */
        }
        if(mx>=MODES_X && mx<=MODES_X+BTN_W && my>=MODES_Y && my<=MODES_Y+BTN_H){
            k=4;
            return;
        }
        if(mx>=HIST_X && mx<=HIST_X+BTN_W && my>=HIST_Y && my<=HIST_Y+BTN_H){
            k = HISTORY; // Go to History screen
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
            if(mx>=BG_MUSIC_PLUS_BUTTON_X && mx<=BG_MUSIC_PLUS_BUTTON_X+60 && my>=BG_MUSIC_PLUS_BUTTON_Y && my<=BG_MUSIC_PLUS_BUTTON_Y+45){
                iIncreaseVolume(bgSoundIdx, 10);
            }
            /* --- BG music - --- */
            if(mx>=BG_MUSIC_MINUS_BUTTON_X && mx<=BG_MUSIC_MINUS_BUTTON_X+60 && my>=BG_MUSIC_MINUS_BUTTON_Y && my<=BG_MUSIC_MINUS_BUTTON_Y+45){
                iDecreaseVolume(bgSoundIdx, 10);
            }

            /* --- SFX + --- */
            if(mx>=SOUND_MUSIC_PLUS_BUTTON_X && mx<=SOUND_MUSIC_PLUS_BUTTON_X+60 && my>=SOUND_MUSIC_PLUS_BUTTON_Y && my<=SOUND_MUSIC_PLUS_BUTTON_Y+45){
                iIncreaseVolume(cheerSoundIdx, 10);
                iIncreaseVolume(kickSoundIdx, 10);
            }
            /* --- SFX - --- */
            if(mx>=SOUND_MUSIC_MINUS_BUTTON_X && mx<=SOUND_MUSIC_MINUS_BUTTON_X+60 && my>=SOUND_MUSIC_MINUS_BUTTON_Y && my<=SOUND_MUSIC_MINUS_BUTTON_Y+45){
                iDecreaseVolume(cheerSoundIdx, 10);
                iDecreaseVolume(kickSoundIdx, 10);
            }

            /* --- Back button --- */
            if(mx>=BTN_BACK_X && mx<=BTN_BACK_X+BTN_BACK_W && my>=BTN_BACK_Y && my<=BTN_BACK_Y+BTN_BACK_H){
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
    if(k == 4 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(mx >= 227 && mx <= 227+428 && my >= 340 && my <= 340+68){
            mode_3min_selected = true;
            mode_score_selected = false; // Ensure other modes are off
            k = 0; // Go back to the main menu
        }
        else if(mx >= 227 && mx <= 227+428 && my >= 240 && my <= 240+68){
            mode_score_selected = true;
            mode_3min_selected = false; // Ensure other modes are off
            k = 0; // Go back to the main menu
        }
        else if(mx>=227 && mx<=227+428 && my>=150 && my<=150+68){
                k = 0;
        }
    }
    if (k == HISTORY && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Assuming a back button at bottom-center
        if (mx >= 400 && mx <= 600 && my >= 50 && my <= 100) {
            k = 0; // Go back to main menu
        }
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
*/
void iKeyboard(unsigned char key)
{
    // Only handle single-press, non-movement actions here.
    if (k == P1_NAME_INPUT || k == P2_NAME_INPUT) {
        if (key == '\r') { // Enter key pressed
            if (k == P1_NAME_INPUT) {
                strcpy(player1_name, name_input_str);
                k = P2_NAME_INPUT; // Move to player 2's turn
                name_input_len = 0;
                name_input_str[0] = '\0';
            } else if (k == P2_NAME_INPUT) {
                strcpy(player2_name, name_input_str);
                // Now start the game
                k = 1;
                reset_game();
                gameEnded = false;
                count_timer = -1;
                minute = 0;
                second = 0;
            }
        } else if (key == '\b') { // Backspace key
            if (name_input_len > 0) {
                name_input_len--;
                name_input_str[name_input_len] = '\0';
            }
        } else { // Any other character
            if (name_input_len < 99) {
                name_input_str[name_input_len] = key;
                name_input_len++;
                name_input_str[name_input_len] = '\0';
            }
        }
        return; // Don't process other keys while typing name
    }
    switch (key)
    {
    case 'm':
     k=0; 
     iIncreaseVolume(bgSoundIdx, 80);
     mode_3min_selected = false;
     gameEnded = false;
     break;
    }
}

/*
function iSpecialKeyboard() is called whenver user hits special keys
*/
void iSpecialKeyboard(unsigned char key)
{
    // Only handle single-press, non-movement actions here.
    if (key == GLUT_KEY_END) {
        exit(0);
    }
}


int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    // place your own initialization codes here.
    loadHistory();
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
    iSetTimer(500, cursorBlink);
    loadingDone = false;
    k = -1;
    iDecreaseVolume(bgSoundIdx, 100);
    iInitialize(1000, 600, "Head Soccer");
    return 0;  
}
