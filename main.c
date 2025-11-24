#include "raylib.h"
#include <stdio.h>
#include <math.h>

int AnimationHandling(float dt, int currentFrameX, int *currentFrameY, float frameTime, int moving, int grounded, int gameState)
{
    static float frameTimer = 0;
    static int prevAnim = -1;
    int numFrames;
    
    if(moving != 0 && grounded == 1){
        numFrames = 8;
        *currentFrameY = 0;
    }
    else if(moving == 0 && grounded == 1){
        numFrames = 5;
        *currentFrameY = 1;
    }
    else{
        numFrames = 5;
        *currentFrameY = 2;
    }
    
    if(*currentFrameY != prevAnim){
        currentFrameX = 0;
        frameTimer = 0;
        prevAnim = *currentFrameY;
    }
    
    if(gameState == 0){
        frameTimer += dt;
        while(frameTimer >= frameTime){
            frameTimer -= frameTime;
            currentFrameX++;

            if(currentFrameX >= numFrames) currentFrameX = 0;
        }
    }
    
    return currentFrameX;
}


void LoadScore(int scores[])
{
    int i;
    FILE *scoreFile = fopen("scores.txt", "r");
    
    if(scoreFile==NULL)
    {
        for(i=0; i<5; i++)
        {
            scores[i]=0;
        }
        
        //create file as file currently doesn't exist
        scoreFile = fopen("scores.txt", "w");
        for(i=0; i<5; i++)
        {
            fprintf(scoreFile, "%d\n", scores[i]);
        }
        fclose(scoreFile);
        return;
    }
    
    for(i=0; i<5; i++)
    {
        fscanf(scoreFile, "%d", &scores[i]);
    }
    fclose(scoreFile);
}

void AddScore(int scores[], int currentScore)
{
    int i, j;
    
    for(i = 0; i < 5; i++)
    {
        if(currentScore > scores[i])
        {
            for(j = 4; j > i; j--)
            {
                scores[j] = scores[j-1];
            }
            scores[i] = currentScore;
            
            FILE *scoreFile = fopen("scores.txt", "w");
            if(scoreFile != NULL)
            {
                for(j = 0; j < 5; j++)
                {
                    fprintf(scoreFile, "%d\n", scores[j]);
                }
                fclose(scoreFile);
            }

            return; 
        }
    }
}


void hunterFollowDuck(Vector2 duck, Vector2 *hunter, float *followSpeed, float dt, int *canShoot, Sound shoot, int gameState, int *score)
{
    const float shootCooldown= 0.5f, trackTime=2.5f;
    static float timer=0, cooldown=0;
    if(gameState==0)
    {
        if(timer<trackTime)
        {
            Vector2 directionToDuck = {duck.x - hunter->x, duck.y - hunter->y};
            float distance = sqrt(directionToDuck.x*directionToDuck.x + directionToDuck.y*directionToDuck.y);
            
            if(distance != 0){
                directionToDuck.x /= distance;
                directionToDuck.y /= distance;
            }
            
            hunter->x += directionToDuck.x * (*followSpeed) * dt;
            hunter->y += directionToDuck.y * (*followSpeed) * dt;
            
            timer += dt;
        }
        else{
            if(cooldown<shootCooldown){
                if(*canShoot==0){
                    PlaySound(shoot);
                    *canShoot=1;
                } 
                cooldown += dt;
            }
            else{
                timer=0;
                cooldown=0;
                *canShoot=0;
                *score+= 1;
                *followSpeed+= 20;
            }
        }
    }
}

