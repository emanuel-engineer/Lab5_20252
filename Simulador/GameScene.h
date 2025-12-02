#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Player.h"
#include "Projectile.h"
#include <QPainter>
#include <QRectF>

class GameScene {
private:
    std::vector<Player> players;
    std::shared_ptr<Projectile> projectile;
    int currentPlayer;
    bool gameOver;
    int winner;
    QRectF bounds;
    double turnTimer;  // Temporizador del turno en segundos
    bool turnEnded;  // Nueva bandera para controlar cambio de turno

    static constexpr double DAMAGE_FACTOR = 0.5;
    static constexpr double RESTITUTION_COEF = 0.7;
    static constexpr double MAX_TURN_TIME = 12.0;  // 12 segundos por turno

public:
    GameScene(double width, double height)
        : currentPlayer(0), gameOver(false), winner(-1),
        bounds(0, 0, width, height), turnTimer(0.0), turnEnded(false) {

        projectile = std::make_shared<Projectile>(Vector2D(0, 0), 8.0, 1.0);
        initializePlayers();
    }

    void initializePlayers() {
        // Jugador 1 (izquierda) - Ajustado para ventana más grande
        Player p1(0, Vector2D(70, bounds.height() - 70), QColor(200, 80, 80));

        p1.addObstacle(std::make_shared<Obstacle>(
            Vector2D(120, bounds.height() - 180), 70, 100, 120.0, QColor(180, 60, 60)));
        p1.addObstacle(std::make_shared<Obstacle>(
            Vector2D(210, bounds.height() - 150), 60, 70, 100.0, QColor(180, 60, 60)));
        p1.addObstacle(std::make_shared<Obstacle>(
            Vector2D(150, bounds.height() - 280), 80, 80, 150.0, QColor(180, 60, 60)));
        p1.addObstacle(std::make_shared<Obstacle>(
            Vector2D(290, bounds.height() - 200), 70, 120, 130.0, QColor(180, 60, 60)));

        // Jugador 2 (derecha) - Ajustado para ventana más grande
        Player p2(1, Vector2D(bounds.width() - 70, bounds.height() - 70),
                  QColor(80, 120, 200));

        p2.addObstacle(std::make_shared<Obstacle>(
            Vector2D(bounds.width() - 190, bounds.height() - 180), 70, 100, 120.0,
            QColor(60, 100, 180)));
        p2.addObstacle(std::make_shared<Obstacle>(
            Vector2D(bounds.width() - 270, bounds.height() - 150), 60, 70, 100.0,
            QColor(60, 100, 180)));
        p2.addObstacle(std::make_shared<Obstacle>(
            Vector2D(bounds.width() - 230, bounds.height() - 280), 80, 80, 150.0,
            QColor(60, 100, 180)));
        p2.addObstacle(std::make_shared<Obstacle>(
            Vector2D(bounds.width() - 360, bounds.height() - 200), 70, 120, 130.0,
            QColor(60, 100, 180)));

        players.push_back(p1);
        players.push_back(p2);
    }

    void launchProjectile(double angle, double speed) {
        if (projectile->isActive() || gameOver) return;

        Vector2D launchPos = players[currentPlayer].getRepresentativePos();
        projectile->setPosition(launchPos);
        projectile->launch(angle, speed, currentPlayer);
        turnTimer = 0.0;  // Reiniciar temporizador
        turnEnded = false;  // Resetear bandera al lanzar
    }

    void update(double dt) {
        if (gameOver) return;

        // Si el proyectil no está activo, no hay nada que actualizar
        if (!projectile->isActive()) return;

        // Actualizar temporizador del turno
        turnTimer += dt;

        projectile->update(dt);

        checkBoundaryCollisions();
        checkObstacleCollisions();
        checkRepresentativeHit();

        Vector2D pos = projectile->getPosition();

        // Condiciones para terminar el turno
        if (turnTimer >= MAX_TURN_TIME ||  // Tiempo excedido
            pos.y > bounds.height() + 100 ||  // Salió por abajo
            pos.x < -100 || pos.x > bounds.width() + 100) {  // Salió por los lados
            if (!turnEnded) {
                endTurn();
            }
        }
    }

    void checkBoundaryCollisions() {
        Vector2D pos = projectile->getPosition();
        double radius = projectile->getRadius();

        if (pos.x - radius < bounds.left()) {
            projectile->setPosition(Vector2D(bounds.left() + radius, pos.y));
            projectile->reflectVelocity(true);
        } else if (pos.x + radius > bounds.right()) {
            projectile->setPosition(Vector2D(bounds.right() - radius, pos.y));
            projectile->reflectVelocity(true);
        }

        if (pos.y - radius < bounds.top()) {
            projectile->setPosition(Vector2D(pos.x, bounds.top() + radius));
            projectile->reflectVelocity(false);
        }

        if (pos.y + radius > bounds.bottom()) {
            projectile->setPosition(Vector2D(pos.x, bounds.bottom() - radius));
            projectile->reflectVelocity(false);
        }
    }

