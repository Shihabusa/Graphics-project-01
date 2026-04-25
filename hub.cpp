#include <GL/glut.h>
#include <string>
#include <vector>
#include <iostream>
#include <deque>
#include <cmath>

const int SCREEN_W = 800;
const int SCREEN_H = 600;

enum GameState { MENU, SNAKE_GAME, CAR_GAME, SPACE_GAME };
GameState currentState = MENU;

void drawText(const char* text, int length, int x, int y) {
    glRasterPos2i(x, y);
    for (int i = 0; i < length; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)text[i]);
}

// ==========================================
//           SNAKE GAME (SPEED ADJUSTED)
// ==========================================
struct Point { int x, y; };
std::deque<Point> snake;
Point food;
int snakeDir = 2; 
bool snakeGameOver = false;
int snakeScore = 0;
float foodPulse = 0;

void initSnake() {
    snake.clear();
    snake.push_back({10, 10}); snake.push_back({9, 10}); snake.push_back({8, 10});
    snakeDir = 2; snakeScore = 0; snakeGameOver = false;
    food.x = rand() % 38 + 1; food.y = rand() % 28 + 2;
}

void drawSnakeScreen() {
    glClearColor(0.02f, 0.05f, 0.02f, 1.0f);
    glColor3f(0.0f, 0.8f, 1.0f); glLineWidth(5.0f); // Neon Blue Border
    glBegin(GL_LINE_LOOP);
        glVertex2i(5, 5); glVertex2i(795, 5); glVertex2i(795, 595); glVertex2i(5, 595);
    glEnd();

    if (snakeGameOver) {
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText("GAME OVER! Press SPACE to Restart", 34, 250, 300);
    } else {
        foodPulse += 0.15f;
        float s = 3.0f * sin(foodPulse);
        glColor3f(1.0f, 0.0f, 0.0f);
        glRectf(food.x*20 - s, food.y*20 - s, food.x*20 + 20 + s, food.y*20 + 20 + s);

        for (size_t i = 0; i < snake.size(); i++) {
            if (i == 0) {
                glColor3f(1.0f, 1.0f, 0.0f);
                glRectf(snake[i].x*20, snake[i].y*20, snake[i].x*20+20, snake[i].y*20+20);
                glColor3f(0, 0, 0); // Eyes
                if(snakeDir==2 || snakeDir==4) {
                    glRectf(snake[i].x*20+12, snake[i].y*20+4, snake[i].x*20+16, snake[i].y*20+8);
                    glRectf(snake[i].x*20+12, snake[i].y*20+12, snake[i].x*20+16, snake[i].y*20+16);
                } else {
                    glRectf(snake[i].x*20+4, snake[i].y*20+12, snake[i].x*20+8, snake[i].y*20+16);
                    glRectf(snake[i].x*20+12, snake[i].y*20+12, snake[i].x*20+16, snake[i].y*20+16);
                }
            } else {
                glColor3f(1.0f, 1.0f, 1.0f); 
                glRectf(snake[i].x*20+1, snake[i].y*20+1, snake[i].x*20+19, snake[i].y*20+19);
            }
        }
    }
    glColor3f(1, 1, 1);
    std::string info = "Score: " + std::to_string(snakeScore) + " | Length: " + std::to_string(snake.size());
    drawText(info.c_str(), info.length(), 20, 30);
}

// ==========================================
//           CAR GAME (ENHANCED GRAPHICS)
// ==========================================
int currentLane = 1;
float laneCenters[4] = {100, 300, 500, 700};
struct Entity { float x, y, speed; bool active; };
std::vector<Entity> enemies;
std::vector<Entity> coins;
int carLives = 3, carScore = 0;
bool carGameOver = false;
float roadOffset = 0;

void initCarGame() {
    currentLane = 1; carLives = 3; carScore = 0; carGameOver = false;
    enemies.clear(); coins.clear();
    enemies.push_back({laneCenters[rand()%4]-25, -100, 10.0f, true});
}

