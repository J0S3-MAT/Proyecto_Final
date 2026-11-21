#ifndef NIVEL2_H
#define NIVEL2_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include "po.h"
#include <QTimer>
#include <QElapsedTimer>

class Nivel2
{
private:
    QGraphicsScene *escena;
    Po *personaje;
    QGraphicsRectItem *armazon;
    QGraphicsTextItem *textoEntregas;
    bool izquierda = false;
    bool derecha = false;

    QTimer *temporizador = nullptr;
    QElapsedTimer reloj;

    void actualizarJuego();

public:
    Nivel2();
    ~Nivel2();

    QGraphicsScene *obtenerEscena() const;
    void presionarTecla(int tecla, bool repetida);
    void soltarTecla(int tecla, bool repetida);
};

#endif // NIVEL2_H

