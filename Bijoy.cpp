#include "iGraphics.h"
#include <cmath>
#include <iostream>
#include "iSound.h"


using namespace std;

enum {
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

enum PowerUpType {
    NONE,
    FREEZE,
    SUPER_KICK,
    SIZE_DOWN
};

// Function prototypes
void gravityTick(void);
void move_ball(void);
void new_game(void);
void populate_player1_images(void);
void populate_player2_images(void);
void loadresources(void);
void draw_settings(void);
void iDraw(void);
void iMouseMove(int mx, int my);
void iMouseDrag(int mx, int my);
void iMouse(int button, int state, int mx, int my);
void iMouseWheel(int dir, int mx, int my);
void iKeyboard(unsigned char key);
void iSpecialKeyboard(unsigned char key);
void update_player1(void);
void update_player2(void);
void update_player(void);
void goalTick(void);
void saveHistory();
void loadHistory();
void addResultToHistory();
bool hit(int px, int py, int pw, int ph, int bx, int by, int bw, int bh);
void powerUpSpawner(void);
void effectsManager(void);
void saveGameState();
bool loadGameState();
void resetField();
void reset_game();
void Watch();
void drawLoadingScreen();
void loadingTick();
void drawGoalOverlay();
void blink();
void handlePlayerInput();
void resolvePlayerCollision();


// Menu button positions and dimensions (adjust these coordinates manually for your Menu.png)
#define BTN_W    350
#define BTN_H     60

// New Button Coordinates (adjust these precisely)
#define NEW_GAME_X    602
#define NEW_GAME_Y    450 // Example: Higher than previous PLAY_Y
#define CONTINUE_X    602
#define CONTINUE_Y    373 // Example: Approx original PLAY_Y
#define MODES_NEW_X   602
#define MODES_NEW_Y   294 // Example: Approx original MODES_Y

// Existing Button Coordinates (assuming they don't change much relative to each other)
#define HIST_X    602
#define HIST_Y    217
#define SET_X     602
#define SET_Y     138
#define QUIT_X    0 // This seems like an outlier, might be a specific corner button
#define QUIT_Y    538
#define QUIT_W    214
#define QUIT_H    45

int m = 0, n = 0, x = 0, y = 0, z = 0;
int q_quit_hover = 0; // New flag for QUIT button hover
int k = 0;

bool saveGameExists = false;
bool isPaused = false;
bool showSaveConfirmation = false;
int saveConfirmTimer = 0;

// Player Naming and history Variables
char player1_name[100] = "Player 1";
char player2_name[100] = "Player 2";
char name_input_str[100];
int name_input_len = 0;

struct GameResult {
    char p1_name[100];
    char p2_name[100];
    int p1_score;
    int p2_score;
};

const int MAX_HISTORY = 5;
GameResult history_log[MAX_HISTORY];
int history_count = 0;
int next_history_slot = 0;

// Goal Detection
const int WINDOW_WIDTH   = 1000;
const int WINDOW_HEIGHT  = 600;
const int LEFT_GOAL_X    = 119;
const int RIGHT_GOAL_X   = 873;
const int GOAL_TOP_Y     = 250;
const int LEFT_BOUNDARY  = 20;
const int RIGHT_BOUNDARY = 960;

// Mode page
#define GAME_TIME_SECONDS 180
bool mode_3min_selected = true;
bool gameEnded = false;
bool mode_score_selected = false;
#define WIN_SCORE 5

// Setting menu
Image settings_bg;
#define BG_MUSIC_PLUS_BUTTON_X  600
#define BG_MUSIC_PLUS_BUTTON_Y  370
#define BG_MUSIC_MINUS_BUTTON_X 678
#define BG_MUSIC_MINUS_BUTTON_Y 370

#define SOUND_MUSIC_PLUS_BUTTON_X  600
#define SOUND_MUSIC_PLUS_BUTTON_Y  290
#define SOUND_MUSIC_MINUS_BUTTON_X 678
#define SOUND_MUSIC_MINUS_BUTTON_Y 295

#define BTN_BACK_X   290
#define BTN_BACK_Y   192
#define BTN_BACK_W   355
#define BTN_BACK_H    60

// Sound
int bgSoundIdx    = -1;
int cheerSoundIdx = -1;
int kickSoundIdx = -1;
const char* BG_MUSIC_FILE  = "assets/sounds/bgm.WAV";
const char* CHEER_FILE     = "assets/sounds/Goal.WAV";
const char* kick = "assets/sounds/kick.WAV";

// Loading-screen
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

// Goal animation constants
#define GOAL_FRAMES 21
#define GOAL_FRAME_MS 100
#define GOAL_CX 500
#define GOAL_CY 300

const int GOAL_SIZE[GOAL_FRAMES] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 150, 150, 150, 150, 150, 150};
const char* GOAL_FILES[GOAL_FRAMES] = {
    "assets/images/Goal/Goal_001.png", "assets/images/Goal/Goal_002.png", "assets/images/Goal/Goal_003.png",
    "assets/images/Goal/Goal_004.png", "assets/images/Goal/Goal_005.png", "assets/images/Goal/Goal_006.png",
    "assets/images/Goal/Goal_007.png", "assets/images/Goal/Goal_008.png", "assets/images/Goal/Goal_009.png",
    "assets/images/Goal/Goal_010.png", "assets/images/Goal/Goal_011.png", "assets/images/Goal/Goal_012.png",
    "assets/images/Goal/Goal_013.png", "assets/images/Goal/Goal_014.png", "assets/images/Goal/Goal_015.png",
    "assets/images/Goal/Goal_016.png", "assets/images/Goal/Goal_017.png", "assets/images/Goal/Goal_018.png",
    "assets/images/Goal/Goal_019.png", "assets/images/Goal/Goal_020.png", "assets/images/Goal/Goal_021.png"
};
Image goalImg[GOAL_FRAMES];
int goalFrame = 0;
bool goalAnimPlaying = false;

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
int ex1, ex2;

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

