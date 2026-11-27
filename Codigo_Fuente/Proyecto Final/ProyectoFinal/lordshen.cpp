#include "lordshen.h"
#include <QBrush>
#include <QPen>
#include <QTransform>

LordShen::LordShen(QGraphicsScene *escena) : Enemigo(escena)
{
    // Cargar los sprites de Lord Shen
    for (int i = 0; i < 5; i++) {
        imagenCaminar[i] = QPixmap(QString(":/imagenes/imagenes/lord_caminado%1.png").arg(i + 1)).scaledToHeight(110, Qt::FastTransformation);
    }

    // Cargar los sprites del lanzamiento
    for (int i = 0; i < 4; i++) {
        imagenLanzamiento[i] = QPixmap(QString(":/imagenes/imagenes/lord_lanzando%1.png").arg(i + 1)).scaledToHeight(110, Qt::FastTransformation);
    }

    // Posicion inicial del enemigo
    imagen->setPixmap(imagenCaminar[0]);
    imagen->setPos(650, 520 - imagen->boundingRect().height());
    actualizarAnimacion(0);

    // Cargar las imagenes de los proyectiles
    imagenArepa = QPixmap(":/imagenes/imagenes/arepa.png").scaled(24, 24, Qt::KeepAspectRatio, Qt::FastTransformation);
    imagenPluma = QPixmap(":/imagenes/imagenes/pluma.png").scaled(28, 28, Qt::KeepAspectRatio, Qt::FastTransformation);

    // Crear el proyectil
    proyectil = escena->addPixmap(imagenArepa);
    proyectil->hide();
}

void LordShen::actuar(double dt, const QRectF &areaPo)
{
    // Ajustar la dificultad del enemigo
    dt *= factorDificultad;

    // Movimiento constante de Lord Shen
    imagen->moveBy(direccionMovimiento * velocidadMovimiento * dt, 0);

    if (imagen->x() <= 210) {
        imagen->setX(210);
        direccionMovimiento = 1;
    }
    else if (imagen->x() >= 650) {
        imagen->setX(650);
        direccionMovimiento = -1;
    }

    // Actualizar la animacion del movimiento
    actualizarAnimacion(dt);

    // Movimiento de los proyectiles
    if (estado == Disparando) {

        // Proyectil parabolico
        if (ataqueParabolico) {
            if (tiempoEstado > 0) {
                tiempoEstado -= qMin(dt, tiempoEstado);

                double t = 0.8 - tiempoEstado;
                double x = xInicioProyectil + (xObjetivoProyectil - xInicioProyectil) * t / 0.8;
                double y = 460 - 675 * t + 0.5 * 1800 * t * t;

                proyectil->setPos(x, y);
            }
            else {
                terminarAtaque();
            }
        }

        // Proyectil horizontal
        else {
            proyectil->moveBy(direccionAtaque * 480 * dt, 0);

            if (proyectil->sceneBoundingRect().right() < 0 || proyectil->sceneBoundingRect().left() > 900) {
                terminarAtaque();
            }
        }

        return;
    }

    // Actualizar el tiempo del estado actual
    tiempoEstado -= dt;

    if (tiempoEstado > 0) {
        return;
    }

    // Seleccionar y preparar el ataque
    if (estado == Esperando) {
        ataqueParabolico = razonar();
        puedeRegistrarEsquive = true;

        direccionAtaque = areaPo.center().x() < obtenerArea().center().x() ? -1 : 1;

        estado = Preparando;
        tiempoEstado = 0.6;
    }

    // Lanzar el proyectil seleccionado
    else if (estado == Preparando) {

        // Lanzar la arepa
        if (ataqueParabolico) {
            xInicioProyectil = obtenerArea().center().x() - 12;
            xObjetivoProyectil = qBound(0.0, areaPo.center().x() - 12, 876.0);

            proyectil->setPixmap(imagenArepa);
            proyectil->setPos(xInicioProyectil, 460);
            tiempoEstado = 0.8;
        }

        // Lanzar la pluma
        else {
            QPixmap sprite = imagenPluma;

            if (direccionAtaque == -1) {
                sprite = sprite.transformed(QTransform().scale(-1, 1));
            }

            proyectil->setPixmap(sprite);
            proyectil->setPos(obtenerArea().center().x() - 12, 450);
        }

        // Alinear la imagen dentro del espacio del proyectil
        proyectil->setOffset((24 - proyectil->pixmap().width()) / 2.0, 24 - proyectil->pixmap().height());

        proyectil->show();
        imagen->setOpacity(1);
        estado = Disparando;
    }
}

LordShen::~LordShen()
{
    delete proyectil;
}

bool LordShen::golpea(const QRectF &areaPo) const
{
    // Colision con el proyectil horizontal
    return estado == Disparando && !ataqueParabolico && proyectil->sceneBoundingRect().intersects(areaPo);
}

bool LordShen::ralentiza(const QRectF &areaPo)
{
    // Colision con el proyectil parabolico durante el descenso
    if (estado == Disparando && ataqueParabolico && tiempoEstado <= 0.425 && proyectil->sceneBoundingRect().intersects(areaPo)) {
        terminarAtaque();
        return true;
    }

    return false;
}

void LordShen::terminarAtaque()
{
    // Ocultar el proyectil y volver a esperar
    proyectil->hide();
    estado = Esperando;
    tiempoEstado = 1.5;
    puedeRegistrarEsquive = false;
}

void LordShen::actualizarAnimacion(double dt)
{
    QPixmap sprite;
    int direccion = direccionMovimiento;

    // Animacion del lanzamiento
    if (estado == Preparando) {
        int indice = qMin(3, int((0.6 - tiempoEstado) / 0.15));
        sprite = imagenLanzamiento[indice];
        direccion = direccionAtaque;
    }

    // Animacion del movimiento
    else {
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= 0.12) {
            fotograma = (fotograma + 1) % 5;
            tiempoAnimacion = 0;
        }

        sprite = imagenCaminar[fotograma];
    }

    // Orientar el sprite segun la direccion
    if (direccion == -1) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    // Mostrar el sprite y mantenerlo sobre el suelo
    imagen->setPixmap(sprite);
    imagen->setY(520 - imagen->boundingRect().height());
}