void drawDetailedCar(float x, float y, bool isPlayer) {
    // Wheels with alloy effect
    glColor3f(0.1f, 0.1f, 0.1f);
    glRectf(x-6, y+10, x, y+35); glRectf(x+50, y+10, x+56, y+35);
    glRectf(x-6, y+60, x, y+85); glRectf(x+50, y+60, x+56, y+85);
    
    // Main Body
    if(isPlayer) glColor3f(0.0f, 0.4f, 0.9f); else glColor3f(0.8f, 0.0f, 0.1f);
    glRectf(x, y, x+50, y+90);
    
    // Detailed Hood / Grill
    glColor3f(0.1f, 0.1f, 0.1f);
    glRectf(x+10, y+2, x+40, y+8); 
    
    // Windshield (Shaded)
    glColor3f(0.2f, 0.2f, 0.2f); glRectf(x+5, y+22, x+45, y+45);
    glColor3f(0.5f, 0.7f, 1.0f); glRectf(x+8, y+25, x+42, y+35); 

    // Spoiler (Rear)
    if(isPlayer) glColor3f(0, 0, 0); else glColor3f(0.5, 0, 0);
    glRectf(x-5, y+85, x+55, y+92);
}

void drawCarGameScreen() {
    // Grass
    glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Road Surface
    glColor3f(0.2f, 0.2f, 0.2f);
    glRectf(50, 0, 750, 600);

    // Road Lines
    glColor3f(1, 1, 1);
    for(int i=-100; i<SCREEN_H; i+=100) {
        glRectf(198, i+roadOffset, 202, i+60+roadOffset);
        glRectf(398, i+roadOffset, 402, i+60+roadOffset);
        glRectf(598, i+roadOffset, 602, i+60+roadOffset);
    }
    
    if(carGameOver) {
        glColor3f(1, 1, 1);
        drawText("OUT OF LIVES! Press SPACE to Restart", 36, 240, 300);
    } else {
        // Coin Drawing
        for(auto &c : coins) if(c.active) {
            glColor3f(1, 0.9f, 0); glBegin(GL_POLYGON);
            for(int i=0; i<360; i+=45) glVertex2f(c.x+18*cos(i*3.14/180), c.y+18*sin(i*3.14/180));
            glEnd();
            glColor3f(1, 1, 1); drawText("$", 1, c.x-5, c.y+5);
        }
        // Player & Enemies
        drawDetailedCar(laneCenters[currentLane]-25, 480, true);
        for(auto &e : enemies) if(e.active) drawDetailedCar(e.x, e.y, false);
    }
    // HUD Header
    glColor3f(0, 0, 0); glRectf(0, 0, 800, 45);
    glColor3f(1, 1, 1);
    std::string hud = "Score: " + std::to_string(carScore) + " | Lives: " + std::to_string(carLives);
    drawText(hud.c_str(), hud.length(), 20, 30);
}
// ==========================================
//           SPACE SHOOTER (DETAILED)
// ==========================================

struct Bullet{
 float x,y;
 bool active;
};

struct EnemyShip{
 float x,y;
 float speed;
 bool active;
};

struct Star{
 float x,y;
 float speed;
};

std::vector<Bullet> bullets;
std::vector<EnemyShip> alienEnemies;
std::vector<Star> stars;

float playerShipX=400;
int spaceLives=3;
int spaceScore=0;
bool spaceGameOver=false;
float nebulaShift=0;

void initSpaceGame(){
 bullets.clear();
 alienEnemies.clear();
 stars.clear();

 playerShipX=400;
 spaceLives=3;
 spaceScore=0;
 spaceGameOver=false;

 for(int i=0;i<140;i++){
   stars.push_back({
    float(rand()%800),
    float(rand()%600),
    float(1+(rand()%4))
   });
 }
}

void drawPlayerShip(float x,float y){

 // engine glow
 glColor3f(1.0,0.5,0.0);
 glBegin(GL_TRIANGLES);
 glVertex2f(x-8,y+42);
 glVertex2f(x,y+58);
 glVertex2f(x+8,y+42);
 glEnd();

 // body
 glColor3f(0.2,0.8,1.0);
 glBegin(GL_POLYGON);
 glVertex2f(x,y);
 glVertex2f(x-18,y+18);
 glVertex2f(x-14,y+42);
 glVertex2f(x+14,y+42);
 glVertex2f(x+18,y+18);
 glEnd();

 // wings
 glColor3f(0.7,0.9,1);
 glBegin(GL_TRIANGLES);
 glVertex2f(x-18,y+24);
 glVertex2f(x-34,y+40);
 glVertex2f(x-10,y+35);
 glEnd();

 glBegin(GL_TRIANGLES);
 glVertex2f(x+18,y+24);
 glVertex2f(x+34,y+40);
 glVertex2f(x+10,y+35);
 glEnd();

 // cockpit
 glColor3f(0.1,0.1,0.7);
 glRectf(x-5,y+10,x+5,y+20);
}
void drawEnemyShip(float x,float y){
 glColor3f(0.9,0.1,0.2);
 glRectf(x-18,y-8,x+18,y+8);

 glColor3f(1,0.6,0.2);
 glRectf(x-8,y-14,x+8,y+14);

 glColor3f(0.7,0,0);
 glRectf(x-28,y-4,x-18,y+4);
 glRectf(x+18,y-4,x+28,y+4);
}

