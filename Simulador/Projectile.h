#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameObject.h"
#include <QColor>

class Projectile : public GameObject {
private:
    double radius;
    bool active;
    QColor color;
    static constexpr double GRAVITY = 98.0;

public:
    Projectile(const Vector2D& pos = Vector2D(), double r = 8.0, double m = 1.0)
        : GameObject(pos, m), radius(r), active(false), color(Qt::red) {}

    void launch(double angleDegrees, double speed, int playerNum) {
        double angleRad = angleDegrees * M_PI / 180.0;
        velocity = Vector2D(speed * std::cos(angleRad), -speed * std::sin(angleRad));
        active = true;
        color = (playerNum == 0) ? QColor(220, 50, 50) : QColor(50, 120, 220);
    }

    void update(double dt) override {
        if (!active) return;
        velocity.y += GRAVITY * dt;
        position += velocity * dt;
    }

    void draw(QPainter& painter) override {
        if (!active) return;

        painter.setBrush(color);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawEllipse(QPointF(position.x, position.y), radius, radius);

        // Estela
        painter.setPen(QPen(color.lighter(150), 1));
        Vector2D dir = velocity.normalize() * (-15);
        painter.drawLine(QPointF(position.x, position.y),
                         QPointF(position.x + dir.x, position.y + dir.y));
    }

    QRectF getBounds() const override {
        return QRectF(position.x - radius, position.y - radius,
                      radius * 2, radius * 2);
    }

    double getRadius() const { return radius; }
    bool isActive() const { return active; }
    void deactivate() { active = false; }

    double getMomentum() const {
        return mass * velocity.magnitude();
    }

    void reflectVelocity(bool horizontal) {
        if (horizontal) {
            velocity.x = -velocity.x;
        } else {
            velocity.y = -velocity.y;
        }
    }

    void applyInelasticCollision(const Vector2D& normal, double restitution = 0.7) {
        double vn = velocity.dot(normal);
        Vector2D normalVel = normal * vn;
        Vector2D tangentVel = velocity - normalVel;
        velocity = tangentVel - normalVel * restitution;
    }
};

#endif // PROJECTILE_H
