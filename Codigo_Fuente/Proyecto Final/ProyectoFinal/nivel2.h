#ifndef NIVEL2_H
#define NIVEL2_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include "po.h"
#include <QTimer>
#include <QElapsedTimer>
#include "tailung.h"
#include "lordshen.h"
#include <QObject>

class Nivel2 : public QObject
{
    Q_OBJECT
private:
    QGraphicsScene *escena;
    Po *personaje;
    Enemigo *enemigo;
    QGraphicsRectItem *armazon;
    QGraphicsTextItem *textoEntregas;
    QGraphicsTextItem *textoVidas;
    int floresEntregadas = 0;
    double tiempoEspera = 0;
    bool entregaHabilitada = true;

    // Sprites de la canasta y la silleta
    QPixmap imagenCanasta[6];
    QPixmap imagenSilleta[6];

    QGraphicsPixmapItem *canasta;
    QGraphicsPixmapItem *silleta;

    void actualizarSpritesFlores();


    QTimer *temporizador = nullptr;
    QElapsedTimer reloj;

    void actualizarJuego();

public:
    Nivel2(bool modoDificil);
    ~Nivel2();

    QGraphicsScene *obtenerEscena() const;
    void presionarTecla(int tecla, bool repetida);
    void soltarTecla(int tecla, bool repetida);

signals:
    void nivelCompletado();
    void nivelPerdido(const QString &mensaje);};

#endif // NIVEL2_H