    void checkObstacleCollisions() {
        int opponent = (currentPlayer + 1) % 2;
        auto& obstacles = players[opponent].getObstacles();

        for (auto& obstacle : obstacles) {
            if (checkCollision(projectile.get(), obstacle.get())) {
                handleObstacleCollision(obstacle);
            }
        }

        players[opponent].removeDestroyedObstacles();

        if (players[opponent].hasLost()) {
            gameOver = true;
            winner = currentPlayer;
        }
    }

    bool checkCollision(Projectile* proj, Obstacle* obs) {
        QRectF projBounds = proj->getBounds();
        QRectF obsBounds = obs->getBounds();
        return projBounds.intersects(obsBounds);
    }

    void handleObstacleCollision(std::shared_ptr<Obstacle> obstacle) {
        double momentum = projectile->getMomentum();
        double damage = DAMAGE_FACTOR * momentum;
        obstacle->takeDamage(damage);

        Vector2D normal = obstacle->getCollisionNormal(projectile->getPosition());
        projectile->applyInelasticCollision(normal, RESTITUTION_COEF);

        QRectF bounds = obstacle->getBounds();
        Vector2D pos = projectile->getPosition();
        double radius = projectile->getRadius();

        if (normal.x != 0) {
            double newX = (normal.x > 0) ? bounds.right() + radius : bounds.left() - radius;
            projectile->setPosition(Vector2D(newX, pos.y));
        } else {
            double newY = (normal.y > 0) ? bounds.bottom() + radius : bounds.top() - radius;
            projectile->setPosition(Vector2D(pos.x, newY));
        }
    }

    void checkRepresentativeHit() {
        int opponent = (currentPlayer + 1) % 2;
        Vector2D repPos = players[opponent].getRepresentativePos();
        double repRadius = players[opponent].getRepresentativeRadius();

        Vector2D projPos = projectile->getPosition();
        double projRadius = projectile->getRadius();

        double distance = (projPos - repPos).magnitude();

        if (distance < repRadius + projRadius) {
            gameOver = true;
            winner = currentPlayer;
        }
    }

    void endTurn() {
        projectile->deactivate();
        currentPlayer = (currentPlayer + 1) % 2;
        turnTimer = 0.0;
        turnEnded = true;
    }

    void draw(QPainter& painter) {
        painter.setPen(QPen(Qt::black, 3));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(bounds);

        for (auto& player : players) {
            for (auto& obstacle : player.getObstacles()) {
                obstacle->draw(painter);
            }
            player.drawRepresentative(painter);
        }

        projectile->draw(painter);

        // Información del juego
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(16);
        font.setBold(true);
        painter.setFont(font);

        if (gameOver) {
            QString text = QString("¡JUGADOR %1 GANA!").arg(winner + 1);
            QRectF textRect(0, 20, bounds.width(), 50);
            painter.fillRect(textRect, QColor(255, 255, 200, 220));
            painter.setPen(QPen(Qt::darkGreen, 2));
            painter.drawText(textRect, Qt::AlignCenter, text);
        } else {
            // Mostrar turno actual con color del jugador
            QString text = QString("Turno: Jugador %1").arg(currentPlayer + 1);
            QRectF textRect(bounds.width() / 2 - 100, 20, 200, 40);

            QColor bgColor = players[currentPlayer].getColor().lighter(140);
            bgColor.setAlpha(200);
            painter.fillRect(textRect, bgColor);

            painter.setPen(Qt::black);
            painter.drawText(textRect, Qt::AlignCenter, text);

            // Mostrar temporizador si hay proyectil activo
            if (projectile->isActive()) {
                double timeLeft = MAX_TURN_TIME - turnTimer;
                if (timeLeft < 0) timeLeft = 0;

                QString timerText = QString("Tiempo: %1s").arg(timeLeft, 0, 'f', 1);
                QRectF timerRect(bounds.width() - 150, 20, 130, 40);

                // Color del timer: verde -> amarillo -> rojo según tiempo restante
                QColor timerBg;
                if (timeLeft > 8) {
                    timerBg = QColor(200, 255, 200, 200);  // Verde
                } else if (timeLeft > 4) {
                    timerBg = QColor(255, 255, 200, 200);  // Amarillo
                } else {
                    timerBg = QColor(255, 200, 200, 200);  // Rojo
                }

                painter.fillRect(timerRect, timerBg);
                font.setPointSize(14);
                painter.setFont(font);
                painter.drawText(timerRect, Qt::AlignCenter, timerText);
            }
        }
    }

    bool isProjectileActive() const { return projectile->isActive(); }
    bool isGameOver() const { return gameOver; }
    int getCurrentPlayer() const { return currentPlayer; }

    void reset() {
        players.clear();
        projectile->deactivate();
        currentPlayer = 0;
        gameOver = false;
        winner = -1;
        turnTimer = 0.0;
        turnEnded = false;
        initializePlayers();
    }

    double getTurnTimeLeft() const {
        return MAX_TURN_TIME - turnTimer;
    }
};

#endif // GAMESCENE_H
