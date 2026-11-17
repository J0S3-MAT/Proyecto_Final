#include "flor.h"

Flor::Flor(QGraphicsScene *escena, double x, double y, QGraphicsItem *padre)
{
    QPixmap imagenFlor(":/imagenes/imagenes/flor.png");
    imagen = escena->addPixmap(imagenFlor.scaledToHeight(30, Qt::SmoothTransformation));

    if (padre != nullptr) {
        imagen->setParentItem(padre);
    }

    imagen->setPos(x, y);
}

bool Flor::recolectarSiToca(const QRectF &areaPo)
{
    if (recogida) {
        return false;
    }

    if (imagen->sceneBoundingRect().intersects(areaPo)) {
        recogida = true;
        imagen->hide();
        return true;
    }

    return false;
}