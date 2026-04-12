
#include "raylib.h"
#include <math.h>
#define screenWidth 1000
#define screenHeight 440
 int movespeed = 120;
int match = 0;
// game will over after 3 match loss
int matchCount = 0;
int score = 0;
int ball_speed_info=0; //player_speed_not_inc_till_scrore
float timepassed = 0.0f; // will be used to determine total game play & start scene
// thrower & lander is player
Rectangle player;
// ball implementation
typedef struct
{
    Vector2 center;
    float radius;
    Vector2 speed;
} Ball;
// ball with initial position
Ball ball;
typedef enum
{
    LOADING,
    PAUSE,
    GAME,
    GAMEOVER,
    START,
    MENU,
    SHOP
} Scene;
const int brick_width = 40,
          brick_height = 40;
typedef struct
{
    Rectangle position;
    bool active;
} Bricks;
const int bricks_rows = 5;
const int bricks_offset = 7;
const int bricks_cols = screenWidth / (brick_height + bricks_offset);
Scene scene = START; // set it start later
int main(void)
{

    InitWindow(screenWidth, screenHeight, "Arknoid - Classic");
    InitAudioDevice(); 
    Music music = LoadMusicStream("music.wav");
    Sound score_sound=LoadSound("score.wav");
    Sound through_sound=LoadSound("through.wav");
    Sound bounce_sound=LoadSound("bounce.wav"),
          restart_game_sound=LoadSound("restart-game.wav"),
          gover_sound=LoadSound("over.wav");
    PlayMusicStream(music);
    //game setting
    bool playMusic = true;
    
    // initialize
    Rectangle pause_button = {
        .x=0,
        .y=screenHeight-50,
        .width=40,
        .height=60
    };
    Bricks bricks[bricks_rows][bricks_cols];
    for (int i = 0; i < bricks_rows; i++) // row
    {
        for (int j = 0; j < bricks_cols; j++) // col
        {

            bricks[i][j].active = true;
            bricks[i][j].position = (Rectangle){
                j * (brick_height + bricks_offset),
                40+i * (brick_width + bricks_offset), //top title bar space 
                brick_width,
                brick_height
            };
        }
    }
    // player
    player.x = screenWidth / 2 - 50;
    player.y = screenHeight - 20;
    player.width = 100;
    player.height = 12;
    // ball
    ball.radius = 10.0f;
    ball.center = (Vector2){
        .x = screenWidth / 2,
        .y = player.y - ball.radius};
    bool matchStarted = false;
    // not move untill key pressed/mouse click
    ball.speed.x = 0;
    ball.speed.y = 0;
    SetTargetFPS(60);
   
    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update music stream
        UpdateMusicStream(music);
   
        // Update
        /*game scene logic*/
        /*========================================================================================*/
        if (scene == GAME)
        {
            // ball collison with player if match not over
            if (matchStarted && matchCount <= 3)
            {

                if(CheckCollisionPointRec(GetMousePosition(),(Rectangle){.x=0,
                                                                        .y=0,
                                                                        .width=screenWidth,
                                                                        .height=30 }))
                  {
                    scene=PAUSE;
                  }                                                 
                // bricks collision
                // rows

                for (int i = 0; i < bricks_rows; i++)
                {
                    for (int j = 0; j < bricks_cols; j++)
                    {
                        if (bricks[i][j].active)
                        {
                            if (CheckCollisionCircleRec(ball.center, ball.radius, bricks[i][j].position))
                            {
                                bricks[i][j].active = false;
                                score += 10;
                                PlaySound(score_sound);
                                ball_speed_info++;
                                if (ball_speed_info==10)
                                {
                                    ball.speed.x+=1; //10 %speed increase
                                    ball.speed.y+=1;
                                    ball_speed_info=0;
                                    movespeed++;
                                    
                                }
                                
                            }
                        }
                    }
                }
                if (CheckCollisionCircleRec(ball.center, ball.radius, player))

                {
                    PlaySound(bounce_sound);
                    float hitPos = (ball.center.x - player.x) / player.width; // 0.0 = left, 1.0 = right
                    float angle = (hitPos - 0.5f) * 120.0f;                   // -60° (left) to +60° (right)
                    float speed = 200.0f;                                     // constant ball speed
                    ball.speed.x = speed * sinf(angle * DEG2RAD);
                    ball.speed.y = -speed * cosf(angle * DEG2RAD); // always upward
                }
                // ball collision with screen
                // top screen
                else if (ball.center.y - ball.radius < 40) //40,not to collide withe title bar
                {
                    ball.speed.y *= -1;
                    PlaySound(bounce_sound);
                }
                // bottom screen and game over
                else if (ball.center.y - ball.radius > screenHeight)
                {
                    player.x = screenWidth / 2 - 50;
                    player.y = screenHeight - 20;
                    ball.speed = (Vector2){0, 0};
                    ball.center = (Vector2){
                        .x = screenWidth / 2,
                        .y = player.y - ball.radius};
                    matchCount--;
                    matchStarted = false;
                    scene = GAMEOVER;
                }
                else if (ball.center.x + ball.radius > screenWidth || ball.center.x - ball.radius < 0)
                {
                    ball.speed.x *= -1;
                    PlaySound(bounce_sound);
                }
                // player with screen
                if (player.x + player.width > screenWidth)
                {
                    player.x = screenWidth - player.width;
                    PlaySound(bounce_sound);
                }
                else if (player.x < 0)
                {
                    player.x = 0;
                }
                // moving player using keyboard arrow keys
                if (IsKeyDown(KEY_LEFT))
                {
                    player.x -= movespeed * GetFrameTime();
                }
                else if (IsKeyDown(KEY_RIGHT))
                {
                    player.x += movespeed * GetFrameTime();
                    // moving player using mouse wheel for easier control
                }
                else if (GetMouseWheelMove())
                {
                    player.x += GetMouseWheelMove() * 20;
                    // clicking left move the player left and right move left;for android
                }
                else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
                {
                    if (GetMouseX() > screenWidth / 2)
                    {
                        player.x += movespeed * GetFrameTime();
                    }
                    else if (GetMouseX() < screenWidth / 2)
                    {
                        player.x -= movespeed * GetFrameTime();
                    }
                }
            }
            /*game over scene logic + enter logic + match over/game over selector*/
            /*===========================================================================================================*/
            // ball through if throughable[game over/start]
            if (!matchStarted && matchCount <= 3)
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))
                {
                    PlaySound(through_sound);
                    ball.speed = (Vector2){0, -120}; // no move to x by default && (-) means upward
                    matchStarted = true;
                    matchCount++;
                }
            }
            else if (matchCount > 3)
            {
                scene = GAMEOVER;
            }
            /*setting scene setup + entering logic
        ============================================================================================================================*/

            /*menu + exit logic
        ============================================================================================================================*/

            // ball move
            ball.center.x += ball.speed.x * GetFrameTime();
            ball.center.y += ball.speed.y * GetFrameTime();
        }
        
        // Draw

        BeginDrawing();
        /*game scene drawing
        ================================================================================================================*/
        if (scene == GAME)
        {
            
            ClearBackground(WHITE);
            // player
            DrawRectangleRec(player, BLACK);
            // pause button
            DrawText("Press Left to move left, Press right to move right,Press Here to Pause",100,10,20,GRAY);
            DrawFPS(10, 10);
            DrawText(TextFormat("Score: %d", score), screenWidth-130,10, 20, BLACK);
 
            // ball
            DrawCircleV(ball.center, ball.radius, RED);
            // draw bricks
            for (int i = 0; i < bricks_rows; i++)
            {
                for (int j = 0; j < bricks_cols; j++)
                {
                    if (bricks[i][j].active)
                    {
                        DrawRectangleRec(bricks[i][j].position, BLUE);
                    }
                }
            }
        }
        /*start scene
        ============================================================================================================================*/
        else if (scene == START)
        {
            timepassed += GetFrameTime();
            ClearBackground(BLACK);
            DrawText("Limon Das Presents", screenWidth / 2 - 100, screenHeight / 2 - 100, 25, GREEN);

            if (timepassed > 1)
            {
               
                scene = GAME;
            }
        }
         /*gameover scene
        ============================================================================================================================*/

        else if (scene == GAMEOVER)
        {
            StopMusicStream(music);
            
            ClearBackground(WHITE);
                for (int i = 0; i < bricks_rows; i++) // row
    {
        for (int j = 0; j < bricks_cols; j++) // col
        {

            bricks[i][j].active = true;
            bricks[i][j].position = (Rectangle){
                j * (brick_height + bricks_offset),
                40+i * (brick_width + bricks_offset), //top title bar space 
                brick_width,
                brick_height
            };
        }
    }

            DrawText("GAME OVER", screenWidth / 2-140 , screenHeight / 2 -50, 50, RED);
            DrawText(TextFormat("Score=%d",score), screenWidth / 2 -20, screenHeight / 2 , 20, RED);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE))
            {
                PlaySound(restart_game_sound);
                PlayMusicStream(music);

                ClearBackground(GRAY);
                scene = GAME;
                matchCount = 0;
            }
        }
         /*pause scene draw + exit logic
        ============================================================================================================================*/

        else if (scene == PAUSE)
        {
            ClearBackground(WHITE);

            DrawText("Paused", screenWidth / 2 - 100, screenHeight / 2 - 100, 100, RED);
            if ( IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)|| IsKeyPressed(KEY_P))
            {

                ClearBackground(GRAY);
                scene = GAME;
                matchCount = 0;
            }
        }
        /*menu scene draw + exit logic
        ============================================================================================================================*/
        else if(scene==MENU){

        }
        /*settings scene draw + exit logic
        ============================================================================================================================*/

        EndDrawing();
    }

    // De-Initialization
    UnloadSound(gover_sound);
    UnloadSound(restart_game_sound);
    UnloadSound(bounce_sound);
    UnloadSound(through_sound);
    UnloadMusicStream(music); 
    CloseAudioDevice(); // Close audio device
    CloseWindow(); // Close window and OpenGL context
    return 0;
}
