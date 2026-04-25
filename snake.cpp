#include <GL/glut.h>
#include <string>
#include <vector>
#include <iostream>
#include <deque>
#include <cmath>

const int SCREEN_W = 800;
const int SCREEN_H = 600;

enum GameState { MENU, SNAKE_GAME, CAR_GAME, SPACE_GAME, PUZZLE_3D };
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
struct Bullet { float x, y, vx, vy; }; std::vector<Bullet> bullets;
struct EnemyShip { float x, y, speed, maxHp, hp; int type; }; std::vector<EnemyShip> alienEnemies;
struct Star { float x, y, speed, size; }; std::vector<Star> stars;
struct Particle { float x, y, vx, vy, life, maxLife, r, g, b; }; std::vector<Particle> particles;

float playerShipX = 400; int spaceLives = 3; int spaceScore = 0; bool spaceGameOver = false; float nebulaShift = 0;

void spawnExplosion(float x, float y, float r, float g, float b, int count) {
    for (int i = 0; i < count; i++) {
        float vx = (float)((rand() % 100) - 50) / 10.0f; float vy = (float)((rand() % 100) - 50) / 10.0f;
        particles.push_back({x, y, vx, vy, 25.0f, 25.0f, r, g, b});
    }
}

void initSpaceGame() {
    bullets.clear(); alienEnemies.clear(); stars.clear(); particles.clear();
    playerShipX = 400; spaceLives = 3; spaceScore = 0; spaceGameOver = false;
    for (int i = 0; i < 150; i++) stars.push_back({float(rand() % 800), float(rand() % 600), float(1 + (rand() % 3)), float(1 + rand() % 2)});
}