void drawSpaceGameScreen(){

 glClearColor(0,0,0.08,1);
 glClear(GL_COLOR_BUFFER_BIT);

 // animated nebula layers
 nebulaShift +=0.2;
 glColor3f(0.08,0.02,0.2);
 glRectf(0,40*sin(nebulaShift/30),800,220);

 glColor3f(0.03,0.08,0.25);
 glRectf(0,260+30*cos(nebulaShift/25),800,420);

 // stars
 for(auto &s:stars){
   glColor3f(1,1,1);
   glRectf(s.x,s.y,s.x+2,s.y+2);
 }

 if(spaceGameOver){
   glColor3f(1,1,1);
   drawText("SPACE GAME OVER - PRESS SPACE",28,250,300);
   return;
 }

 // player
 drawPlayerShip(playerShipX,500);

 // bullets
 for(auto &b:bullets)
 if(b.active){
   glColor3f(1,1,0);
   glRectf(b.x-2,b.y,b.x+2,b.y+14);

   // glow ring
   glColor3f(1,0.8,0.2);
   glRectf(b.x-4,b.y+4,b.x+4,b.y+8);
 }

 // enemies
 for(auto &e:alienEnemies)
 if(e.active)
   drawEnemyShip(e.x,e.y);

 // HUD
 glColor3f(0,0,0);
 glRectf(0,0,800,40);

 glColor3f(1,1,1);
 std::string hud = "Kills: " + std::to_string(spaceScore)
  + "  Lives: " + std::to_string(spaceLives);

 drawText(hud.c_str(),hud.length(),20,25);
}

// ==========================================
//           GAME TIMER (FPS & SPEED)
// ==========================================
int frameCount = 0;
void gameTimer(int v) {
    frameCount++;

    // Snake Speed Control: Moves every 6th frame (~10 moves per second)
    if (currentState == SNAKE_GAME && !snakeGameOver) {
        if(frameCount % 6 == 0) { 
            Point head = snake.front();
            if(snakeDir==1) head.y--; if(snakeDir==2) head.x++; if(snakeDir==3) head.y++; if(snakeDir==4) head.x--;
            if(head.x<0 || head.x>=40 || head.y<0 || head.y>=30) snakeGameOver=true;
            if(!snakeGameOver) {
                snake.push_front(head);
                if(head.x==food.x && head.y==food.y) { snakeScore+=10; food.x=rand()%38+1; food.y=rand()%28+2; }
                else snake.pop_back();
            }
        }
    }

    if (currentState == CAR_GAME && !carGameOver) {
        roadOffset += 12; if(roadOffset > 100) roadOffset = 0;
        for(auto &e : enemies) {
            e.y += e.speed;
            float px = laneCenters[currentLane]-25;
            if(px < e.x+50 && px+50 > e.x && 480 < e.y+90 && 480+90 > e.y) {
                carLives--; enemies.clear(); coins.clear(); 
                if(carLives <= 0) carGameOver = true; break;
            }
        }
        for(auto &c : coins) {
            if(c.active) {
                c.y += 10;
                float px = laneCenters[currentLane]-25;
                if(px < c.x+18 && px+50 > c.x-18 && 480 < c.y+18 && 480+90 > c.y-18) { c.active=false; carScore+=50; }
            }
        }
        if(enemies.empty() || (enemies.back().y > 280)) {
            enemies.push_back({laneCenters[rand()%4]-25, -120, 14.0f + (carScore/200), true});
            if(rand()%10 < 2) coins.push_back({laneCenters[rand()%4], -50, 0, true});
        }
        if(!enemies.empty() && enemies[0].y > 600) { enemies.erase(enemies.begin()); carScore+=10; }
        if(!coins.empty() && coins[0].y > 600) coins.erase(coins.begin());
    }
    if(currentState==SPACE_GAME && !spaceGameOver){

 // moving starfield parallax
 for(auto &s:stars){
   s.y += s.speed;
   if(s.y>600){
     s.y=0;
     s.x=rand()%800;
   }
 }

 // bullets up
 for(auto &b:bullets)
 if(b.active)
   b.y-=12;

 // enemies down
 for(auto &e:alienEnemies)
 if(e.active)
   e.y += e.speed;

 // spawn enemies
 if(rand()%22==0){
  alienEnemies.push_back({
   float(rand()%700+50),
   -20,
   float(2+(rand()%3)),
   true
  });
 }

 // bullet collisions
 for(auto &b:bullets)
 for(auto &e:alienEnemies){

  if(b.active && e.active &&
     fabs(b.x-e.x)<18 &&
     fabs(b.y-e.y)<18){

      b.active=false;
      e.active=false;
      spaceScore +=10;
  }
 }

 // enemy reaches player -> lose life
 for(auto &e:alienEnemies){
  if(e.active && e.y>480 && fabs(e.x-playerShipX)<35){
      e.active=false;
      spaceLives--;

      if(spaceLives<=0)
         spaceGameOver=true;
  }
 }

 // remove offscreen enemies also damage if missed
 for(size_t i=0;i<alienEnemies.size();i++){
   if(alienEnemies[i].y>610 && alienEnemies[i].active){
      alienEnemies[i].active=false;
      spaceLives--;
      if(spaceLives<=0)
        spaceGameOver=true;
   }
 }
}


    glutPostRedisplay();
    glutTimerFunc(16, gameTimer, 0); // Locked to ~60 FPS
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(currentState == MENU) {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glColor3f(1, 0.8f, 0); drawText("ULTIMATE 5-IN-1 ARCADE HUB", 27, 270, 100);
        glColor3f(0.1f, 0.6f, 0.1f); glRectf(300, 200, 500, 250); glColor3f(1,1,1); drawText("1. SNAKE", 8, 360, 230);
        glColor3f(0.7f, 0.1f, 0.1f); glRectf(300, 280, 500, 330); glColor3f(1,1,1); drawText("2. CAR", 6, 365, 310);
        glColor3f(0.15f,0.2f,0.75f);glRectf(300,360,500,410);glColor3f(1,1,1);drawText("3. SPACE SHOOTER",15,330,390);
        glColor3f(0.5f, 0.5f, 0.5f); drawText("Press ESC to return to Menu", 27, 275, 500);
    } 
    else if(currentState == SNAKE_GAME) drawSnakeScreen();
    else if(currentState == CAR_GAME) drawCarGameScreen();
    else if(currentState==SPACE_GAME) drawSpaceGameScreen();
    glutSwapBuffers();
}

