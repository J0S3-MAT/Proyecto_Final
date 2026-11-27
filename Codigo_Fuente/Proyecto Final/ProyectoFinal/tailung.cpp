#include "tailung.h"
#include <QTransform>
#include <QtGlobal>
#include <QRandomGenerator>
#include <QDebug>

TaiLung::TaiLung(QGraphicsScene *escena) : Enemigo(escena)
{
    // Cargar los sprites de los ataques
    for (int i = 0; i < 5; i++) {
        imagenEmbestida[i] = QPixmap(QString(":/imagenes/imagenes/tai_embistiendo%1.png").arg(i + 1)).scaledToHeight(70, Qt::FastTransformation);
    }

    for (int i = 0; i < 4; i++) {
        imagenParabolico[i] = QPixmap(QString(":/imagenes/imagenes/tai_parabolico%1.png").arg(i + 1)).scaledToHeight(130, Qt::FastTransformation);
    }

    // Cargar los sprites de preparacion
    for (int i = 0; i < 2; i++) {
        imagenPrepararSalto[i] = QPixmap(QString(":/imagenes/imagenes/tai_prep_saltar%1.png").arg(i + 1)).scaledToHeight(130, Qt::FastTransformation);
        imagenPrepararEmbestida[i] = QPixmap(QString(":/imagenes/imagenes/tai_prep_envestir%1.png").arg(i + 1)).scaledToHeight(70, Qt::FastTransformation);
    }

    // Mostrar el primer sprite en reposo
    imagen->setPixmap(imagenEmbestida[0]);
    imagen->setPos(400, 520 - imagen->boundingRect().height());
}

void TaiLung::actuar(double dt, const QRectF &areaPo)
{
    // Ajustar la velocidad del enemigo segun la dificultad
    dt *= factorDificultad;

    // Movimiento y animacion de la embestida
    if (estado == Embistiendo) {
        if (tiempoEstado > 0) {
            double paso = qMin(dt, tiempoEstado);
            imagen->moveBy(direccionAtaque * 550 * paso, 0);
            tiempoEstado -= paso;

            int indice = qMin(4, int((0.65 - tiempoEstado) / 0.13));
            mostrarSprite(imagenEmbestida[indice]);

            imagen->setY(520 - imagen->boundingRect().height());
            imagen->setX(qBound(0.0, imagen->x(), 900.0 - imagen->boundingRect().width()));
            return;
        }

        estado = Esperando;
        tiempoEstado = 1.5;
        mostrarSprite(imagenEmbestida[0]);
        imagen->setY(520 - imagen->boundingRect().height());
        return;
    }

    // Movimiento y animacion del ataque parabolico
    if (estado == Saltando) {
        if (tiempoEstado > 0) {
            tiempoEstado -= qMin(dt, tiempoEstado);

            double t = 0.6 - tiempoEstado;

            int indice = qMin(3, int(t / 0.15));
            mostrarSprite(imagenParabolico[indice]);

            double x = xInicioSalto + (xObjetivoSalto - xInicioSalto) * t / 0.6;
            double y = 520 - imagen->boundingRect().height() - 900 * t + 0.5 * 3000 * t * t;

            x = qBound(0.0, x, 900.0 - imagen->boundingRect().width());
            imagen->setPos(x, y);
            return;
        }

        estado = Esperando;
        tiempoEstado = 1.5;
        mostrarSprite(imagenEmbestida[0]);
        imagen->setY(520 - imagen->boundingRect().height());
        return;
    }

    // Actualizar el tiempo del estado actual
    tiempoEstado -= dt;

    // Animacion de preparacion del ataque
    if (estado == Preparando && tiempoEstado > 0) {
        int indice = tiempoEstado > 0.3 ? 0 : 1;
        mostrarSprite(ataqueSalto ? imagenPrepararSalto[indice] : imagenPrepararEmbestida[indice]);
        imagen->setY(520 - imagen->boundingRect().height());
        imagen->setX(qBound(0.0, imagen->x(), 900.0 - imagen->boundingRect().width()));
    }

    if (tiempoEstado > 0) {
        return;
    }

    // Seleccionar el ataque mediante el agente inteligente
    if (estado == Esperando) {
        ataqueSalto = razonar();
        puedeRegistrarEsquive = true;
        direccionAtaque = areaPo.center().x() < imagen->sceneBoundingRect().center().x() ? -1 : 1;

        estado = Preparando;
        tiempoEstado = 0.6;

        // Mostrar la preparacion del ataque
        mostrarSprite(ataqueSalto ? imagenPrepararSalto[0] : imagenPrepararEmbestida[0]);
        imagen->setY(520 - imagen->boundingRect().height());
        imagen->setX(qBound(0.0, imagen->x(), 900.0 - imagen->boundingRect().width()));
    }

    // Iniciar el ataque seleccionado
    else if (estado == Preparando) {
        if (ataqueSalto) {
            direccionAtaque = areaPo.center().x() < imagen->x() + imagenParabolico[0].width() / 2.0 ? -1 : 1;
            mostrarSprite(imagenParabolico[0]);
            imagen->setX(qBound(0.0, imagen->x(), 900.0 - imagen->boundingRect().width()));

            xInicioSalto = imagen->x();
            xObjetivoSalto = areaPo.center().x() - imagen->boundingRect().width() / 2;
            xObjetivoSalto = qBound(0.0, xObjetivoSalto, 900.0 - imagen->boundingRect().width());

            estado = Saltando;
            tiempoEstado = 0.6;
        }
        else {
            mostrarSprite(imagenEmbestida[0]);
            imagen->setX(qBound(0.0, imagen->x(), 900.0 - imagen->boundingRect().width()));

            estado = Embistiendo;
            tiempoEstado = 0.65;
        }
    }
}

bool TaiLung::golpea(const QRectF &areaPo) const
{
    // Colision durante la embestida
    if (estado == Embistiendo) {
        return obtenerArea().intersects(areaPo);
    }

    // Colision durante el descenso del salto
    if (estado == Saltando && tiempoEstado <= 0.3) {
        return obtenerArea().intersects(areaPo);
    }

    return false;
}

double TaiLung::obtenerVelocidadEmpuje() const
{
    // Empuje durante la embestida
    if (estado == Embistiendo) {
        return direccionAtaque * 350.0;
    }

    return 0;
}

void TaiLung::mostrarSprite(const QPixmap &sprite)
{
    // Orientar el sprite segun la direccion del ataque
    if (direccionAtaque == -1) {
        imagen->setPixmap(sprite.transformed(QTransform().scale(-1, 1)));
    }
    else {
        imagen->setPixmap(sprite);
    }
}



