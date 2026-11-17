#ifndef PO_H
#define PO_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QRectF>
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
    QPixmap imagenCaminar[6];
    QPixmap imagenSalto[3];

    // Control de animaciones
    int fotograma = 0;
    double tiempoAnimacion = 0;
    bool estabaCaminando = false;
    bool mirandoIzquierda = false;

    //flores
    int flores = 0;


public:
    Po(QGraphicsScene *escena);
    void mover(double dt, bool izquierda, bool derecha);
    void saltar();
    void actualizarMovimientoVertical(double dt, const std::vector<PlataformaOscilante*> &plataformas);
    void actualizarAnimacion(double dt, bool izquierda, bool derecha);
    QRectF obtenerArea() const;
    void sumarFlor();
    int obtenerFlores() const;
};


#endif // PO_H

