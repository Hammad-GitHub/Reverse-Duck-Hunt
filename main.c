#include "raylib.h"
#include <stdio.h>

int main()
{
    int windowLength= 1024, windowHeight= 800;
    float gravity= 1750.0f, jumpStrength= -700.0f, velocityY= 0.0f, duckSpeed=250.0f, spriteScale=7.75;
    int directionX= 0, spriteDimensions=124, facing=1;
    
    Texture2D duck;
    
    InitWindow(windowLength, windowHeight, "Reverse Duck Hunt");
    SetExitKey(KEY_ESCAPE);
    
    duck = LoadTexture("duck.png");
    Vector2 duckPos = {0.0f, 200.0f}, origin = {0.0f, 0.0f};
    

    
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        //updates:
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

        //drawing
        BeginDrawing();//put all things that need to be shown on the screen in here
            ClearBackground(WHITE);
            
            Rectangle src = { 0, 0, duck.width * facing, duck.height };
            Rectangle dest = { duckPos.x, duckPos.y, duck.width * spriteScale, duck.height * spriteScale };

            DrawTexturePro(duck, src, dest, origin,0.0f, WHITE);
            
        EndDrawing();
    }
    
    return 0;
}
 
