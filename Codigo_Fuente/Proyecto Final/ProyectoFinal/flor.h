#ifndef FLOR_H
#define FLOR_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QRectF>
#include <QGraphicsItem>

class Flor
{
private:
    QGraphicsPixmapItem *imagen;
    bool recogida = false;

public:
    Flor(QGraphicsScene *escena, double x, double y, QGraphicsItem *padre = nullptr);
    bool recolectarSiToca(const QRectF &areaPo);
};

#endif // FLOR_H