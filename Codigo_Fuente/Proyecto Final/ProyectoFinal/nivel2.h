#ifndef NIVEL2_H
#define NIVEL2_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSimpleTextItem>
#include "po.h"
#include <QTimer>
#include <QElapsedTimer>
#include "tailung.h"
#include "lordshen.h"
#include <QObject>
#include <array>

class Nivel2 : public QObject
{
    Q_OBJECT
private:
    QGraphicsScene *escena;
    Po *personaje;
    Enemigo *enemigo;
    QGraphicsRectItem *armazon;
    QGraphicsSimpleTextItem *textoEntregas;
    std::array<QGraphicsPixmapItem*, 5> iconosVidas;
    QGraphicsSimpleTextItem *textoDash;
    QGraphicsRectItem *fondoBarraDash;
    QGraphicsRectItem *rellenoBarraDash;
    QGraphicsRectItem *fondoEstado;
    int floresEntregadas = 0;
    double tiempoEspera = 0;
    bool entregaHabilitada = true;

    // Sprites de la canasta y la silleta
    std::array<QPixmap, 6> imagenCanasta;
    std::array<QPixmap, 6> imagenSilleta;

    QGraphicsPixmapItem *canasta;
    QGraphicsPixmapItem *silleta;

    void actualizarSpritesFlores();


    QTimer *temporizador = nullptr;
    QElapsedTimer reloj;

    void actualizarJuego();
    void actualizarDash();

public:
    Nivel2(bool modoDificil);
    ~Nivel2();

    QGraphicsScene *obtenerEscena() const;
    void presionarTecla(int tecla, bool repetida);
    void soltarTecla(int tecla, bool repetida);

signals:
    void nivelCompletado();
    void nivelPerdido(const QString &mensaje);
    void florEntregada();
    void vidaPerdida();
};

#endif // NIVEL2_H

