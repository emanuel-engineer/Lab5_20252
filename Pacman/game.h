#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QPainter>
#include <QVector>
#include <QPoint>

class Game : public QWidget {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void gameLoop();

private:
    // Configuración del juego
    static const int CELL_SIZE = 30;
    static const int GRID_WIDTH = 19;
    static const int GRID_HEIGHT = 21;

    // Mapa (0=vacío, 1=muro, 2=punto, 3=power pellet)
    int map[GRID_HEIGHT][GRID_WIDTH];

    // Pac-Man
    QPointF pacmanPos;
    int pacmanDir; // 0=derecha, 1=abajo, 2=izquierda, 3=arriba
    int nextDir;
    float pacmanSpeed;
    int mouthAngle;

    // Fantasmas
    struct Ghost {
        QPointF pos;
        int dir;
        QColor color;
        bool scared;
    };
    QVector<Ghost> ghosts;

    // Estado del juego
    int score;
    int lives;
    bool gameOver;
    int frightenedTimer;

    // Timer
    QTimer *timer;

    // Métodos auxiliares
    void initMap();
    void initGame();
    void movePacman();
    void moveGhosts();
    bool canMove(QPointF pos, int dir);
    void checkCollisions();
    void eatDot(int x, int y);
    QPointF getNextPos(QPointF pos, int dir);
    void drawPacman(QPainter &painter);
    void drawGhosts(QPainter &painter);
    void drawMap(QPainter &painter);
    void drawUI(QPainter &painter);
};

#endif // GAME_H
