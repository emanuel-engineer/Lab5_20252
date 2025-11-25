#include <QApplication>
#include "game.h"

int main(int argc, char *argv[]) {
    // suprimir warning de session manager
    unsetenv("SESSION_MANAGER");

    QApplication app(argc, argv);

    // crear directamente el widget del juego
    Game game;
    game.show();

    return app.exec();
}