void drawSpaceGameScreen() {
    glClearColor(0.01f, 0.01f, 0.05f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    nebulaShift += 0.2f;
    glColor4f(0.05f, 0.0f, 0.2f, 0.5f); glRectf(0, 40 * sin(nebulaShift / 30), 800, 250);
    glColor4f(0.0f, 0.05f, 0.15f, 0.6f); glRectf(0, 260 + 30 * cos(nebulaShift / 25), 800, 480);

    for (auto &s : stars) {
        float brightness = s.speed / 3.0f; glColor4f(brightness, brightness, brightness, 1.0f);
        glRectf(s.x, s.y, s.x + s.size, s.y + s.size);
    }

    if (spaceGameOver) { glDisable(GL_BLEND); glColor3f(1, 1, 1); drawText("SHIP DESTROYED! PRESS SPACE TO RESTART", 38, 220, 300); return; }

    for (auto &p : particles) { float alpha = p.life / p.maxLife; glColor4f(p.r, p.g, p.b, alpha); glRectf(p.x - 2, p.y - 2, p.x + 2, p.y + 2); }
    for (auto &b : bullets) { glColor4f(1.0f, 1.0f, 0.0f, 1.0f); glRectf(b.x - 1, b.y, b.x + 1, b.y + 12); glColor4f(1.0f, 0.5f, 0.0f, 0.4f); glRectf(b.x - 3, b.y - 2, b.x + 3, b.y + 14); }

    for (auto &e : alienEnemies) {
        if (e.type == 0) {
            glColor3f(0.8f, 0.1f, 0.1f); glBegin(GL_TRIANGLES); glVertex2f(e.x - 15, e.y - 10); glVertex2f(e.x + 15, e.y - 10); glVertex2f(e.x, e.y + 15); glEnd();
            glColor3f(1.0f, 0.5f, 0.0f); glRectf(e.x - 8, e.y - 15, e.x + 8, e.y - 10);
        } else {
            glColor3f(0.6f, 0.1f, 0.8f); glRectf(e.x - 20, e.y - 15, e.x + 20, e.y + 5);
            glColor3f(0.2f, 0.9f, 0.2f); glRectf(e.x - 10, e.y + 5, e.x + 10, e.y + 15);
        }
    }

    glColor3f(0.2f, 0.8f, 1.0f); glBegin(GL_POLYGON); glVertex2f(playerShipX, 500); glVertex2f(playerShipX - 15, 520); glVertex2f(playerShipX - 10, 540); glVertex2f(playerShipX + 10, 540); glVertex2f(playerShipX + 15, 520); glEnd();
    glColor3f(0.8f, 0.9f, 1.0f); glBegin(GL_TRIANGLES); glVertex2f(playerShipX - 15, 525); glVertex2f(playerShipX - 35, 545); glVertex2f(playerShipX - 10, 540); glEnd();
    glBegin(GL_TRIANGLES); glVertex2f(playerShipX + 15, 525); glVertex2f(playerShipX + 35, 545); glVertex2f(playerShipX + 10, 540); glEnd();
    glColor3f(0.1f, 0.1f, 0.4f); glRectf(playerShipX - 4, 515, playerShipX + 4, 525);

    glDisable(GL_BLEND);
    glColor3f(0.05f, 0.05f, 0.1f); glRectf(0, 0, 800, 35);
    glColor3f(0, 1, 1); std::string hud = "SCORE: " + std::to_string(spaceScore) + "   |   SHIELDS: " + std::to_string(spaceLives) + "   |   WEAPON: " + (spaceScore >= 300 ? "PLASMA SPREAD" : (spaceScore >= 150 ? "TWIN LASER" : "SINGLE BLASTER"));
    drawText(hud.c_str(), hud.length(), 20, 22);
}

// ==========================================
//      3D PUZZLE (RANDOMIZED TARGETS)
// ==========================================
float puzRotX=45, puzRotY=45, puzZoom=6; int lastMouseX=-1, lastMouseY=-1; bool isDragging=false, puzSolved=false; int puzTime=0;
int puzTargetFace = 0; 
std::string puzTargetName = "GOLD";

void initPuzzle() { 
    puzRotX = (rand()%180)+45; puzRotY = (rand()%180)+45; puzZoom = 6; puzTime = 0; puzSolved = false; 
    puzTargetFace = rand() % 6; // Choose a random face from 0 to 5
    std::string names[] = {"GOLD", "RED", "GREEN", "BLUE", "PURPLE", "CYAN"};
    puzTargetName = names[puzTargetFace];
}

void drawTexturedFace(float r, float g, float b) {
    glColor3f(r, g, b); glBegin(GL_QUADS); glVertex3f(-1,-1,1); glVertex3f(1,-1,1); glVertex3f(1,1,1); glVertex3f(-1,1,1); glEnd();
    glColor3f(r*0.5f, g*0.5f, b*0.5f); glBegin(GL_QUADS); glVertex3f(-0.7f,-0.7f,1.01f); glVertex3f(0.7f,-0.7f,1.01f); glVertex3f(0.7f,0.7f,1.01f); glVertex3f(-0.7f,0.7f,1.01f); glEnd();
}

void draw3DPuzzleScreen() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity(); gluPerspective(45.0, (double)SCREEN_W/SCREEN_H, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glEnable(GL_COLOR_MATERIAL);
    GLfloat aL[]={0.3f,0.3f,0.3f,1}, dL[]={0.8f,0.8f,0.8f,1}, lP[]={5,5,5,1}; glLightfv(GL_LIGHT0, GL_AMBIENT, aL); glLightfv(GL_LIGHT0, GL_DIFFUSE, dL); glLightfv(GL_LIGHT0, GL_POSITION, lP);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    gluLookAt(0,0,puzZoom,0,0,0,0,1,0); glRotatef(puzRotX, 1,0,0); glRotatef(puzRotY, 0,1,0);
    
    // Exact Target Matching Logic
    int modX = ((int)round(puzRotX) % 360 + 360) % 360;
    int modY = ((int)round(puzRotY) % 360 + 360) % 360;
    bool matched = false;
    if (puzTargetFace == 0 && (modX<12||modX>348) && (modY<12||modY>348)) matched = true; // Front (Gold)
    if (puzTargetFace == 1 && (modX<12||modX>348) && (modY>168&&modY<192)) matched = true; // Back (Red)
    if (puzTargetFace == 2 && (modX>78&&modX<102)  && (modY<12||modY>348)) matched = true; // Top (Green)
    if (puzTargetFace == 3 && (modX>258&&modX<282) && (modY<12||modY>348)) matched = true; // Bottom (Blue)
    if (puzTargetFace == 4 && (modX<12||modX>348) && (modY>258&&modY<282)) matched = true; // Right (Purple)
    if (puzTargetFace == 5 && (modX<12||modX>348) && (modY>78&&modY<102))  matched = true; // Left (Cyan)

    if (!puzSolved && matched && puzTime > 30) puzSolved = true;

    // Draw Cube Faces
    glNormal3f(0,0,1); drawTexturedFace(1,0.8f,0); // Gold
    glPushMatrix(); glRotatef(180, 0,1,0); glNormal3f(0,0,1); drawTexturedFace(0.8f,0.1f,0.1f); glPopMatrix(); // Red
    glPushMatrix(); glRotatef(-90, 1,0,0); glNormal3f(0,0,1); drawTexturedFace(0.1f,0.8f,0.1f); glPopMatrix(); // Green
    glPushMatrix(); glRotatef(90, 1,0,0);  glNormal3f(0,0,1); drawTexturedFace(0.1f,0.3f,0.9f); glPopMatrix(); // Blue
    glPushMatrix(); glRotatef(90, 0,1,0);  glNormal3f(0,0,1); drawTexturedFace(0.6f,0.1f,0.8f); glPopMatrix(); // Purple
    glPushMatrix(); glRotatef(-90, 0,1,0); glNormal3f(0,0,1); drawTexturedFace(0.1f,0.8f,0.8f); glPopMatrix(); // Cyan
    
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW); glPopMatrix();
    
    glColor3f(1,1,1); std::string hud = puzSolved ? "SOLVED! Press SPACE to play again" : "Align the " + puzTargetName + " face forward! | Time: " + std::to_string(puzTime/60) + "s";
    drawText(hud.c_str(), hud.length(), 20, 30);
}
// ==========================================
//           GAME TIMER (FPS & SPEED)
// ==========================================
// ==========================================
//           CORE TIMER & LOGIC
// ==========================================
int frameCount = 0;
void gameTimer(int v) {
    frameCount++;

    if (currentState == SNAKE_GAME && !snakeGameOver && frameCount%6==0) {
        Point head = snake.front(); if(snakeDir==1) head.y--; if(snakeDir==2) head.x++; if(snakeDir==3) head.y++; if(snakeDir==4) head.x--;
        if(head.x<0 || head.x>=40 || head.y<0 || head.y>=30) snakeGameOver=true;
        if(!snakeGameOver) { snake.push_front(head); if(head.x==food.x && head.y==food.y) { snakeScore+=10; food.x=rand()%38+1; food.y=rand()%28+2; } else snake.pop_back(); }
    }
    else if (currentState == CAR_GAME && !carGameOver) {
        roadOffset += 12; if(roadOffset > 100) roadOffset = 0;
        for(auto &e : enemies) { e.y += e.speed; if(laneCenters[currentLane]-25 < e.x+50 && laneCenters[currentLane]+25 > e.x && 480 < e.y+90 && 570 > e.y) { carLives--; enemies.clear(); if(carLives<=0) carGameOver=true; break; } }
        for(auto &c : coins) if(c.active) { c.y += 10; if(laneCenters[currentLane]-25 < c.x+18 && laneCenters[currentLane]+25 > c.x-18 && 480 < c.y+18 && 570 > c.y-18) { c.active=false; carScore+=50; } }
        if(enemies.empty() || (enemies.back().y > 280)) { enemies.push_back({laneCenters[rand()%4]-25, -120, 14.0f + (carScore/200), true}); if(rand()%10 < 2) coins.push_back({laneCenters[rand()%4], -50, 0, true}); }
        if(!enemies.empty() && enemies[0].y > 600) { enemies.erase(enemies.begin()); carScore+=10; }
    }
    else if (currentState == SPACE_GAME && !spaceGameOver) {
        for (auto &s : stars) { s.y += s.speed; if (s.y > 600) { s.y = -10; s.x = rand() % 800; } }
        spawnExplosion(playerShipX, 545, 0.0f, 0.8f, 1.0f, 1);
        
        for (size_t i = 0; i < particles.size();) {
            particles[i].x += particles[i].vx; particles[i].y += particles[i].vy; particles[i].life -= 1.0f;
            if (particles[i].life <= 0) particles.erase(particles.begin() + i); else i++;
        }

        for (size_t i = 0; i < bullets.size();) {
            bullets[i].x += bullets[i].vx; bullets[i].y += bullets[i].vy;
            if (bullets[i].y < -20 || bullets[i].x < -20 || bullets[i].x > 820) bullets.erase(bullets.begin() + i); else i++;
        }

        if (rand() % 30 == 0) alienEnemies.push_back({float(rand() % 700 + 50), -30, float(3 + (rand() % 4)), 1, 1, rand() % 5 == 0 ? 1 : 0});
        
        for (size_t i = 0; i < alienEnemies.size();) {
            alienEnemies[i].y += alienEnemies[i].speed;
            bool enemyDestroyed = false;

            // Collision with Bullets
            for (size_t b = 0; b < bullets.size();) {
                if (fabs(bullets[b].x - alienEnemies[i].x) < 20 && fabs(bullets[b].y - alienEnemies[i].y) < 20) {
                    bullets.erase(bullets.begin() + b);
                    enemyDestroyed = true; spaceScore += (alienEnemies[i].type == 1 ? 30 : 10);
                    spawnExplosion(alienEnemies[i].x, alienEnemies[i].y, 1.0f, 0.5f, 0.0f, 20); 
                    break;
                } else b++;
            }

            // PERFECTED HITBOX LOGIC: Only lose life if it exactly hits the ship, OR goes completely off-screen (past the player)
            bool hitShip = (alienEnemies[i].y > 490 && alienEnemies[i].y < 540 && fabs(alienEnemies[i].x - playerShipX) < 25);
            bool passedScreen = (alienEnemies[i].y > 610);

            if (!enemyDestroyed && hitShip) {
                enemyDestroyed = true; spaceLives--; spawnExplosion(playerShipX, 520, 0.2f, 0.5f, 1.0f, 30); 
                if (spaceLives <= 0) spaceGameOver = true;
            } else if (!enemyDestroyed && passedScreen) {
                enemyDestroyed = true; spaceLives--; 
                if (spaceLives <= 0) spaceGameOver = true; 
            }

            if (enemyDestroyed) alienEnemies.erase(alienEnemies.begin() + i); else i++;
        }
    }
    else if (currentState == PUZZLE_3D && !puzSolved) { puzTime++; }

    glutPostRedisplay(); glutTimerFunc(16, gameTimer, 0); 
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    if(currentState == MENU) {
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glColor3f(1, 0.8f, 0); drawText("ULTIMATE 4-IN-1 ARCADE HUB", 26, 260, 80);
        glColor3f(0.1f, 0.6f, 0.1f); glRectf(300, 150, 500, 200); glColor3f(1,1,1); drawText("1. SNAKE", 8, 360, 180);
        glColor3f(0.7f, 0.1f, 0.1f); glRectf(300, 230, 500, 280); glColor3f(1,1,1); drawText("2. CAR", 6, 365, 260);
        glColor3f(0.15f,0.2f,0.75f); glRectf(300, 310, 500, 360); glColor3f(1,1,1); drawText("3. SPACE", 8, 360, 340);
        glColor3f(0.6f, 0.4f, 0.8f); glRectf(300, 390, 500, 440); glColor3f(1,1,1); drawText("4. 3D PUZZLE", 12, 350, 420);
        glColor3f(0.5f, 0.5f, 0.5f); drawText("Press ESC to return to Menu", 27, 275, 520);
    } 
    else if(currentState == SNAKE_GAME) drawSnakeScreen(); else if(currentState == CAR_GAME) drawCarGameScreen(); else if(currentState == SPACE_GAME) drawSpaceGameScreen(); else if(currentState == PUZZLE_3D) draw3DPuzzleScreen();
    glutSwapBuffers();
}

