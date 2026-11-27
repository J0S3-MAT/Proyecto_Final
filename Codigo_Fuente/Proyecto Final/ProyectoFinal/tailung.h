#ifndef TAILUNG_H
#define TAILUNG_H

#include "enemigo.h"
#include <QPixmap>
#include <array>

class TaiLung : public Enemigo
{
private:
    // Estados del enemigo
    enum Estado {
        Esperando,
        Preparando,
        Embistiendo,
        Saltando
    };

    Estado estado = Esperando;
    double tiempoEstado = 1.5;
    int direccionAtaque = 1;

    // Ataque parabolico
    bool ataqueSalto = false;
    double xInicioSalto = 0;
    double xObjetivoSalto = 0;

    // Sprites de los ataques
    std::array<QPixmap, 5> imagenEmbestida;
    std::array<QPixmap, 4> imagenParabolico;

    // Sprites de preparacion de los ataques
    std::array<QPixmap, 2> imagenPrepararSalto;
    std::array<QPixmap, 2> imagenPrepararEmbestida;

    void mostrarSprite(const QPixmap &sprite);



public:
    TaiLung(QGraphicsScene *escena);
    void actuar(double dt, const QRectF &areaPo) override;
    bool golpea(const QRectF &areaPo) const override;
    double obtenerVelocidadEmpuje() const override;

};

#endif