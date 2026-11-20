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

    // Cargar los tres sprites de la cabra quemandose
    for (int i = 0; i < 3; i++) {
        QPixmap sprite(QString(":/imagenes/imagenes/cabra_quemando%1.png").arg(i + 1));

        imagenQuemando[i] = sprite.scaledToHeight(80, Qt::SmoothTransformation)
                                .transformed(QTransform().scale(-1, 1));
    }

    // Crear la cabra en el escenario
    imagen = escena->addPixmap(imagenCorrer[0]);
    imagen->setPos(750, 520 - imagen->boundingRect().height());
    imagen->hide(); //No aparezca automaticamente al comenzar la partida
}

void Cabra::actualizar(double dt, double xPo, const std::vector<QGraphicsRectItem*> &trampas)
{

    // Si la cabra esta quemandose, mostrar la animacion
    if (quemando) {
        tiempoQuemando += dt;

        if (tiempoQuemando >= 0.18) {
            tiempoQuemando -= 0.18;
            fotogramaQuemando++;

            if (fotogramaQuemando < 3) {
                imagen->setPixmap(imagenQuemando[fotogramaQuemando]);
            } else {
                // Termino la animacion: desaparecer
                quemando = false;
                imagen->hide();

                // Preparar la proxima aparicion
                imagen->setPixmap(imagenCorrer[0]);
                fotograma = 0;
                tiempoAnimacion = 0;
                tiempoAparicion = 0;
            }
        }

        return;
    }

    // Aparicion aleatoria cada 5 segundos
    tiempoAparicion += dt;

    if (tiempoAparicion >= 5.0) {
        tiempoAparicion -= 5.0;

        if (!imagen->isVisible() && QRandomGenerator::global()->bounded(100) < 70) {
            imagen->setX(xPo + 800);
            imagen->show();
            velocidad = velocidadBase;
            tiempoChoque = 0;
        }
    }

    if (!imagen->isVisible()) {
        return;
    }


    //Movimiento
    imagen->moveBy(velocidad * dt, 0);

    // Comprobar si la cabra toca alguna trampa de lava
    QRectF areaCabra = imagen->sceneBoundingRect();

    for (QGraphicsRectItem *trampa : trampas) {
        QRectF areaLava = trampa->sceneBoundingRect();

        bool coincideHorizontalmente =
            areaCabra.right() > areaLava.left() &&
            areaCabra.left() < areaLava.right();

        bool tocaAlturaLava =
            areaCabra.bottom() >= areaLava.top() &&
            areaCabra.top() < areaLava.bottom();

        if (coincideHorizontalmente && tocaAlturaLava) {
            // Comenzar la animacion de quemadura
            quemando = true;
            fotogramaQuemando = 0;
            tiempoQuemando = 0;
            imagen->setPixmap(imagenQuemando[0]);
            return;
        }
    }


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
            velocidad = velocidadBase;
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
        if (!imagen->isVisible() || quemando) {
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

void Cabra::configurarDificultad(bool dificil)
{
    if (dificil) {
        velocidadBase = -175;
    } else {
        velocidadBase = -160;
    }

    velocidad = velocidadBase;
}