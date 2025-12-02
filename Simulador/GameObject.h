#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Vector2D.h"
#include <QPainter>
#include <QRectF>

class GameObject {
protected:
    Vector2D position;
    Vector2D velocity;
    double mass;

public:
    GameObject(const Vector2D& pos = Vector2D(), double m = 1.0)
        : position(pos), velocity(Vector2D()), mass(m) {}

    virtual ~GameObject() {}

    virtual void update(double dt) {
        position += velocity * dt;
    }

    virtual void draw(QPainter& painter) = 0;
    virtual QRectF getBounds() const = 0;

    Vector2D getPosition() const { return position; }
    Vector2D getVelocity() const { return velocity; }
    double getMass() const { return mass; }

    void setPosition(const Vector2D& pos) { position = pos; }
    void setVelocity(const Vector2D& vel) { velocity = vel; }
};

#endif // GAMEOBJECT_H
