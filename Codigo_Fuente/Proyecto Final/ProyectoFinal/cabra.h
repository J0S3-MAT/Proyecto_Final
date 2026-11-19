#ifndef CABRA_H
#define CABRA_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>

class Cabra
{
private:
    QGraphicsPixmapItem *imagen;
    QPixmap imagenCorrer[6];

    double velocidad = -160;
    double tiempoChoque = 0;
    double tiempoAnimacion = 0;
    int fotograma = 0;
    double tiempoAparicion = 0;

public:
    Cabra(QGraphicsScene *escena);
    void actualizar(double dt, double xPo);
    QRectF obtenerArea() const;
    double obtenerVelocidad() const;
    void recibirChoque(double nuevaVelocidad);
};

#endif // CABRA_H
