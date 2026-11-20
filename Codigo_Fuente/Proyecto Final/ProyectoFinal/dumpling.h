#ifndef DUMPLING_H
#define DUMPLING_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QRectF>

class Dumpling
{
private:
    QGraphicsPixmapItem *imagen;
    bool bueno;
    bool recogido = false;

public:
    Dumpling(QGraphicsScene *escena, double x, double y, bool esBueno);
    bool recolectarSiToca(const QRectF &areaPo);
    bool esBueno() const;
};

#endif // DUMPLING_H
