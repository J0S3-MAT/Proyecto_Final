#include "dumpling.h"
#include <QPixmap>

Dumpling::Dumpling(QGraphicsScene *escena, double x, double y, bool esBueno)
{
    bueno = esBueno;

    // Elegir la imagen segun el tipo de dumpling
    QString rutaImagen;

    if (bueno) {
        rutaImagen = ":/imagenes/imagenes/dumpling_bueno.png";
    } else {
        rutaImagen = ":/imagenes/imagenes/dumpling_malo.png";
    }

    // Cargar y ajustar el tamaño de la imagen
    QPixmap sprite(rutaImagen);
    sprite = sprite.scaled(45, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Agregar el dumpling al escenario
    imagen = escena->addPixmap(sprite);
    imagen->setPos(x, y);
}

bool Dumpling::recolectarSiToca(const QRectF &areaPo)
{
    if (recogido) {
        return false;
    }

    if (imagen->sceneBoundingRect().intersects(areaPo)) {
        recogido = true;
        imagen->hide();
        return true;
    }

    return false;
}

bool Dumpling::esBueno() const
{
    return bueno;
}
