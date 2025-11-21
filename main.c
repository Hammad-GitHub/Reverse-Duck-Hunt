#include "raylib.h"
#include <stdio.h>

int main()
{
    int windowLength= 1024, windowHeight= 800;
    float gravity= 1250.0f, jumpStrength= -700.0f, velocityY= 0.0f, duckSpeed=250.0f;
    int directionX= 0;
    
    Texture2D duck;
    
    InitWindow(windowLength, windowHeight, "Reverse Duck Hunt");
    
    duck = LoadTexture("duck.png");
    Vector2 duckPos = {0.0f, 200.0f};
    
    SetTargetFPS(60);
    
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        //updates:
        //x direction input
        if(IsKeyDown(KEY_RIGHT)){
            directionX= 1;
        }
        else if(IsKeyDown(KEY_LEFT)){
            directionX= -1;
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
        if (duckPos.y > windowHeight - 140) {
            duckPos.y = windowHeight - 140;
            velocityY = 0;
        }

        //jump
        if (IsKeyPressed(KEY_UP) && duckPos.y <= windowHeight - 140) {
            velocityY = jumpStrength;
        }

        //drawing
        BeginDrawing();//put all things that need to be shown on the screen in here
            ClearBackground(WHITE);
            DrawTextureEx(duck, duckPos, 0.0f, 8.75f,  WHITE);
            
        EndDrawing();
    }
    
    return 0;
}
 