int main()
{
    //window properties and texture loading
    int windowLength= 1024, windowHeight= 800;
    Texture2D duck, hunter;
    Sound shooting, jumping, hurt;
    
    Color bgColour = {179, 230, 255, 255};
    
    InitAudioDevice();
    InitWindow(windowLength, windowHeight, "Reverse Duck Hunt");
    
    //duck properties
    Vector2 duckPos = {0.0f, 200.0f}, duckOrigin = {0.0f, 0.0f};
    float gravity= 1750.0f, jumpStrength= -700.0f, velocityY= 0.0f, duckSpeed=250.0f, spriteScale=7.75;
    int directionX= 0, spriteDimensions=124, facing=1, isGrounded=0;
    int duckboxSize= spriteDimensions/2;
    
    //hunter properties
    Vector2 hunterPos= {830.0f, 350.0f};
    float hunterScale= 9.25f, followSpeed=150.0f;
    int hunterDimensions= 148, canShoot=0;
    int hitboxSize = hunterDimensions/3.5;
    
    SetExitKey(KEY_ESCAPE);
    
    //texture assignment
    duck = LoadTexture("sprites/duck_animations.png");
    hunter = LoadTexture("sprites/crosshair.png");
    //sound assignment
    shooting= LoadSound("audio/explosion.wav");
    jumping= LoadSound("audio/jump.wav");
    hurt= LoadSound("audio/hitHurt.wav");
    
    //game states and scores
    int gameOver= 0, score=0;
    float postDeathTimer=0.0f;
    int highScores[5];
    LoadScore(highScores);
    
    //animation handling
    const float frameTime = 0.12f;
    int frameWidth = 16, frameHeight = 16, currentFrameX = 0, currentFrameY = 0;
    
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime();
        
        //centering the hitboxes
        float duckCenterX= duckPos.x + spriteDimensions/2, duckCenterY= duckPos.y + spriteDimensions/2;
        float centerX= hunterPos.x+hunterDimensions/2, centerY= hunterPos.y+hunterDimensions/2;
        
        //Rectangles 
        currentFrameX = AnimationHandling(dt, currentFrameX, &currentFrameY, frameTime, directionX, isGrounded, gameOver);
      
        Rectangle src= {16 * currentFrameX, 16 * currentFrameY, frameWidth * facing, frameHeight};//for flipping the sprite
        Rectangle dest= {duckPos.x, duckPos.y, spriteDimensions, spriteDimensions};//for sprite display ONLY
        Rectangle duckBox= {duckCenterX - duckboxSize/2, duckCenterY - duckboxSize/2, duckboxSize, duckboxSize};//for collisions 
        Rectangle hunterCollision = {centerX - hitboxSize/2, centerY - hitboxSize/2, hitboxSize, hitboxSize};
        
        //hunter logic
        hunterFollowDuck(duckPos, &hunterPos, &followSpeed, dt, &canShoot, shooting, gameOver, &score);
        
        if(CheckCollisionRecs(duckBox, hunterCollision) && canShoot==1 && gameOver==0){
            gameOver=1;
            PlaySound(hurt);
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
                isGrounded=1;
            }

            //jump
            if (IsKeyPressed(KEY_UP) && duckPos.y <= windowHeight-spriteDimensions && duckPos.y >= 0){
                velocityY = jumpStrength;
                PlaySound(jumping);
                isGrounded=0;
            }
        }
        else if(gameOver==1 && postDeathTimer<1.5){
            if(followSpeed!=0){
                followSpeed = 0;
                AddScore(highScores, score);
            }
            
            postDeathTimer+= dt;
        }
        
        //drawing
        
        BeginDrawing();//put all things that need to be shown on the screen in here
            ClearBackground(bgColour);
            
            if(gameOver==0 || postDeathTimer<1.5)
            {
                DrawTexturePro(duck, src, dest, duckOrigin,0.0f, WHITE);
                DrawTextureEx(hunter, hunterPos, 0.0f, hunterScale, WHITE);
                DrawText(TextFormat("Score: %d", score), 10, 10, 30, DARKGRAY);
                if(canShoot==1){
                  DrawRectangleRec(hunterCollision, RED);  
                }
            }
            else
            {
                int scoreSpacing = 30, numScores = 5, startY = windowHeight/2 - 35; 

                // draw top 5 high scores
                DrawText("GAME OVER!", windowLength/2 - 160, windowHeight/2 - 130, 40, RED);
                DrawText("HIGH SCORES:", windowLength/2 - 130, startY - 40, 25, DARKGRAY);

                for(int i=0; i<numScores; i++)
                {
                    DrawText(TextFormat("%d", highScores[i]), windowLength/2 - 50, startY + i * scoreSpacing, 20, DARKBLUE);
                }
                
                DrawText("Press ESC to quit", windowLength/2 - 135,  startY + numScores * scoreSpacing + 10,  20, DARKGRAY);
            }
        EndDrawing();
    }
    
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}
 