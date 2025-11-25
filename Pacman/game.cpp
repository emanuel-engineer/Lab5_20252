#include "game.h"
#include <QApplication>
#include <cmath>
#include <ctime>
#include <cstdlib>

Game::Game(QWidget *parent) : QWidget(parent) {
    setFixedSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 50);
    setWindowTitle("Pac-Man");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Game::gameLoop);

    srand(time(nullptr));
    initGame();
    timer->start(50); // 20 FPS
}

void Game::initGame() {
    score = 0;
    lives = 3;
    gameOver = false;
    frightenedTimer = 0;
    mouthAngle = 0;

    // Inicializar Pac-Man
    pacmanPos = QPointF(9.5, 15.5);
    pacmanDir = 0;
    nextDir = 0;
    pacmanSpeed = 0.15f;

    // Inicializar fantasmas
    ghosts.clear();
    ghosts.append({QPointF(8.5, 9.5), 0, Qt::red, false});
    ghosts.append({QPointF(9.5, 9.5), 0, Qt::cyan, false});
    ghosts.append({QPointF(10.5, 9.5), 0, QColor(255, 184, 255), false});
    ghosts.append({QPointF(11.5, 9.5), 0, QColor(255, 184, 82), false});

    initMap();
}

void Game::initMap() {
    // Mapa simplificado (1=muro, 2=punto, 3=power pellet)
    int tempMap[GRID_HEIGHT][GRID_WIDTH] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
        {1,3,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,3,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1},
        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
        {1,1,1,1,2,1,1,1,0,1,0,1,1,1,2,1,1,1,1},
        {1,1,1,1,2,1,0,0,0,0,0,0,0,1,2,1,1,1,1},
        {1,1,1,1,2,1,0,1,1,0,1,1,0,1,2,1,1,1,1},
        {0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,0,0,0,0},
        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1},
        {1,1,1,1,2,1,0,0,0,0,0,0,0,1,2,1,1,1,1},
        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1},
        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,3,2,1,2,2,2,2,2,2,2,2,2,2,2,1,2,3,1},
        {1,1,2,1,2,1,2,1,1,1,1,1,2,1,2,1,2,1,1},
        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
        {1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for(int i = 0; i < GRID_HEIGHT; i++) {
        for(int j = 0; j < GRID_WIDTH; j++) {
            map[i][j] = tempMap[i][j];
        }
    }
}

void Game::gameLoop() {
    if(gameOver) return;

    movePacman();
    moveGhosts();
    checkCollisions();

    if(frightenedTimer > 0) {
        frightenedTimer--;
        if(frightenedTimer == 0) {
            for(auto &ghost : ghosts) {
                ghost.scared = false;
            }
        }
    }

    update();
}

QPointF Game::getNextPos(QPointF pos, int dir) {
    QPointF next = pos;
    switch(dir) {
    case 0: next.setX(next.x() + pacmanSpeed); break; // derecha
    case 1: next.setY(next.y() + pacmanSpeed); break; // abajo
    case 2: next.setX(next.x() - pacmanSpeed); break; // izquierda
    case 3: next.setY(next.y() - pacmanSpeed); break; // arriba
    }

    // Túnel (wrap around)
    if(next.x() < 0) next.setX(GRID_WIDTH - 0.5f);
    if(next.x() >= GRID_WIDTH) next.setX(0.5f);

    return next;
}

bool Game::canMove(QPointF pos, int dir) {
    QPointF next = getNextPos(pos, dir);
    int x = static_cast<int>(next.x());
    int y = static_cast<int>(next.y());

    if(y < 0 || y >= GRID_HEIGHT || x < 0 || x >= GRID_WIDTH) return false;
    return map[y][x] != 1;
}

void Game::movePacman() {
    // Intentar cambiar dirección
    if(nextDir != pacmanDir && canMove(pacmanPos, nextDir)) {
        pacmanDir = nextDir;
    }

    // Mover en la dirección actual
    if(canMove(pacmanPos, pacmanDir)) {
        pacmanPos = getNextPos(pacmanPos, pacmanDir);
        mouthAngle = (mouthAngle + 5) % 60;
    }

    // Comer puntos
    int x = static_cast<int>(pacmanPos.x());
    int y = static_cast<int>(pacmanPos.y());
    if(x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        eatDot(x, y);
    }
}

