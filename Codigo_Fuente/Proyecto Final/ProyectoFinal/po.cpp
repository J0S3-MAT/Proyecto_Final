#include "po.h"
#include <QTransform>

Po::Po(QGraphicsScene *escena)
{
    // Imagen de reposo
    imagenReposo = QPixmap(":/imagenes/imagenes/po_reposo.png").scaledToHeight(100, Qt::SmoothTransformation);

    imagen = escena->addPixmap(imagenReposo);
    imagen->setPos(100, 320);

    // Imagenes para caminar
    for (int i = 0; i < 6; i++) {
        imagenCaminar[i] = QPixmap(QString(":/imagenes/imagenes/po_caminar%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    // Imagenes para saltar
    for (int i = 0; i < 3; i++) {
        imagenSalto[i] = QPixmap(QString(":/imagenes/imagenes/po_salto%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }
}


void Po::mover(double dt, bool izquierda, bool derecha)
{
    if (izquierda) {
        imagen->moveBy(-velocidad * dt, 0);
    }

    if (derecha) {
        imagen->moveBy(velocidad * dt, 0);
    }

    // Limites horizontales
    if (imagen->x() < 0) {
        imagen->setX(0);
    }

    double limiteDerecho = imagen->scene()->sceneRect().right();

    if (imagen->x() + imagen->boundingRect().width() > limiteDerecho) {
        imagen->setX(limiteDerecho - imagen->boundingRect().width());
    }
}

void Po::saltar()
{
    if (!saltando && !cayendo) {
        saltando = true;
        plataformaActual = nullptr;
        tiempoSalto = 0;
        yInicioSalto = imagen->y();
    }
}



void Po::actualizarMovimientoVertical(double dt, const std::vector<PlataformaOscilante*> &plataformas)
{
    double alturaPo = imagen->boundingRect().height();
    double anchoPo = imagen->boundingRect().width();

    // Po acompaña la plataforma en la que esta apoyado
    if (plataformaActual != nullptr) {
        bool coincideX = imagen->x() + anchoPo > plataformaActual->x() && imagen->x() < plataformaActual->x() + plataformaActual->rect().width();

        if (coincideX) {
            imagen->setY(plataformaActual->y() - alturaPo);
        }
        else {
            plataformaActual = nullptr;
            cayendo = true;
            velocidadCaida = 0;
        }
    }

    double yAnterior = imagen->y();

    // Movimiento parabolico
    if (saltando) {
        tiempoSalto += dt;
        double y = yInicioSalto - 500 * tiempoSalto + 0.5 * 1000 * tiempoSalto * tiempoSalto;
        imagen->setY(y);
    }

    // Caida por gravedad
    else if (cayendo) {
        velocidadCaida += 1000 * dt;
        imagen->moveBy(0, velocidadCaida * dt);
    }

    // Comprobar aterrizaje en cualquiera de las plataformas
    if ((saltando && tiempoSalto >= 0.5) || cayendo) {
        for (PlataformaOscilante *plataforma : plataformas) {
            QGraphicsRectItem *rectangulo = plataforma->obtenerRectangulo();

            bool coincideX = imagen->x() + anchoPo > rectangulo->x() && imagen->x() < rectangulo->x() + rectangulo->rect().width();
            bool coincideY = yAnterior + alturaPo <= rectangulo->y() && imagen->y() + alturaPo >= rectangulo->y();

            if (coincideX && coincideY) {
                imagen->setY(rectangulo->y() - alturaPo);
                plataformaActual = rectangulo;
                saltando = false;
                cayendo = false;
                velocidadCaida = 0;
                break;
            }
        }
    }

    // Aterrizaje en el suelo
    if ((saltando || cayendo) && imagen->y() + alturaPo >= 420) {
        imagen->setY(420 - alturaPo);
        saltando = false;
        cayendo = false;
        velocidadCaida = 0;
    }
}


void Po::actualizarAnimacion(double dt, bool izquierda, bool derecha)
{
    bool caminando = (izquierda != derecha) && !saltando && !cayendo;
    bool cambioDireccion = false;

    // Guardar la direccion hacia la que mira Po
    if (izquierda != derecha) {
        cambioDireccion = mirandoIzquierda != izquierda;
        mirandoIzquierda = izquierda;
    }

    // Animacion del salto y la caida
    if (saltando || cayendo) {
        QPixmap nuevaImagen;

        if (saltando) {
            double velocidadVertical = -500 + 1000 * tiempoSalto;

            if (velocidadVertical < -50) {
                nuevaImagen = imagenSalto[0];
            }
            else if (velocidadVertical <= 50) {
                nuevaImagen = imagenSalto[1];
            }
            else {
                nuevaImagen = imagenSalto[2];
            }
        }
        else {
            nuevaImagen = imagenSalto[2];
        }

        if (mirandoIzquierda) {
            nuevaImagen = nuevaImagen.transformed(QTransform().scale(-1, 1));
        }

        imagen->setPixmap(nuevaImagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }

    // Animacion al caminar
    else if (caminando) {
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= 0.12 || !estabaCaminando || cambioDireccion) {
            if (tiempoAnimacion >= 0.12) {
                fotograma = (fotograma + 1) % 6;
            }

            QPixmap nuevaImagen = imagenCaminar[fotograma];

            if (mirandoIzquierda) {
                nuevaImagen = nuevaImagen.transformed(QTransform().scale(-1, 1));
            }

            imagen->setPixmap(nuevaImagen);
            tiempoAnimacion = 0;
        }

        estabaCaminando = true;
    }

    // Animacion en reposo
    else {
        QPixmap nuevaImagen = imagenReposo;

        if (mirandoIzquierda) {
            nuevaImagen = nuevaImagen.transformed(QTransform().scale(-1, 1));
        }

        imagen->setPixmap(nuevaImagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }
}

QRectF Po::obtenerArea() const
{
    return imagen->sceneBoundingRect();
}

void Po::sumarFlor()
{
    flores++;
}

int Po::obtenerFlores() const
{
    return flores;
}