int count_timer=0;
int second,minute;
char timeText[20];

int pic_ballx,pic_bally;
int ball_speed = 0;
bool ball_moving = false;
int ball_dir = 1;
int ball_width = 50;
int player1_width = 103;
int player2_width = 103;
int player1_score = 0;
int player2_score  = 0;
int bvx = 0, bvy = 0;
const int BALL_GRAV = -2;
const int GROUND_Y  = 85;
const float RESTITUTION = 0.60f;
const float FRICTION    = 0.97f;

int vy1 = 0;
int vx1 = 0;
int vy2 = 0;
int vx2 = 0;

// Power-Up System Variables
struct PowerUpIcon {
    bool isActive;
    PowerUpType type;
    int x, y;
    int width = 50;
    int height = 50;
};

struct PlayerEffects {
    PowerUpType armedPowerUp = NONE;
    bool isFrozen = false;
    bool isSizeDown = false;
    int effectTimer = 0;
};

PowerUpIcon currentPowerUpIcon;
PlayerEffects player1_effects;
PlayerEffects player2_effects;

int lastPlayerToTouchBall = 0;
bool isBallFiery = false;

void powerUpSpawner() {
    if (isPaused) return;
    if (!currentPowerUpIcon.isActive) {
        currentPowerUpIcon.isActive = true;

        int randType = rand() % 3;
        if (randType == 0) currentPowerUpIcon.type = FREEZE;
        else if (randType == 1) currentPowerUpIcon.type = SUPER_KICK;
        else currentPowerUpIcon.type = SIZE_DOWN;

        currentPowerUpIcon.x = 300 + rand() % 400;
        currentPowerUpIcon.y = 150 + rand() % 100;
    }
}

void effectsManager() {
    if (isPaused) return;
    if (player1_effects.effectTimer > 0) {
        player1_effects.effectTimer--;
    } else {
        player1_effects.isFrozen = false;
        player1_effects.isSizeDown = false;
    }

    if (player2_effects.effectTimer > 0) {
        player2_effects.effectTimer--;
    } else {
        player2_effects.isFrozen = false;
        player2_effects.isSizeDown = false;
    }
}

inline int pLeft (int px){ return px + 53; }
inline int pRight(int px){ return px + 103; }

void resolvePlayerCollision() {
    bool p1_is_grounded = (pic1_y < 70);
    bool p2_is_grounded = (pic2_y < 70);
    int g = (pic1_x<pic2_x) ? (pic2_x - pic1_x) : (pic1_x - pic2_x);
    if( pRight(pic1_x) > pLeft(pic2_x) && pRight(pic2_x) > pLeft(pic1_x) && p1_is_grounded && p2_is_grounded) {
        int overlap = (pic1_x < pic2_x) ? (pRight(pic1_x) - pLeft(pic2_x)) : (pRight(pic2_x) - pLeft(pic1_x));
        int push_back = (overlap / 2) + 1;

        if(pic1_x < pic2_x) {
            pic1_x -= push_back;
            pic2_x += push_back;
        } else {
            pic1_x += push_back;
            pic2_x -= push_back;
        }
        vx1 = vx2 = 0;
    } else if((hit(pic1_x,pic1_y,player1_width+ex1,123, pic_ballx,pic_bally,ball_width,ball_width) || hit(pic2_x-ex2,pic2_y,player2_width,123, pic_ballx,pic_bally,ball_width,ball_width)) && (g <= 115)){
        if(pic1_x < pic_ballx) {
            pic1_x -= 5;
        } else if(pic1_x > pic_ballx){
            pic1_x += 5;
        }
        if(pic2_x < pic_ballx) {
            pic2_x -= 5;
        } else if(pic2_x > pic_ballx){
            pic2_x += 5;
        }
        vx1 = vx2 = 0;
    }
}

void gravityTick() {
    if (isPaused) return;
    if (pic1_y > 62 || vy1 > 0) {
        vy1 -= 2;
        pic1_y += vy1;
        if (pic1_y <= 62) {
            pic1_y = 62;
            vy1 = vx1 = 0;
            state = IDLE;
        }
    }
    pic1_x += vx1;

    if (pic2_y > 62 || vy2 > 0) {
        vy2 -= 2;
        pic2_y += vy2;
        if (pic2_y <= 62) {
            pic2_y = 62;
            vy2 = vx2 = 0;
            state_2 = IDLE;
        }
    }
    pic2_x += vx2;

    resolvePlayerCollision();

    if (pic1_x < LEFT_BOUNDARY) {
        pic1_x = LEFT_BOUNDARY;
        vx1 = 0;
    }
    if (pic1_x + player1_width > WINDOW_WIDTH) {
        pic1_x = WINDOW_WIDTH - player1_width;
        vx1 = 0;
    }

    if (pic2_x < 0) {
        pic2_x = 0;
        vx2 = 0;
    }
    if (pic2_x + player2_width > RIGHT_BOUNDARY) {
        pic2_x = RIGHT_BOUNDARY - player2_width;
        vx2 = 0;
    }
}

bool hit(int px, int py, int pw, int ph, int bx, int by, int bw, int bh) {
    return !( px+pw < bx || bx+bw < px+53 || py+ph < by || by+bh < py+27 );
}

