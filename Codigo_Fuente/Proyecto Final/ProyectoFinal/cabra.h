#ifndef CABRA_H
#define CABRA_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QGraphicsRectItem>
#include <vector>

class Cabra
{
private:
    QGraphicsPixmapItem *imagen;
    QPixmap imagenCorrer[6];
    // Animacion cuando la cabra toca la lava
    QPixmap imagenQuemando[3];
    bool quemando = false;
    int fotogramaQuemando = 0;
    double tiempoQuemando = 0;
    double velocidad = -160;
    double velocidadBase = -160;
    double tiempoChoque = 0;
    double tiempoAnimacion = 0;
    int fotograma = 0;
    double tiempoAparicion = 0;

public:
    Cabra(QGraphicsScene *escena);
    void actualizar(double dt, double xPo, const std::vector<QGraphicsRectItem*> &trampas);
    QRectF obtenerArea() const;
    double obtenerVelocidad() const;
    void recibirChoque(double nuevaVelocidad);
    void configurarDificultad(bool dificil);
};

#endif // CABRA_H
