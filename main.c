#include "raylib.h"
#include <stdio.h>
#include <math.h>

void hunterFollowDuck(Vector2 duck, Vector2 *hunter, float followSpeed, float dt)
{
    Vector2 directionToDuck = {duck.x - hunter->x, duck.y - hunter->y};
    float distance = sqrt(directionToDuck.x*directionToDuck.x + directionToDuck.y*directionToDuck.y);
    
    if(distance != 0){
        directionToDuck.x /= distance;
        directionToDuck.y /= distance;
    }
    
    hunter->x += directionToDuck.x * followSpeed * dt;
    hunter->y += directionToDuck.y * followSpeed * dt;
}

int main()
{
    //window properties and texture loading
    int windowLength= 1024, windowHeight= 800;
    Texture2D duck, hunter;
    InitWindow(windowLength, windowHeight, "Reverse Duck Hunt");
    
    //duck properties
    Vector2 duckPos = {0.0f, 200.0f}, duckOrigin = {0.0f, 0.0f};
    float gravity= 1750.0f, jumpStrength= -700.0f, velocityY= 0.0f, duckSpeed=250.0f, spriteScale=7.75;
    int directionX= 0, spriteDimensions=124, facing=1;
    int duckboxSize= spriteDimensions/2;
    
    //hunter properties
    Vector2 hunterPos= {530.0f, 450.0f};
    float hunterScale= 9.25f, followSpeed=250.0f;
    int hunterDimensions= 148;
    int hitboxSize = hunterDimensions/3.25;
    
    
    
    SetExitKey(KEY_ESCAPE);
    
    duck = LoadTexture("duck.png");
    hunter = LoadTexture("crosshair.png");
    
    
    int gameOver= 0;
    float postDeathTimer=0.0f;

    
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        
        //centering the hitboxes
        float duckCenterX= duckPos.x + spriteDimensions/2, duckCenterY= duckPos.y + spriteDimensions/2;
        float centerX= hunterPos.x+hunterDimensions/2, centerY= hunterPos.y+hunterDimensions/2;
        
        //Rectangles 
        Rectangle src= {0, 0, duck.width * facing, duck.height};//for flipping the sprite
        Rectangle dest= {duckPos.x, duckPos.y, spriteDimensions, spriteDimensions};//for sprite display ONLY
        Rectangle duckBox= {duckCenterX - duckboxSize/2, duckCenterY - duckboxSize/2, duckboxSize, duckboxSize};//for collisions 
        Rectangle hunterCollision = {centerX - hitboxSize/2, centerY - hitboxSize/2, hitboxSize, hitboxSize};
        
        hunterFollowDuck(duckPos, &hunterPos, followSpeed, dt);
        
        if(CheckCollisionRecs(duckBox, hunterCollision) && gameOver==0){
            gameOver=1;
        }
        
        if(gameOver==0)
        {
            //x direction input
            if(IsKeyDown(KEY_RIGHT) && duckPos.x<windowLength-spriteDimensions){
                directionX= 1;
                facing= 1;
            }
            else if(IsKeyDown(KEY_LEFT)&& duckPos.x>0){
                directionX= -1;
                facing= -1;
            }
            else{
                directionX= 0;
            }
            
            //x movement
            duckPos.x += duckSpeed * dt * directionX;
            
            
            //gravity 
            velocityY += gravity * dt;
            duckPos.y += velocityY * dt;

            //ground check
            if (duckPos.y > windowHeight-spriteDimensions) {
                duckPos.y = windowHeight-spriteDimensions;
                velocityY = 0;
            }

            //jump
            if (IsKeyPressed(KEY_UP) && duckPos.y <= windowHeight-spriteDimensions) {
                velocityY = jumpStrength;
            }
        }
        else if(gameOver==1 && postDeathTimer<1){
            followSpeed = 0;
            postDeathTimer+= dt;
        }
        
        //drawing
        
        BeginDrawing();//put all things that need to be shown on the screen in here
            ClearBackground(WHITE);
            
            if(gameOver==0 || postDeathTimer<1)
            {
                DrawTexturePro(duck, src, dest, duckOrigin,0.0f, WHITE);
                DrawTextureEx(hunter, hunterPos, 0.0f, hunterScale, WHITE);
                //DrawRectangleRec(duckBox, BLUE);
                //DrawRectangleRec(hunterCollision, RED);    hitbox visualization
            }
            else
            {
                DrawText("GAME OVER!", windowLength/2 - 120, windowHeight/2 - 40, 40, RED);
                DrawText("Press ESC to quit", windowLength/2 - 100, windowHeight/2 + 50, 20, DARKGRAY);
            }
        EndDrawing();
    }
    
    return 0;
}
 