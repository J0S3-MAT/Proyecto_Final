#ifndef PLATAFORMAOSCILANTE_H
#define PLATAFORMAOSCILANTE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class PlataformaOscilante
{
private:
    QGraphicsRectItem *rectangulo;
    double centroY;
    double amplitud;
    double periodo;

public:
    PlataformaOscilante(QGraphicsScene *escena, double x, double y, double a, double t, double ancho);
    void actualizar(double tiempoJuego);
    QGraphicsRectItem *obtenerRectangulo();
};

#endif // PLATAFORMAOSCILANTE_H
