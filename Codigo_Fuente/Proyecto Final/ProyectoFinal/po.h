#ifndef PO_H
#define PO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QRectF>
#include <array>
#include <vector>
#include "plataformaoscilante.h"

class Po
{
private:
    QGraphicsPixmapItem *imagen;
    double velocidad = 250;
    bool saltando = false;
    bool cayendo = false;
    double tiempoSalto = 0;
    double yInicioSalto = 0;
    double velocidadCaida = 0;
    QGraphicsRectItem *plataformaActual = nullptr;
    // Imagenes de Po
    QPixmap imagenReposo;
    std::array<QPixmap, 6> imagenCaminar;
    std::array<QPixmap, 3> imagenSalto;
    std::array<QPixmap, 4> imagenChoque;
    std::array<QPixmap, 5> imagenDash;

    // Control de animaciones
    int fotograma = 0;
    double tiempoPesado = 0;
    double tiempoRapido = 0;
    double tiempoAnimacion = 0;
    bool estabaCaminando = false;
    bool mirandoIzquierda = false;

    //flores
    int flores = 0;

    //Choque
    double velocidadChoque = 0;
    double tiempoChoque = 0;

    bool izquierda = false;
    bool derecha = false;

    // Dash
    bool dashHabilitado = false;
    double tiempoDash = 0;
    double tiempoRecargaDash = 0;
    int direccionDash = 1;

    // Sistema de vida
    int vidas = 5;
    double tiempoInvulnerable = 0;


public:
    Po(QGraphicsScene *escena, bool permiteDash = false);
    void iniciarDash();
    bool puedeIniciarDash() const;
    double obtenerProgresoRecargaDash() const;
    void mover(double dt, const std::vector<PlataformaOscilante*> &plataformas);
    void saltar();
    void actualizarMovimientoVertical(double dt, const std::vector<PlataformaOscilante*> &plataformas);
    void actualizarAnimacion(double dt);
    QRectF obtenerArea() const;
    void sumarFlor();
    void sumarFlores(int cantidad);
    void quitarFlores(int cantidad);
    void activarVelocidad();
    int obtenerFlores() const;
    double recibirChoque(double velocidadCabra);
    void mostrarQuemado();

    bool presionarTecla(int tecla, bool repetida);
    void soltarTecla(int tecla, bool repetida);

    void recibirDanio(double empuje = 0);
    int obtenerVidas() const;

    void ralentizar();
    bool estaRalentizado() const;
};


#endif // PO_H