void resetField() {
    goalAnimPlaying = true;
    goalFrame = 0;
    cheerSoundIdx = iPlaySound(CHEER_FILE, false);
    bvx = bvy = 0;
    pic_ballx = 475;
    pic_bally = GROUND_Y;

    ball_speed = 0;
    ball_dir = 0;
    ball_moving = false;

    pic1_x = 100;
    pic1_y = 62;
    vx1 = vy1 = 0;
    state = IDLE;

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

void move_ball() {
    if (isPaused) return;
    if(bvx==0 && bvy==0) return;

    pic_ballx += bvx;
    pic_bally += bvy;

    bvy += BALL_GRAV;

    if(pic_bally <= GROUND_Y){
        pic_bally = GROUND_Y;
        bvy = (-bvy) * RESTITUTION;
        bvx *= FRICTION;
        if(fabs(bvy) < 2)  bvy = 0;
        if(fabs(bvx) < 1)  bvx = 0;
    }

    if (pic_bally + ball_width > WINDOW_HEIGHT) {
        pic_bally = WINDOW_HEIGHT - ball_width;
        bvy *= -RESTITUTION;
    }

    if (pic_ballx < 0) {
        pic_ballx = 0;
        bvx *= -RESTITUTION;
    }

    if (pic_ballx + ball_width > WINDOW_WIDTH) {
        pic_ballx = WINDOW_WIDTH - ball_width;
        bvx *= -RESTITUTION;
    }

    const int GOAL_BAR_THICKNESS = 15;

    if (pic_ballx <= LEFT_GOAL_X && pic_ballx > LEFT_BOUNDARY) {
        if (pic_bally + ball_width < GOAL_TOP_Y) {
            ++player2_score;
            resetField();
            return;
        } else if (pic_bally < GOAL_TOP_Y + GOAL_BAR_THICKNESS) {
            bvy *= -RESTITUTION;
            if (bvy > 0) {
                pic_bally = GOAL_TOP_Y + GOAL_BAR_THICKNESS;
            }
        }
    }

    if (pic_ballx + ball_width >= RIGHT_GOAL_X && pic_ballx + ball_width < RIGHT_BOUNDARY) {
        if (pic_bally + ball_width < GOAL_TOP_Y) {
            ++player1_score;
            resetField();
            return;
        } else if (pic_bally < GOAL_TOP_Y + GOAL_BAR_THICKNESS) {
            bvy *= -RESTITUTION;
            if (bvy > 0) {
                pic_bally = GOAL_TOP_Y + GOAL_BAR_THICKNESS;
            }
        }
    }
    if (currentPowerUpIcon.isActive) {
        if (hit(currentPowerUpIcon.x, currentPowerUpIcon.y, currentPowerUpIcon.width, currentPowerUpIcon.height, pic_ballx, pic_bally, ball_width, ball_width)) {
            PowerUpType collectedType = currentPowerUpIcon.type;
            currentPowerUpIcon.isActive = false;

            if (lastPlayerToTouchBall == 1) {
                if (collectedType == SUPER_KICK) {
                    player1_effects.armedPowerUp = SUPER_KICK;
                } else if (collectedType == FREEZE) {
                    player2_effects.isFrozen = true;
                    player2_effects.effectTimer = 30;
                } else if (collectedType == SIZE_DOWN) {
                    player2_effects.isSizeDown = true;
                    player2_effects.effectTimer = 50;
                }
            } else if (lastPlayerToTouchBall == 2) {
                if (collectedType == SUPER_KICK) {
                    player2_effects.armedPowerUp = SUPER_KICK;
                } else if (collectedType == FREEZE) {
                    player1_effects.isFrozen = true;
                    player1_effects.effectTimer = 30;
                } else if (collectedType == SIZE_DOWN) {
                    player1_effects.isSizeDown = true;
                    player1_effects.effectTimer = 50;
                }
            }
        }
    }
}


void handlePlayerInput() {
    if (isPaused) return;
    if (k != 1) return;

    if (!player1_effects.isFrozen){
        if (isKeyPressed('d') ||isKeyPressed('D')  ) {
            if (pic1_y == 62) { pic1_x += 15; state = FORWARD; }
            else { vx1 = 12; }
        }
        if (isKeyPressed('a') || isKeyPressed('A') ) {
            if (pic1_y == 62) { pic1_x -= 15; state = BACKWARD; }
            else { vx1 = -12; }
        }
        if (isKeyPressed('w') || isKeyPressed('W') ) {
            if (pic1_y == 62) { vy1 = 40; state = JUMP; }
        }
        if (isKeyPressed('s') || isKeyPressed('S') ) {
            if (pic1_y > 62) { vy1 -= 3; }
        }
        if (isKeyPressed('f') || isKeyPressed('F')) {
            if (pic1_y == 62 && state != KICK) { state = KICK; kick_idx = 0; }
        }
    } else {
        vx1 = 0;
    }

    if (!player2_effects.isFrozen){
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
    } else {
        vx2 = 0;
    }
}

void Watch(){
    if (isPaused) return;
    if(k==1){
        count_timer++;
        second = count_timer % 60;
        minute = count_timer / 60;
        if(count_timer >= GAME_TIME_SECONDS){
            gameEnded = true;
            k = 5;
            addResultToHistory();
            saveHistory();
        }
    }
    sprintf(timeText, "%02d:%02d", minute, second);
}

void new_game(){
    iShowImage(0, 40, "assets/images/bg.png");
    iShowImage(300, 500, "assets/images/score_board1.png");

    int p1_w = player1_width, p1_h = 123;
    if (player1_effects.isSizeDown) { p1_w /= 2; p1_h /= 2; }
    if (player1_effects.isFrozen) {
        iShowImage(pic1_x, pic1_y, "assets/images/Freez1.png");
    } else {
        iShowImage(pic1_x, pic1_y, player1_image);
    }

    int p2_w = player2_width, p2_h = 123;
    if (player2_effects.isSizeDown) { p2_w /= 2; p2_h /= 2; }
    if (player2_effects.isFrozen) {
        iShowImage(pic2_x, pic2_y, "assets/images/Freez2.png");
    } else {
        iShowImage(pic2_x, pic2_y, player2_image);
    }

    if (isBallFiery) {
        iShowImage(pic_ballx, pic_bally, "assets/images/fire ball 001.png");
    } else {
        iShowImage(pic_ballx, pic_bally, "assets/images/Ball 02.png");
    }
    ex1 = (state==KICK ? 15 : 0);
    ex2 = (state_2==KICK ? 15 : 0);

    if( hit(pic1_x,pic1_y,player1_width+ex1,123, pic_ballx,pic_bally,ball_width,ball_width) ){
        if (isBallFiery && lastPlayerToTouchBall == 2) {
            if(pic1_x < pic_ballx) { pic1_x -= 300; }
            else if(pic1_x > pic_ballx){ pic1_x += 300; }
            isBallFiery = false;
        }
        lastPlayerToTouchBall = 1;
        bool strong = (state == KICK);
        bvx = (pic_ballx >= pic1_x + 20 ) ?  20 : -20;
        if (player1_effects.armedPowerUp == SUPER_KICK && strong) {
            bvx *= 3;
            bvy = 15;
            isBallFiery = true;
            player1_effects.armedPowerUp = NONE;
        } else {
            bvy = strong ? 20 : 0;
        }
        iPlaySound(kick,false);
    }

    if( hit(pic2_x-ex2,pic2_y,player2_width,123, pic_ballx,pic_bally,ball_width,ball_width) ){
        if (isBallFiery && lastPlayerToTouchBall == 1) {
            if(pic2_x < pic_ballx) { pic2_x -= 300; }
            else if(pic2_x > pic_ballx){ pic2_x += 300; }
            isBallFiery = false;
        }
        lastPlayerToTouchBall = 2;
        bool strong = (state_2 == KICK);
        bvx = (pic_ballx >= pic2_x + player2_width/2) ?  20 : -20;
        if (player2_effects.armedPowerUp == SUPER_KICK && strong) {
            bvx *= 3;
            bvy = 15;
            isBallFiery = true;
            player2_effects.armedPowerUp = NONE;
        } else {
            bvy = strong ? 20 : 0;
        }
        iPlaySound(kick,false);
    }

    ball_moving = (ball_speed>0);
    if(mode_score_selected && !gameEnded){
        if(player1_score >= WIN_SCORE || player2_score >= WIN_SCORE){
            gameEnded = true;
            k = 5;
            addResultToHistory();
            saveHistory();
        }
    }
}

void update_player(){
    if (isPaused) return;
    update_player1();
    update_player2();
    goalTick();
}

void populate_player1_images(void) {
    for (int i = 0; i < 18; i++) { sprintf(player1_idle[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Idle/Idle_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player1_jump[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Jump/Jump_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player1_falldown[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Falling Down/Falling Down_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player1_forward[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Move Forward/Move Forward_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player1_kick[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Kick/Kick_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player1_backward[i], "assets/Characters/Character 01 - Brazil/PNG Sequences/Move Backward/Move Backward_%03d.png", i); }
    player1_image = player1_idle[0];
}

void populate_player2_images(void) {
    for (int i = 0; i < 18; i++) { sprintf(player2_idle[i], "assets/Characters/Character 02 - England/PNG Sequences/Idle/Idle_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player2_jump[i], "assets/Characters/Character 02 - England/PNG Sequences/Jump/Jump_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player2_falldown[i], "assets/Characters/Character 02 - England/PNG Sequences/Falling Down/Falling Down_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player2_forward[i], "assets/Characters/Character 02 - England/PNG Sequences/Move Forward/Move Forward_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player2_kick[i], "assets/Characters/Character 02 - England/PNG Sequences/Kick/Kick_%03d.png", i); }
    for (int i = 0; i < 5; i++) { sprintf(player2_backward[i], "assets/Characters/Character 02 - England/PNG Sequences/Move Backward/Move Backward_%03d.png", i); }
    player2_image = player2_idle[0];
}

Image bg;
void loadresources() {
    iLoadImage(&bg, "assets/images/Menu.png");
    for (int i = 0; i < GOAL_FRAMES; i++) { iLoadImage(&goalImg[i], GOAL_FILES[i]); }
    iLoadImage(&settings_bg, "assets/images/setting.JPG");
}

void draw_settings() {
    iShowImage(0, 40, "assets/images/setting.JPG");
    iSetColor(255,255,255);
    iRectangle(290,192,355,60);
}

void blink(){ b=!b; }

void drawLoadingScreen() {
    iShowImage(0, 40, LOADING_BG);
    if(!loadingStarted){
        iSetColor(255,255,255);
        if(b==0){
            iText(430, 70, "Click to continue", GLUT_BITMAP_9_BY_15);
            iText(431, 71, "Click to continue", GLUT_BITMAP_9_BY_15);
        }
        return;
    }

    int cx[3] = { 440, 500, 560 };
    iSetColor(255,255,255);
    for(int i=0;i<3;i++){ iFilledCircle(cx[i], LOADING_Y, LOADING_RADIUS); }
    iShowImage(cx[currentCircle]-LOADING_RADIUS,LOADING_Y-LOADING_RADIUS,BALL_IMG);
}

void loadingTick() {
    if(!loadingStarted || loadingDone) return;
    loadingTimer += 20;
    if( loadingTimer % BALL_JUMP_MS == 0 ){ currentCircle = (currentCircle+1)%3; }
    if( loadingTimer >= LOADING_TIME_MS ){
        loadingDone = true;
        k = 0;
        iIncreaseVolume(bgSoundIdx, 100);
    }
}

void drawGoalOverlay() {
    if (!goalAnimPlaying) return;
    int w = GOAL_SIZE[goalFrame];
    int h = w;
    int x = GOAL_CX - w / 2;
    int y = GOAL_CY - h / 2;
    iShowImage(x, y, GOAL_FILES[goalFrame]);
}

void goalTick() {
    if (!goalAnimPlaying) return;
    goalFrame++;
    if (goalFrame >= GOAL_FRAMES) {
        goalAnimPlaying = false;
        goalFrame = 0;
    }
}

void addResultToHistory() {
    strcpy(history_log[next_history_slot].p1_name, player1_name);
    strcpy(history_log[next_history_slot].p2_name, player2_name);
    history_log[next_history_slot].p1_score = player1_score;
    history_log[next_history_slot].p2_score = player2_score;

    next_history_slot = (next_history_slot + 1) % MAX_HISTORY;
    if (history_count < MAX_HISTORY) { history_count++; }
}

void saveGameState() {
    if (k != 1) return;
    FILE *fp = fopen("savegame.txt", "w");
    if (fp != NULL) {
        fprintf(fp, "%d\n", player1_score);
        fprintf(fp, "%d\n", player2_score);
        fprintf(fp, "%d\n", count_timer);
        fprintf(fp, "%d\n", mode_3min_selected);
        fclose(fp);
    }
}

bool loadGameState() {
    FILE *fp = fopen("savegame.txt", "r");
    if (fp != NULL) {
        int mode_flag;
        if (fscanf(fp, "%d %d %d %d", &player1_score, &player2_score, &count_timer, &mode_flag) == 4) {
            mode_3min_selected = (mode_flag == 1);
            mode_score_selected = !mode_3min_selected;
            fclose(fp);
            return true;
        }
        fclose(fp);
    }
    return false;
}

void saveHistory() {
    FILE *fp = fopen("history.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < history_count; i++) {
            int index = (next_history_slot + i) % MAX_HISTORY;
            char temp_p1_name[100];
            char temp_p2_name[100];
            strcpy(temp_p1_name, history_log[index].p1_name);
            strcpy(temp_p2_name, history_log[index].p2_name);

            for (int j = 0; temp_p1_name[j] != '\0'; j++) { if (temp_p1_name[j] == ' ') temp_p1_name[j] = '_'; }
            for (int j = 0; temp_p2_name[j] != '\0'; j++) { if (temp_p2_name[j] == ' ') temp_p2_name[j] = '_'; }

            fprintf(fp, "%s %s %d %d\n", temp_p1_name, temp_p2_name, history_log[index].p1_score, history_log[index].p2_score);
        }
        fclose(fp);
    }
}

void loadHistory() {
    FILE *fp = fopen("history.txt", "r");
    if (fp != NULL) {
        history_count = 0;
        while (fscanf(fp, "%s %s %d %d",
                      history_log[history_count].p1_name,
                      history_log[history_count].p2_name,
                      &history_log[history_count].p1_score,
                      &history_log[history_count].p2_score) != EOF)
        {
            char* p1_name = history_log[history_count].p1_name;
            for (int j = 0; p1_name[j] != '\0'; j++) { if (p1_name[j] == '_') p1_name[j] = ' '; }
            char* p2_name = history_log[history_count].p2_name;
            for (int j = 0; p2_name[j] != '\0'; j++) { if (p2_name[j] == '_') p2_name[j] = ' '; }

            history_count++;
            if (history_count >= MAX_HISTORY) break;
        }
        fclose(fp);
        next_history_slot = history_count % MAX_HISTORY;
    }
}


void iDraw() {
    iClear();
    if(!loadingDone){
        drawLoadingScreen();
        return;
    }
    if (k == P1_NAME_INPUT) {
        iShowImage(0, 40, "assets/images/name.jpg");
        iShowImage(20,520, "assets/images/Button1.png");
        iSetColor(255, 255, 0);
        iText(400, 400, "Enter Player 1 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
        iRectangle(350, 300, 300, 50);
        iSetColor(255, 255, 255);
        iText(360, 320, name_input_str, GLUT_BITMAP_TIMES_ROMAN_24);
        if (b==0) { iText(360 + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)name_input_str), 320, "_", GLUT_BITMAP_TIMES_ROMAN_24); }
        return;
    }

    if (k == P2_NAME_INPUT) {
        iShowImage(0, 40, "assets/images/name.jpg");
        iShowImage(20,520, "assets/images/Button1.png");
        iSetColor(255, 255, 0);
        iText(400, 400, "Enter Player 2 Name:", GLUT_BITMAP_TIMES_ROMAN_24);
        iRectangle(350, 300, 300, 50);
        iSetColor(255, 255, 255);
        iText(360, 320, name_input_str, GLUT_BITMAP_TIMES_ROMAN_24);
        if (b==0) { iText(360 + glutBitmapLength(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)name_input_str), 320, "_", GLUT_BITMAP_TIMES_ROMAN_24); }
        return;
    }

    if (k == HISTORY) {
        iShowImage(0, 40, "assets/images/HIS.jpg");
        iSetColor(255, 0,0);
        int newest_game_index = (next_history_slot - 1 + MAX_HISTORY) % MAX_HISTORY;
        for (int i = 0; i < history_count; i++) {
            char history_text[250];
            int index = (newest_game_index - i + MAX_HISTORY) % MAX_HISTORY;
            sprintf(history_text, "%d        %d", history_log[index].p1_score, history_log[index].p2_score);
            iText(170, 425 - i * 65, history_log[index].p1_name, GLUT_BITMAP_TIMES_ROMAN_24);
            iText(457, 425 - i * 65, history_text, GLUT_BITMAP_TIMES_ROMAN_24);
            iText(580, 425 - i * 65, history_log[index].p2_name, GLUT_BITMAP_TIMES_ROMAN_24);
        }
        iSetColor(255, 0, 0);
        iFilledRectangle(400, 50, 200, 50);
        iSetColor(255, 255, 255);
        iText(470, 70, "BACK", GLUT_BITMAP_HELVETICA_18);
        return;
    }

    if(k==5){
        iShowImage(0, 40, "assets/images/result.png");
        iSetColor(255,255,255);
        char result_text[200];
        if(player1_score > player2_score)
            sprintf(result_text, "%s WINS!", player1_name);
        else if(player2_score > player1_score)
            sprintf(result_text, "%s WINS!", player2_name);
        else
            sprintf(result_text, "IT'S A DRAW!");

        iText(400, 400, result_text, GLUT_BITMAP_TIMES_ROMAN_24);
        sprintf(result_text, "Score: %d - %d", player1_score, player2_score);
        iText(400, 350, result_text, GLUT_BITMAP_TIMES_ROMAN_24);

        iSetColor(255, 0, 0);
        iFilledRectangle(400, 50, 200, 50);
        iSetColor(255, 255, 255);
        iText(470, 70, "BACK", GLUT_BITMAP_HELVETICA_18);
        return;
    }

    // Main Menu (k==0)
    if(k==0){
        iShowImage(0,40,"assets/images/Menu.png");
        iSetColor(255,255,255);

        // NEW GAME Button (top button)
        if(!m) iText(NEW_GAME_X + 130, NEW_GAME_Y + 25, "NEW GAME", GLUT_BITMAP_HELVETICA_18);

        // CONTINUE / NO SAVED GAMES Button (second from top)
        if (saveGameExists) {
            if(!n) iText(CONTINUE_X + 125, CONTINUE_Y + 25, "CONTINUE", GLUT_BITMAP_HELVETICA_18);
        } else {
            iSetColor(150, 150, 150); // Gray out "NO SAVED GAMES" text
            iText(CONTINUE_X + 100, CONTINUE_Y + 25, "NO SAVED GAMES", GLUT_BITMAP_HELVETICA_18);
            iSetColor(255, 255, 255); // Reset color
        }

        // MODES Button (third from top)
        if(!x) iText(MODES_NEW_X + 140, MODES_NEW_Y + 25, "MODES", GLUT_BITMAP_HELVETICA_18);

        // HISTORY Button (fourth from top)
        if(!y) iText(HIST_X+130,  HIST_Y+25,  "HISTORY", GLUT_BITMAP_HELVETICA_18);

        // SETTING Button (fifth from top)
        if(!z) iText(SET_X+130,   SET_Y+25,   "SETTING", GLUT_BITMAP_HELVETICA_18);

        // QUIT Button (last)
        if(!q_quit_hover) iText(QUIT_X+80,  QUIT_Y+18,  "QUIT",    GLUT_BITMAP_HELVETICA_18);

        // Highlighted text (yellow)
        iSetColor(255,255,0);

        if(m){ iText(NEW_GAME_X + 125, NEW_GAME_Y + 22, "NEW GAME", GLUT_BITMAP_TIMES_ROMAN_24); }
        if(n && saveGameExists){ iText(CONTINUE_X + 120, CONTINUE_Y + 22, "CONTINUE", GLUT_BITMAP_TIMES_ROMAN_24); }
        if(x){ iText(MODES_NEW_X + 133, MODES_NEW_Y + 22, "MODES", GLUT_BITMAP_TIMES_ROMAN_24); }
        if(y){iText(HIST_X+125, HIST_Y+22, "HISTORY", GLUT_BITMAP_TIMES_ROMAN_24);}
        if(z){iText(SET_X+125, SET_Y+22, "SETTING", GLUT_BITMAP_TIMES_ROMAN_24);}
        if(q_quit_hover){iText(QUIT_X+75, QUIT_Y+15, "QUIT", GLUT_BITMAP_TIMES_ROMAN_24);}

        return;
    }

    // Gameplay (k==1)
    if(k==1){
        new_game();
        handlePlayerInput();

        if (currentPowerUpIcon.isActive) {
            if (currentPowerUpIcon.type == FREEZE) iShowImage(currentPowerUpIcon.x, currentPowerUpIcon.y, "assets/images/Freez.png");
            else if (currentPowerUpIcon.type == SUPER_KICK) iShowImage(currentPowerUpIcon.x, currentPowerUpIcon.y, "assets/images/Superkick.png");
            else if (currentPowerUpIcon.type == SIZE_DOWN) iShowImage(currentPowerUpIcon.x, currentPowerUpIcon.y, "assets/images/shrink.png");
        }

        iShowImage(940, 540, "assets/images/pause_icon.png");

        if (isPaused) {
            iSetColor(0, 0, 0); // Solid black color
            iFilledRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

            iSetColor(200, 200, 200);
            iFilledRectangle(400, 350, 200, 50);
            iFilledRectangle(400, 280, 200, 50);
            iFilledRectangle(400, 210, 200, 50);

            iSetColor(0, 0, 0);
            iText(460, 368, "RESUME", GLUT_BITMAP_HELVETICA_18);
            iText(445, 298, "SAVE GAME", GLUT_BITMAP_HELVETICA_18);
            iText(430, 228, "BACK TO MENU", GLUT_BITMAP_HELVETICA_18);

            if (showSaveConfirmation) {
                iSetColor(0, 255, 0);
                iText(440, 420, "GAME SAVED!", GLUT_BITMAP_TIMES_ROMAN_24);
                saveConfirmTimer++;
                if (saveConfirmTimer > 100) {
                    showSaveConfirmation = false;
                    saveConfirmTimer = 0;
                }
            }
        }
    }
    // Settings (k==3)
    else if(k==3){
        draw_settings();
    }
    // Modes (k==4)
    else if(k==4){
        iShowImage(0, 40, "assets/images/mode_bg.png");
        iSetColor(255, 255, 255);
        iFilledRectangle(400, 350, 200, 50);
        iFilledRectangle(400, 280, 200, 50);
        iFilledRectangle(400, 210, 200, 50);

        iSetColor(0, 0, 0);
        iText(430, 368, "3 MINUTES", GLUT_BITMAP_HELVETICA_18);
        iText(430, 298, "FIRST TO 5", GLUT_BITMAP_HELVETICA_18);
        iText(470, 228, "BACK", GLUT_BITMAP_HELVETICA_18);

        if (mode_3min_selected) {
            iSetColor(0, 255, 0);
            iText(430, 368, "3 MINUTES", GLUT_BITMAP_HELVETICA_18);
        } else {
            iSetColor(0, 255, 0);
            iText(430, 298, "FIRST TO 5", GLUT_BITMAP_HELVETICA_18);
        }
    }
    drawGoalOverlay();

    // Display scores and time
    if (k == 1) {
        iSetColor(255, 255, 255);
        iText(360, 540, player1_name, GLUT_BITMAP_HELVETICA_18);
        char score_str[10];
        sprintf(score_str, "%d", player1_score);
        iText(450, 540, score_str, GLUT_BITMAP_HELVETICA_18);

        iText(560, 540, player2_name, GLUT_BITMAP_HELVETICA_18);
        sprintf(score_str, "%d", player2_score);
        iText(650, 540, score_str, GLUT_BITMAP_HELVETICA_18);

        iText(480, 510, timeText, GLUT_BITMAP_HELVETICA_18);
    }
}


void iMouseMove(int mx, int my) {
    if(k) return;

    m = (mx>=NEW_GAME_X && mx<=NEW_GAME_X+BTN_W && my>=NEW_GAME_Y && my<=NEW_GAME_Y+BTN_H);
    n = (mx>=CONTINUE_X && mx<=CONTINUE_X+BTN_W && my>=CONTINUE_Y && my<=CONTINUE_Y+BTN_H);
    x = (mx>=MODES_NEW_X && mx<=MODES_NEW_X+BTN_W && my>=MODES_NEW_Y && my<=MODES_NEW_Y+BTN_H);
    y = (mx>=HIST_X  && mx<=HIST_X+BTN_W  && my>=HIST_Y && my<=HIST_Y+BTN_H);
    z = (mx>=SET_X   && mx<=SET_X+BTN_W   && my>=SET_Y  && my<=SET_Y+BTN_H);
    q_quit_hover = (mx>=QUIT_X  && mx<=QUIT_X+QUIT_W  && my>=QUIT_Y && my<=QUIT_Y+QUIT_H);
}

void iMouseDrag(int mx, int my) {
    // No drag functionality needed at the moment
}


void iMouse(int button, int state, int mx, int my) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (!loadingStarted && !loadingDone) {
            loadingStarted = true;
            return;
        }

        if (k == 0) { // Main Menu
            // NEW GAME Button Click
            if (mx>=NEW_GAME_X && mx<=NEW_GAME_X+BTN_W && my>=NEW_GAME_Y && my<=NEW_GAME_Y+BTN_H) {
                saveGameExists = false;
                remove("savegame.txt");
                k = P1_NAME_INPUT;
                name_input_len = 0;
                name_input_str[0] = '\0';
                return;
            }

            // CONTINUE Button Click (only if save exists)
            if (saveGameExists && mx>=CONTINUE_X && mx<=CONTINUE_X+BTN_W && my>=CONTINUE_Y && my<=CONTINUE_Y+BTN_H) {
                loadGameState();
                resetField();
                k = 1;
                gameEnded = false;
                return;
            }

            // MODES Button Click
            if (mx>=MODES_NEW_X && mx<=MODES_NEW_X+BTN_W && my>=MODES_NEW_Y && my<=MODES_NEW_Y+BTN_H) {
                k=4;
                return;
            }

            // HISTORY Button Click
            if(mx>=HIST_X && mx<=HIST_X+BTN_W && my>=HIST_Y && my<=HIST_Y+BTN_H){
                k = HISTORY;
                return;
            }
            // SETTING Button Click
            if(mx>=SET_X && mx<=SET_X+BTN_W && my>=SET_Y && my<=SET_Y+BTN_H){
               k=3;
               return;
            }
            // QUIT Button Click
            if(mx>=QUIT_X && mx<=QUIT_X+QUIT_W && my>=QUIT_Y && my<=QUIT_Y+QUIT_H){
                exit(0);
            }
        }
        else if (k == 1) { // In-game
            if (mx >= 940 && mx <= 940 + 50 && my >= 540 && my <= 540 + 50) { // Pause icon
                isPaused = !isPaused;
                if (!isPaused) {
                    showSaveConfirmation = false;
                    iResumeTimer(0); // Resume gravity tick
                    iResumeTimer(1); // Resume ball movement tick
                    iResumeTimer(2); // Resume player update tick
                    iResumeTimer(3); // Resume power-up spawner tick
                    iResumeTimer(4); // Resume effects manager tick
                    iResumeTimer(5); // Resume watch tick
                    iIncreaseVolume(bgSoundIdx, 80);
                } else {
                    iPauseTimer(0); // Pause gravity tick
                    iPauseTimer(1); // Pause ball movement tick
                    iPauseTimer(2); // Pause player update tick
                    iPauseTimer(3); // Pause power-up spawner tick
                    iPauseTimer(4); // Pause effects manager tick
                    iPauseTimer(5); // Pause watch tick
                    iDecreaseVolume(bgSoundIdx, 80);
                }
            }
            if (isPaused) {
                // RESUME button
                if (mx >= 400 && mx <= 600 && my >= 350 && my <= 400) {
                    isPaused = false;
                    showSaveConfirmation = false;
                    iResumeTimer(0); iResumeTimer(1); iResumeTimer(2); iResumeTimer(3); iResumeTimer(4); iResumeTimer(5);
                    iIncreaseVolume(bgSoundIdx, 80);
                }
                // SAVE GAME button
                else if (mx >= 400 && mx <= 600 && my >= 280 && my <= 330) {
                    saveGameState();
                    saveGameExists = true;
                    showSaveConfirmation = true;
                    saveConfirmTimer = 0;
                }
                // BACK TO MENU button
                else if (mx >= 400 && mx <= 600 && my >= 210 && my <= 260) {
                    isPaused = false;
                    showSaveConfirmation = false;
                    k = 0; // Go back to main menu
                    iResumeTimer(0); iResumeTimer(1); iResumeTimer(2); iResumeTimer(3); iResumeTimer(4); iResumeTimer(5);
                    iIncreaseVolume(bgSoundIdx, 80);
                }
            }
        }
        else if (k == 3) { // Settings Menu
            if (mx >= BTN_BACK_X && mx <= BTN_BACK_X + BTN_BACK_W && my >= BTN_BACK_Y && my <= BTN_BACK_Y + BTN_BACK_H) {
                k = 0; // Go back to main menu
            }
            // BG Music controls
            if (mx >= BG_MUSIC_PLUS_BUTTON_X && mx <= BG_MUSIC_PLUS_BUTTON_X + 25 && my >= BG_MUSIC_PLUS_BUTTON_Y && my <= BG_MUSIC_PLUS_BUTTON_Y + 25) {
                iIncreaseVolume(bgSoundIdx, 20);
            } else if (mx >= BG_MUSIC_MINUS_BUTTON_X && mx <= BG_MUSIC_MINUS_BUTTON_X + 25 && my >= BG_MUSIC_MINUS_BUTTON_Y && my <= BG_MUSIC_MINUS_BUTTON_Y + 25) {
                iDecreaseVolume(bgSoundIdx, 20);
            }
            // Sound Effects controls
            else if (mx >= SOUND_MUSIC_PLUS_BUTTON_X && mx <= SOUND_MUSIC_PLUS_BUTTON_X + 25 && my >= SOUND_MUSIC_PLUS_BUTTON_Y && my <= SOUND_MUSIC_PLUS_BUTTON_Y + 25) {
                iIncreaseVolume(cheerSoundIdx, 20); // Using cheerSoundIdx as an example for sound effects
                iIncreaseVolume(kickSoundIdx, 20);
            } else if (mx >= SOUND_MUSIC_MINUS_BUTTON_X && mx <= SOUND_MUSIC_MINUS_BUTTON_X + 25 && my >= SOUND_MUSIC_MINUS_BUTTON_Y && my <= SOUND_MUSIC_MINUS_BUTTON_Y + 25) {
                iDecreaseVolume(cheerSoundIdx, 20);
                iDecreaseVolume(kickSoundIdx, 20);
            }
        }
        else if (k == 4) { // Modes Menu
            if (mx >= 400 && mx <= 600 && my >= 350 && my <= 400) { // 3 MINUTES button
                mode_3min_selected = true;
                mode_score_selected = false;
                k = 0; // Back to main menu
            } else if (mx >= 400 && mx <= 600 && my >= 280 && my <= 330) { // FIRST TO 5 button
                mode_3min_selected = false;
                mode_score_selected = true;
                k = 0; // Back to main menu
            } else if (mx >= 400 && mx <= 600 && my >= 210 && my <= 260) { // BACK button
                k = 0; // Back to main menu
            }
        }
        else if (k == 5 || k == HISTORY) { // Result Screen or History Screen
            if (mx >= 400 && mx <= 600 && my >= 50 && my <= 100) { // BACK button
                k = 0; // Back to main menu
                reset_game(); // Reset scores, ball, players etc.
                count_timer = 0;
                gameEnded = false;
                isPaused = false;
                saveGameExists = loadGameState(); // Check if a save exists after returning
            }
        }
    }
}

void iMouseWheel(int dir, int mx, int my) {
    // Scroll wheel functionality (not used in current logic)
}

void iKeyboard(unsigned char key) {
    if (k == P1_NAME_INPUT) {
        if (key == '\b') { // Backspace
            if (name_input_len > 0) {
                name_input_str[--name_input_len] = '\0';
            }
        } else if (key == '\r') { // Enter
            if (name_input_len > 0) {
                strcpy(player1_name, name_input_str);
                name_input_len = 0;
                name_input_str[0] = '\0';
                k = P2_NAME_INPUT; // Move to Player 2 name input
            }
        } else if (name_input_len < 99) { // Max 99 characters
            name_input_str[name_input_len++] = key;
            name_input_str[name_input_len] = '\0';
        }
    } else if (k == P2_NAME_INPUT) {
        if (key == '\b') { // Backspace
            if (name_input_len > 0) {
                name_input_str[--name_input_len] = '\0';
            }
        } else if (key == '\r') { // Enter
            if (name_input_len > 0) {
                strcpy(player2_name, name_input_str);
                name_input_len = 0;
                name_input_str[0] = '\0';
                k = 1; // Start game after name input
                reset_game(); // Initialize game state for new game
            }
        } else if (name_input_len < 99) { // Max 99 characters
            name_input_str[name_input_len++] = key;
            name_input_str[name_input_len] = '\0';
        }
    } else if (k == 1) {
        if (key == 'm') {
            k=0;
            iIncreaseVolume(bgSoundIdx, 80);
            mode_3min_selected = false;
            gameEnded = false;
        }
    }
}

void iSpecialKeyboard(unsigned char key) {
    if (key == GLUT_KEY_END) { exit(0); }
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    loadHistory();
    saveGameExists = loadGameState(); // Check if a save exists at startup
    iInitializeSound();
    bgSoundIdx = iPlaySound(BG_MUSIC_FILE, true);
    if(k==1)iDecreaseVolume(bgSoundIdx, 40);
    if(k!=1)iIncreaseVolume(bgSoundIdx, 40);

    pic1_x = 100;
    pic1_y = 62;
    pic2_x = 745;
    pic2_y = 62;
    pic_ballx = 475;
    pic_bally = 85;

    iSetTimer(16, move_ball);
    populate_player1_images();
    populate_player2_images();
    iSetTimer(16, gravityTick);
    iSetTimer(16, update_player);
    iSetTimer(5000, powerUpSpawner); // Power-up spawns every 5 seconds
    iSetTimer(100, effectsManager); // Effects tick every 100ms
    iSetTimer(1000, Watch);
    iSetTimer(20, loadingTick);
    iSetTimer(300, blink); // For blinking cursor and "Click to continue"

    iInitialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Football Game");
    loadresources();

    glutMainLoop();
    return 0;
}