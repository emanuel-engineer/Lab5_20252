#ifndef PLAYER_H
#define PLAYER_H

#include "Obstacle.h"
#include <vector>
#include <memory>
#include <algorithm>

class Player {
private:
    int id;
    std::vector<std::shared_ptr<Obstacle>> obstacles;
    Vector2D representativePos;
    double representativeRadius;
    QColor playerColor;

public:
    Player(int playerId, const Vector2D& repPos, const QColor& color)
        : id(playerId), representativePos(repPos),
        representativeRadius(15.0), playerColor(color) {}

    void addObstacle(std::shared_ptr<Obstacle> obstacle) {
        obstacles.push_back(obstacle);
    }

    std::vector<std::shared_ptr<Obstacle>>& getObstacles() {
        return obstacles;
    }

    void removeDestroyedObstacles() {
        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                           [](const std::shared_ptr<Obstacle>& obs) {
                               return obs->isDestroyed();
                           }),
            obstacles.end()
            );
    }

    bool hasLost() const {
        return obstacles.empty();
    }

    Vector2D getRepresentativePos() const { return representativePos; }
    double getRepresentativeRadius() const { return representativeRadius; }
    QColor getColor() const { return playerColor; }
    int getId() const { return id; }

    void drawRepresentative(QPainter& painter) {
        painter.setBrush(playerColor);
        painter.setPen(QPen(Qt::black, 3));
        painter.drawEllipse(QPointF(representativePos.x, representativePos.y),
                            representativeRadius, representativeRadius);

        // Cara simple
        painter.setBrush(Qt::black);
        painter.drawEllipse(QPointF(representativePos.x - 5, representativePos.y - 3), 2, 2);
        painter.drawEllipse(QPointF(representativePos.x + 5, representativePos.y - 3), 2, 2);

        painter.setPen(QPen(Qt::black, 2));
        painter.drawArc(QRectF(representativePos.x - 7, representativePos.y, 14, 8),
                        180 * 16, 180 * 16);
    }
};

#endif // PLAYER_H
