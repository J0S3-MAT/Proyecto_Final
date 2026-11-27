#ifndef LORDSHEN_H
#define LORDSHEN_H

#include "enemigo.h"
#include <array>

class LordShen : public Enemigo
{
private:
    // Estados del enemigo
    enum Estado {
        Esperando,
        Preparando,
        Disparando
    };

    Estado estado = Esperando;
    double tiempoEstado = 1.5;

    // Proyectil
    // Imagenes de los proyectiles
    QGraphicsPixmapItem *proyectil;
    QPixmap imagenArepa;
    QPixmap imagenPluma;
    int direccionAtaque = 1;

    // Movimiento del enemigo
    double velocidadMovimiento = 100;
    int direccionMovimiento = -1;

    // Ataque parabolico
    bool ataqueParabolico = false;
    double xInicioProyectil = 0;
    double xObjetivoProyectil = 0;

    // Animacion del movimiento
    // Animaciones del enemigo
    std::array<QPixmap, 5> imagenCaminar;
    std::array<QPixmap, 4> imagenLanzamiento;
    int fotograma = 0;
    double tiempoAnimacion = 0;



    void actualizarAnimacion(double dt);

    void terminarAtaque();

public:
    LordShen(QGraphicsScene *escena);
    ~LordShen() override;

    void actuar(double dt, const QRectF &areaPo) override;
    bool golpea(const QRectF &areaPo) const override;
    bool ralentiza(const QRectF &areaPo) override;
};

#endif