void mouse(int b, int s, int x, int y) {
    if(b==GLUT_LEFT_BUTTON && s==GLUT_DOWN && currentState == MENU) {
        if(x>=300 && x<=500 && y>=150 && y<=200) { currentState=SNAKE_GAME; initSnake(); }
        if(x>=300 && x<=500 && y>=230 && y<=280) { currentState=CAR_GAME; initCarGame(); }
        if(x>=300 && x<=500 && y>=310 && y<=360) { currentState=SPACE_GAME; initSpaceGame(); }
        if(x>=300 && x<=500 && y>=390 && y<=440) { currentState=PUZZLE_3D; initPuzzle(); }
    }
    if (currentState == PUZZLE_3D) {
        if (b == GLUT_LEFT_BUTTON) { if (s == GLUT_DOWN) { isDragging = true; lastMouseX = x; lastMouseY = y; } else isDragging = false; }
        if (b == 3) puzZoom -= 0.5f; if (b == 4) puzZoom += 0.5f;
    }
}

void motion(int x, int y) {
    if (currentState == PUZZLE_3D && isDragging && !puzSolved) { puzRotY += (x - lastMouseX) * 0.5f; puzRotX += (y - lastMouseY) * 0.5f; lastMouseX = x; lastMouseY = y; glutPostRedisplay(); }
}