void mouse(int b, int s, int x, int y) {
    if(b==GLUT_LEFT_BUTTON && s==GLUT_DOWN && currentState == MENU) {
        if(x>=300 && x<=500 && y>=200 && y<=250) { currentState=SNAKE_GAME; initSnake(); }
        if(x>=300 && x<=500 && y>=280 && y<=330) { currentState=CAR_GAME; initCarGame(); }
        if(x>=300 && x<=500 && y>=360 && y<=410){currentState=SPACE_GAME;initSpaceGame();}
    }
}

void keyboard(unsigned char k, int x, int y) {
    if(k==27) currentState = MENU;
    if(k==' ') { if(snakeGameOver) initSnake(); if(carGameOver) initCarGame();    
    }
    if(currentState==SPACE_GAME){

 if(spaceGameOver){
   initSpaceGame();
 }
 else{
  bullets.push_back({
   playerShipX,
   500,
   true
  });
 }
 return;
}
}

void special(int k, int x, int y) {
    if(currentState == SNAKE_GAME) {
        if(k==GLUT_KEY_UP && snakeDir!=3) snakeDir=1; if(k==GLUT_KEY_RIGHT && snakeDir!=4) snakeDir=2;
        if(k==GLUT_KEY_DOWN && snakeDir!=1) snakeDir=3; if(k==GLUT_KEY_LEFT && snakeDir!=2) snakeDir=4;
    }
    if(currentState == CAR_GAME) {
        if(k==GLUT_KEY_LEFT && currentLane>0) currentLane--; if(k==GLUT_KEY_RIGHT && currentLane<3) currentLane++;
    }
    if(currentState==SPACE_GAME){
        if(k==GLUT_KEY_LEFT && playerShipX>50)
         playerShipX-=22;

        if(k==GLUT_KEY_RIGHT && playerShipX<750)
         playerShipX+=22;
}
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600); glutCreateWindow("Enhanced Arcade Hub");
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 800, 600, 0); glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(display); glutMouseFunc(mouse); glutKeyboardFunc(keyboard);
    glutSpecialFunc(special); glutTimerFunc(16, gameTimer, 0);
    glutMainLoop(); return 0;
}