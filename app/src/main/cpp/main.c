
#include "raylib.h"
#include<math.h>
const int screenWidth = 800;
const int screenHeight = 360;
const int movespeed=120;
int match=0;
//game will over after 3 match loss 
int matchCount=0;
float timepassed=0.0f; //will be used to determine total game play & start scene
//thrower & lander is player
Rectangle player;
//ball implementation
typedef struct{
    Vector2 center;
    float radius;
    Vector2 speed;
}Ball;
//ball with initial position
Ball ball;
typedef enum{
    LOADING,
    GAME,
    GAMEOVER,
    START,
    MENU,
    SHOP
} Scene;
Scene scene=START; //set it start later
int main(void)
{
    
    InitWindow(screenWidth, screenHeight, "Break The Brick");
    
    //initialize
    //player
    player.x=screenWidth/2-50;
    player.y=screenHeight-20;
    player.width=100;
    player.height=12;
    //ball
    ball.radius=10.0f;
    ball.center=(Vector2){
    .x=screenWidth/2,
    .y=player.y-ball.radius
};
    bool matchStarted=false;
    //not move untill key pressed/mouse click
    ball.speed.x=0; 
    ball.speed.y=0; 
    SetTargetFPS(60);          
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
    if(scene==GAME){
        //ball collison with player
    if (matchStarted && matchCount<=3){
        if (CheckCollisionCircleRec(ball.center, ball.radius, player) ){
            float hitPos = (ball.center.x - player.x) / player.width; // 0.0 = left, 1.0 = right
            float angle = (hitPos - 0.5f) * 120.0f; // -60° (left) to +60° (right)
            float speed = 200.0f; // constant ball speed
            ball.speed.x = speed * sinf(angle * DEG2RAD);
            ball.speed.y = -speed * cosf(angle * DEG2RAD); // always upward
        }
        //ball collision with screen
        //top screen
        else if(ball.center.y-ball.radius<0){
            ball.speed.y*=-1;
        }
        //bottom screen and game over
        else if(ball.center.y-ball.radius>screenHeight){ 
            player.x=screenWidth/2-50;
            player.y=screenHeight-20;
            ball.speed=(Vector2){0,0};
            ball.center=(Vector2){
                    .x=screenWidth/2,
                    .y=player.y-ball.radius
            };
            matchCount--;
            matchStarted=false;
        } else if (ball.center.x+ball.radius>screenWidth || ball.center.x-ball.radius<0){
            ball.speed.x*=-1;
        }         
        //player with screen
        if(player.x+player.width>screenWidth){
            player.x=screenWidth-player.width;
        }else  if(player.x<0){
            player.x=0;
        }
        //moving player using keyboard arrow keys
        if(IsKeyDown(KEY_LEFT)){
            player.x-=movespeed*GetFrameTime();
        } 
        else if(IsKeyDown(KEY_RIGHT)){
            player.x+=movespeed*GetFrameTime();
        //moving player using mouse wheel for easier control
        }else if(GetMouseWheelMove()){
            player.x+=GetMouseWheelMove()*20;
        //clicking left move the player left and right move left;for android    
        }else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            if(GetMouseX()>screenWidth/2){
                player.x+=movespeed*GetFrameTime();
            }else if(GetMouseX()<screenWidth/2){
                player.x-=movespeed*GetFrameTime();
            }
        }
    }
    //ball through if throughable[game over/start]
    if(!matchStarted && matchCount<=3){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)){
            ball.speed=(Vector2){0,-120};//no move to x by default && (-) means upward
            matchStarted=true;
            matchCount++;
            }
    }else if(matchCount>3){
        scene=GAMEOVER;
    }
        //ball move
        ball.center.x+=ball.speed.x*GetFrameTime();       
        ball.center.y+=ball.speed.y*GetFrameTime();
    }
        // Draw
        DrawFPS(0,0);
        BeginDrawing();
        if(scene==GAME){
            ClearBackground(WHITE);
            //player
            DrawRectangleRec(player,BLACK);
            //ball
            DrawCircleV(ball.center,ball.radius,RED);
        }else if(scene==START){
            timepassed+=GetFrameTime();
            ClearBackground(WHITE);
            DrawText("Limon Das Presents",screenWidth/2-100,screenHeight/2-100,25,BLACK);
            if(timepassed>1){
                scene=GAME;
            }
        }else if (scene==GAMEOVER){
            DrawText("GAME OVER",120,120,30,RED);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_SPACE)){
                ClearBackground(GRAY);
                scene=GAME;
                matchCount=0;
            }
            
        }
        
        EndDrawing();
    }

    // De-Initialization
    CloseWindow();        // Close window and OpenGL context
    return 0;
}