void keyboard(unsigned char k, int x, int y) {
    if(k==27) currentState = MENU;
    if(k==' ') { 
        if(currentState == SNAKE_GAME && snakeGameOver) initSnake(); 
        if(currentState == CAR_GAME && carGameOver) initCarGame(); 
        if(currentState == SPACE_GAME && spaceGameOver) initSpaceGame();
        
        // Puzzle Restart Logic
        if(currentState == PUZZLE_3D && puzSolved) initPuzzle();
        
        // Weapon Logic
        if(currentState == SPACE_GAME && !spaceGameOver) {
            if (spaceScore >= 300) { bullets.push_back({playerShipX, 490, 0, -18}); bullets.push_back({playerShipX-10, 490, -4, -16}); bullets.push_back({playerShipX+10, 490, 4, -16}); } 
            else if (spaceScore >= 150) { bullets.push_back({playerShipX-15, 490, 0, -15}); bullets.push_back({playerShipX+15, 490, 0, -15}); } 
            else { bullets.push_back({playerShipX, 490, 0, -15}); }
        }
    }
}

void special(int k, int x, int y) {
    if(currentState == SNAKE_GAME) { if(k==GLUT_KEY_UP && snakeDir!=3) snakeDir=1; if(k==GLUT_KEY_RIGHT && snakeDir!=4) snakeDir=2; if(k==GLUT_KEY_DOWN && snakeDir!=1) snakeDir=3; if(k==GLUT_KEY_LEFT && snakeDir!=2) snakeDir=4; }
    if(currentState == CAR_GAME) { if(k==GLUT_KEY_LEFT && currentLane>0) currentLane--; if(k==GLUT_KEY_RIGHT && currentLane<3) currentLane++; }
    if(currentState == SPACE_GAME){ if(k==GLUT_KEY_LEFT && playerShipX>40) playerShipX-=25; if(k==GLUT_KEY_RIGHT && playerShipX<760) playerShipX+=25; }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600); glutCreateWindow("Elite 4-in-1 Arcade Hub");
    glMatrixMode(GL_PROJECTION); glLoadIdentity(); gluOrtho2D(0, 800, 600, 0); glMatrixMode(GL_MODELVIEW);
    glutDisplayFunc(display); glutMouseFunc(mouse); glutMotionFunc(motion); glutKeyboardFunc(keyboard); glutSpecialFunc(special); glutTimerFunc(16, gameTimer, 0);
    glutMainLoop(); return 0;
}
