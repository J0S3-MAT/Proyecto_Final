#include "cabra.h"
#include <QTransform>
#include <QRandomGenerator>

Cabra::Cabra(QGraphicsScene *escena)
{
    // Cargar los sprites de la cabra corriendo
    for (int i = 0; i < 6; i++) {
        QPixmap sprite(QString(":/imagenes/imagenes/cabra_correr%1.png").arg(i + 1));
        imagenCorrer[i] = sprite.scaledToHeight(80, Qt::SmoothTransformation).transformed(QTransform().scale(-1, 1));
    }

    // Crear la cabra en el escenario
    imagen = escena->addPixmap(imagenCorrer[0]);
    imagen->setPos(750, 520 - imagen->boundingRect().height());
    imagen->hide(); //No aparezca automaticamente al comenzar la partida
}

void Cabra::actualizar(double dt, double xPo)
{

    // Aparicion aleatoria cada 5 segundos
    tiempoAparicion += dt;

    if (tiempoAparicion >= 5.0) {
        tiempoAparicion -= 5.0;

        if (!imagen->isVisible() && QRandomGenerator::global()->bounded(100) < 70) {
            imagen->setX(xPo + 800);
            imagen->show();
            velocidad = -160;
            tiempoChoque = 0;
        }
    }

    if (!imagen->isVisible()) {
        return;
    }


    //Movimiento
    imagen->moveBy(velocidad * dt, 0);

    // Desaparecer cuando la cabra queda atras de Po
    if (imagen->x() + imagen->boundingRect().width() < xPo - 350) {
        imagen->hide();
        return;
    }

    //Si choca se devuelve
    if (tiempoChoque > 0) {
        tiempoChoque -= dt;

        //Si ya paso el tiempo de choque sigue a la izquirda
        if (tiempoChoque <= 0) {
            velocidad = -160;
        }
    }

    tiempoAnimacion += dt;

    if (tiempoAnimacion >= 0.12) {
        fotograma = (fotograma + 1) % 6;
        imagen->setPixmap(imagenCorrer[fotograma]);
        tiempoAnimacion = 0;
    }
}

QRectF Cabra::obtenerArea() const
    {
        if (!imagen->isVisible()) {
            return QRectF();           //Cuando la cabra no está visible, devolvemos un rectángulo vacío
        }

        return imagen->sceneBoundingRect();
    }

double Cabra::obtenerVelocidad() const
{
    return velocidad;
}

void Cabra::recibirChoque(double nuevaVelocidad)
{
    velocidad = nuevaVelocidad;
    tiempoChoque = 0.35;
}