void Game::eatDot(int x, int y) {
    if(map[y][x] == 2) {
        map[y][x] = 0;
        score += 10;
    } else if(map[y][x] == 3) {
        map[y][x] = 0;
        score += 50;
        frightenedTimer = 100;
        for(auto &ghost : ghosts) {
            ghost.scared = true;
        }
    }
}

void Game::moveGhosts() {
    for(auto &ghost : ghosts) {
        // IA simple: movimiento aleatorio
        if(rand() % 20 == 0) {
            ghost.dir = rand() % 4;
        }

        if(canMove(ghost.pos, ghost.dir)) {
            ghost.pos = getNextPos(ghost.pos, ghost.dir);
        } else {
            ghost.dir = rand() % 4;
        }
    }
}

void Game::checkCollisions() {
    for(const auto &ghost : ghosts) {
        float dx = pacmanPos.x() - ghost.pos.x();
        float dy = pacmanPos.y() - ghost.pos.y();
        float dist = sqrt(dx*dx + dy*dy);

        if(dist < 0.5f) {
            if(ghost.scared) {
                score += 200;
            } else {
                lives--;
                if(lives <= 0) {
                    gameOver = true;
                } else {
                    pacmanPos = QPointF(9.5, 15.5);
                }
            }
        }
    }
}

void Game::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawMap(painter);
    drawPacman(painter);
    drawGhosts(painter);
    drawUI(painter);
}

void Game::drawMap(QPainter &painter) {
    for(int y = 0; y < GRID_HEIGHT; y++) {
        for(int x = 0; x < GRID_WIDTH; x++) {
            if(map[y][x] == 1) {
                painter.fillRect(x * CELL_SIZE, y * CELL_SIZE,
                                 CELL_SIZE, CELL_SIZE, Qt::blue);
            } else if(map[y][x] == 2) {
                painter.setBrush(QColor(255, 255, 200));
                painter.drawEllipse(x * CELL_SIZE + CELL_SIZE/2 - 2,
                                    y * CELL_SIZE + CELL_SIZE/2 - 2, 4, 4);
            } else if(map[y][x] == 3) {
                painter.setBrush(Qt::white);
                painter.drawEllipse(x * CELL_SIZE + CELL_SIZE/2 - 5,
                                    y * CELL_SIZE + CELL_SIZE/2 - 5, 10, 10);
            }
        }
    }
}

void Game::drawPacman(QPainter &painter) {
    int x = static_cast<int>(pacmanPos.x() * CELL_SIZE);
    int y = static_cast<int>(pacmanPos.y() * CELL_SIZE);

    painter.setBrush(Qt::yellow);
    int startAngle = (pacmanDir * 90 + mouthAngle/2) * 16;
    painter.drawPie(x - CELL_SIZE/2 + 2, y - CELL_SIZE/2 + 2,
                    CELL_SIZE - 4, CELL_SIZE - 4,
                    startAngle, (360 - mouthAngle) * 16);
}

void Game::drawGhosts(QPainter &painter) {
    for(const auto &ghost : ghosts) {
        int x = static_cast<int>(ghost.pos.x() * CELL_SIZE);
        int y = static_cast<int>(ghost.pos.y() * CELL_SIZE);

        painter.setBrush(ghost.scared ? Qt::blue : ghost.color);
        painter.drawEllipse(x - CELL_SIZE/2 + 2, y - CELL_SIZE/2 + 2,
                            CELL_SIZE - 4, CELL_SIZE - 4);

        // Ojos
        painter.setBrush(Qt::white);
        painter.drawEllipse(x - 5, y - 5, 6, 6);
        painter.drawEllipse(x + 2, y - 5, 6, 6);
    }
}

void Game::drawUI(QPainter &painter) {
    painter.setPen(Qt::white);
    painter.drawText(10, GRID_HEIGHT * CELL_SIZE + 30,
                     QString("Score: %1  Lives: %2").arg(score).arg(lives));

    if(gameOver) {
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void Game::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Left:  nextDir = 2; break;
    case Qt::Key_Right: nextDir = 0; break;
    case Qt::Key_Up:    nextDir = 3; break;
    case Qt::Key_Down:  nextDir = 1; break;
    case Qt::Key_R:     if(gameOver) initGame(); break;
    }
}
