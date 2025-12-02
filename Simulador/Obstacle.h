#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "GameObject.h"
#include <QColor>

class Obstacle : public GameObject {
private:
    double width;
    double height;
    double resistance;
    double maxResistance;
    QColor baseColor;

public:
    Obstacle(const Vector2D& pos, double w, double h, double resist, const QColor& color)
        : GameObject(pos, 1.0), width(w), height(h),
        resistance(resist), maxResistance(resist), baseColor(color) {}

    void takeDamage(double damage) {
        resistance -= damage;
        if (resistance < 0) resistance = 0;
    }

    bool isDestroyed() const {
        return resistance <= 0;
    }

    double getResistance() const { return resistance; }
    double getMaxResistance() const { return maxResistance; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }

    void draw(QPainter& painter) override {
        double healthPercent = resistance / maxResistance;

        QColor currentColor;
        if (healthPercent > 0.66) {
            currentColor = baseColor;
        } else if (healthPercent > 0.33) {
            currentColor = baseColor.lighter(120);
        } else {
            currentColor = baseColor.lighter(150);
        }

        painter.setBrush(currentColor);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawRect(QRectF(position.x, position.y, width, height));

        // Barra de vida
        double barWidth = width * healthPercent;
        painter.setBrush(Qt::green);
        painter.drawRect(QRectF(position.x, position.y - 8, barWidth, 5));
        painter.setBrush(Qt::red);
        painter.drawRect(QRectF(position.x + barWidth, position.y - 8,
                                width - barWidth, 5));

        // Mostrar valor de resistencia
        painter.setPen(Qt::white);
        QFont font = painter.font();
        font.setPointSize(8);
        font.setBold(true);
        painter.setFont(font);
        QString text = QString("%1").arg((int)resistance);
        painter.drawText(QRectF(position.x, position.y, width, height),
                         Qt::AlignCenter, text);
    }

    QRectF getBounds() const override {
        return QRectF(position.x, position.y, width, height);
    }

    Vector2D getCollisionNormal(const Vector2D& projectilePos) const {
        QRectF bounds = getBounds();
        Vector2D center(bounds.center().x(), bounds.center().y());
        Vector2D dir = projectilePos - center;

        double dx = std::abs(dir.x) / (width / 2.0);
        double dy = std::abs(dir.y) / (height / 2.0);

        if (dx > dy) {
            return Vector2D(dir.x > 0 ? 1 : -1, 0);
        } else {
            return Vector2D(0, dir.y > 0 ? 1 : -1);
        }
    }
};

#endif  // OBSTACLE_H
