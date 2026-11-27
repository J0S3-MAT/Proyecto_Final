#ifndef TAILUNG_H
#define TAILUNG_H

#include "enemigo.h"
#include <QPixmap>

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
    QPixmap imagenEmbestida[5];
    QPixmap imagenParabolico[5];

    void mostrarSprite(const QPixmap &sprite);



public:
    TaiLung(QGraphicsScene *escena);
    void actuar(double dt, const QRectF &areaPo) override;
    bool golpea(const QRectF &areaPo) const override;

};

#endif