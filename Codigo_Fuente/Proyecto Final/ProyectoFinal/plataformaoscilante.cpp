#include "plataformaoscilante.h"
#include <QBrush>
#include <QColor>
#include <QPen>
#include <cmath>
#include <QPixmap>

PlataformaOscilante::PlataformaOscilante(QGraphicsScene *escena, double x, double y, double a, double t, int tipo)
{
    centroY = y;
    amplitud = a;
    periodo = t;

    QPixmap sprite(QString(":/imagenes/imagenes/plataforma%1.png").arg(tipo));

    if (tipo == 1) {
        sprite = sprite.scaledToWidth(90, Qt::SmoothTransformation);
    }
    else if (tipo == 2) {
        sprite = sprite.scaledToWidth(130, Qt::SmoothTransformation);
    }
    else {
        sprite = sprite.scaledToWidth(180, Qt::SmoothTransformation);
    }


    rectangulo = escena->addRect(0, 0, sprite.width(), sprite.height(), QPen(Qt::NoPen), QBrush(sprite));
    rectangulo->setPos(x, y);
}

void PlataformaOscilante::actualizar(double tiempoJuego)
{
    double y = centroY + amplitud * std::sin(2 * 3.1416 * tiempoJuego / periodo);
    rectangulo->setY(y);
}

QGraphicsRectItem *PlataformaOscilante::obtenerRectangulo()
{
    return rectangulo;
}
