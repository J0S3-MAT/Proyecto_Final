#ifndef ENEMIGO_H
#define ENEMIGO_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <deque>

class Enemigo
{
private:
    // Memoria del agente inteligente
    std::deque<bool> historialEsquives;

    void aprender(bool salto);

protected:
    QGraphicsPixmapItem *imagen;

    // Control y razonamiento del agente
    bool puedeRegistrarEsquive = false;
    // Dificultad progresiva
    double factorDificultad = 1.0;

    virtual bool razonar();

public:
    Enemigo(QGraphicsScene *escena);
    virtual ~Enemigo();

    QRectF obtenerArea() const;
    virtual bool golpea(const QRectF &areaPo) const = 0;

    // Percepcion y accion del agente
    void percibir(bool salto);
    virtual void actuar(double dt, const QRectF &areaPo) = 0;
    void ajustarDificultad(int floresEntregadas);
    virtual bool ralentiza(const QRectF &) { return false; }
};

#endif