#include "plataformaoscilante.h"
#include <QBrush>
#include <QColor>
#include <QPen>
#include <cmath>

PlataformaOscilante::PlataformaOscilante(QGraphicsScene *escena, double x, double y, double a, double t)
{
    centroY = y;
    amplitud = a;
    periodo = t;

    rectangulo = escena->addRect(0, 0, 120, 15, QPen(Qt::NoPen), QBrush(QColor(140, 90, 50)));
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
