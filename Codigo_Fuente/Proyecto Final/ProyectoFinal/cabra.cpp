#include "cabra.h"
#include <QTransform>

Cabra::Cabra(QGraphicsScene *escena)
{
    // Cargar los sprites de la cabra corriendo
    for (int i = 0; i < 6; i++) {
        QPixmap sprite(QString(":/imagenes/imagenes/cabra_correr%1.png").arg(i + 1));
        imagenCorrer[i] = sprite.scaledToHeight(80, Qt::SmoothTransformation).transformed(QTransform().scale(-1, 1));
    }

    // Crear la cabra en el escenario
    imagen = escena->addPixmap(imagenCorrer[0]);
    imagen->setPos(750, 420 - imagen->boundingRect().height());
}

void Cabra::actualizar(double dt)
{
    // Movimiento hacia la izquierda
    imagen->moveBy(-velocidad * dt, 0);

    // Regresar al extremo derecho cuando sale del escenario
    if (imagen->x() + imagen->boundingRect().width() < 0) {
        imagen->setX(imagen->scene()->sceneRect().right());
    }

    // Animacion de carrera
    tiempoAnimacion += dt;

    if (tiempoAnimacion >= 0.12) {
        fotograma = (fotograma + 1) % 6;
        imagen->setPixmap(imagenCorrer[fotograma]);
        tiempoAnimacion = 0;
    }
}
