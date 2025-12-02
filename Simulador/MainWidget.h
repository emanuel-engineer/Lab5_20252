#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include "GameScene.h"

class MainWidget : public QWidget {
    Q_OBJECT

private:
    GameScene* scene;
    QTimer* timer;

    QSlider* angleSlider;
    QSlider* speedSlider;
    QLabel* angleLabel;
    QLabel* speedLabel;
    QPushButton* launchButton;
    QPushButton* resetButton;

    static constexpr int SCENE_WIDTH = 1200;
    static constexpr int SCENE_HEIGHT = 400;
    static constexpr int CONTROL_HEIGHT = 120;
    static constexpr double UPDATE_INTERVAL = 16.67;

public:
    MainWidget(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(SCENE_WIDTH, SCENE_HEIGHT + CONTROL_HEIGHT);
        setWindowTitle("Simulador");

        scene = new GameScene(SCENE_WIDTH, SCENE_HEIGHT);

        setupUI();
        setupTimer();
    }

    ~MainWidget() {
        delete scene;
        delete timer;
    }

private:
    void setupUI() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        mainLayout->addStretch(1);

        QWidget* controlPanel = new QWidget();
        controlPanel->setFixedHeight(CONTROL_HEIGHT);
        controlPanel->setStyleSheet("background-color: #f0f0f0;");

        QVBoxLayout* controlLayout = new QVBoxLayout(controlPanel);

        QHBoxLayout* angleLayout = new QHBoxLayout();
        angleLabel = new QLabel("Ángulo: 45°");
        angleLabel->setStyleSheet("color: Black;");
        angleLabel->setMinimumWidth(120);
        angleSlider = new QSlider(Qt::Horizontal);
        angleSlider->setRange(0, 180);
        angleSlider->setValue(45);
        angleLayout->addWidget(angleLabel);
        angleLayout->addWidget(angleSlider);

        QHBoxLayout* speedLayout = new QHBoxLayout();
        speedLabel = new QLabel("Velocidad: 150 m/s");
        speedLabel->setStyleSheet("color: Black;");
        speedLabel->setMinimumWidth(120);
        speedSlider = new QSlider(Qt::Horizontal);
        speedSlider->setRange(50, 300);
        speedSlider->setValue(150);
        speedLayout->addWidget(speedLabel);
        speedLayout->addWidget(speedSlider);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        launchButton = new QPushButton("Lanzar Proyectil");
        launchButton->setStyleSheet(
            "QPushButton { background-color: #4CAF50; color: Black; "
            "font-size: 14px; font-weight: bold; padding: 10px; border-radius: 5px; }"
            "QPushButton:hover { background-color: #45a049; }"
            "QPushButton:disabled { background-color: #cccccc; }");

        resetButton = new QPushButton("Reiniciar Juego");
        resetButton->setStyleSheet(
            "QPushButton { background-color: #2196F3; color: Black; "
            "font-size: 14px; font-weight: bold; padding: 10px; border-radius: 5px; }"
            "QPushButton:hover { background-color: #0b7dda; }");

        buttonLayout->addStretch();
        buttonLayout->addWidget(launchButton);
        buttonLayout->addWidget(resetButton);
        buttonLayout->addStretch();

        controlLayout->addLayout(angleLayout);
        controlLayout->addLayout(speedLayout);
        controlLayout->addLayout(buttonLayout);

        mainLayout->addWidget(controlPanel);

        connect(angleSlider, &QSlider::valueChanged, this, &MainWidget::onAngleChanged);
        connect(speedSlider, &QSlider::valueChanged, this, &MainWidget::onSpeedChanged);
        connect(launchButton, &QPushButton::clicked, this, &MainWidget::onLaunch);
        connect(resetButton, &QPushButton::clicked, this, &MainWidget::onReset);
    }

    void setupTimer() {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWidget::onTimerUpdate);
        timer->start(UPDATE_INTERVAL);
    }

    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.fillRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT, QColor(230, 240, 255));

        scene->draw(painter);
    }

private slots:
    void onAngleChanged(int value) {
        angleLabel->setText(QString("Ángulo: %1°").arg(value));
    }

    void onSpeedChanged(int value) {
        speedLabel->setText(QString("Velocidad: %1 m/s").arg(value));
    }

    void onLaunch() {
        if (!scene->isProjectileActive() && !scene->isGameOver()) {
            double angle = angleSlider->value();
            double speed = speedSlider->value();
            scene->launchProjectile(angle, speed);
            launchButton->setEnabled(false);
        }
    }

    void onReset() {
        scene->reset();
        angleSlider->setValue(45);
        speedSlider->setValue(150);
        launchButton->setEnabled(true);
        update();
    }

    void onTimerUpdate() {
        scene->update(UPDATE_INTERVAL / 1000.0);

        if (!scene->isProjectileActive() && !scene->isGameOver()) {
            launchButton->setEnabled(true);
        }

        update();
    }
};

#endif  // MAINWIDGET_H
