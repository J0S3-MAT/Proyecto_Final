#include "po.h"
#include <QTransform>
#include <QKeyEvent>

static constexpr double duracionRecargaDash = 1.5;

Po::Po(QGraphicsScene *escena, bool permiteDash)
{
    dashHabilitado = permiteDash;
    // Imagen de reposo
    imagenReposo = QPixmap(":/imagenes/imagenes/po_reposo.png").scaledToHeight(100, Qt::SmoothTransformation);

    imagen = escena->addPixmap(imagenReposo);
    imagen->setPos(100, 420);

    // Imagenes para caminar
    for (int i = 0; i < 6; i++) {
        imagenCaminar[i] = QPixmap(QString(":/imagenes/imagenes/po_caminar%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    // Imagenes para saltar
    for (int i = 0; i < 3; i++) {
        imagenSalto[i] = QPixmap(QString(":/imagenes/imagenes/po_salto%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    // Imagenes de choque
    for (int i = 0; i < 4; i++) {
        imagenChoque[i] = QPixmap(QString(":/imagenes/imagenes/po_choque%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    // Imagenes del dash
    for (int i = 0; i < 5; i++) {
        imagenDash[i] = QPixmap(QString(":/imagenes/imagenes/po_dash%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }
}

bool Po::presionarTecla(int tecla, bool repetida)
{
    // Movimiento horizontal
    if (tecla == Qt::Key_A) {
        izquierda = true;
    }

    if (tecla == Qt::Key_D) {
        derecha = true;
    }

    // Comprobar si se realiza un salto
    if (tecla == Qt::Key_Space && !repetida) {
        bool estabaSaltando = saltando;
        saltar();
        return !estabaSaltando && saltando;
    }

    // Comprobar si se realiza un dash
    if (tecla == Qt::Key_Shift && !repetida) {
        bool estabaHaciendoDash = tiempoDash > 0;
        iniciarDash();
        return !estabaHaciendoDash && tiempoDash > 0;
    }

    return false;
}

void Po::soltarTecla(int tecla, bool repetida)
{
    if (repetida) {
        return;
    }

    if (tecla == Qt::Key_A) {
        izquierda = false;
    }

    if (tecla == Qt::Key_D) {
        derecha = false;
    }
}

void Po::mover(double dt, const std::vector<PlataformaOscilante*> &plataformas)
{

    // Actualizar duracion del estado pesado
    if (tiempoPesado > 0) {
        tiempoPesado -= dt;
    }

    // Guardar la posicion antes de moverse
    double xAnterior = imagen->x();

    // Recarga invulnerable
    if (tiempoInvulnerable > 0) {
        tiempoInvulnerable -= dt;
    }

    // Recarga del dash
    if (tiempoRecargaDash > 0 && tiempoDash <= 0) {
        tiempoRecargaDash -= dt;
    }

    // Actualizar duracion de la velocidad adicional
    if (tiempoRapido > 0) {
        tiempoRapido -= dt;
    }

    if (tiempoChoque > 0) {
        imagen->moveBy(velocidadChoque * dt, 0);
        tiempoChoque -= dt;
    }
    else if (tiempoDash > 0) {
        double paso = qMin(dt, tiempoDash);
        imagen->moveBy(direccionDash * 750 * paso, 0);
        tiempoDash -= paso;
    }
    else {

        double velocidadActual = velocidad;

        // Estado Pesado
        if (tiempoPesado > 0) {
            velocidadActual = velocidad * 0.7;
        }

        // Velocidad adicional del dumpling bueno
        if (tiempoRapido > 0) {
            velocidadActual *= 1.10;
        }

        if (izquierda) {
            imagen->moveBy(-velocidadActual * dt, 0);
        }

        if (derecha) {
            imagen->moveBy(velocidadActual * dt, 0);
        }
    }


    // Evitar que Po atraviese las plataformas por los lados
    double desplazamiento = imagen->x() - xAnterior;
    double anchoPo = imagen->boundingRect().width();
    QRectF areaPo = imagen->sceneBoundingRect();

    for (PlataformaOscilante *plataforma : plataformas) {

        QRectF areaPlataforma = plataforma->obtenerRectangulo()->sceneBoundingRect();

        // Verificar si Po esta a la misma altura de la plataforma
        bool coincideAltura = areaPo.bottom() > areaPlataforma.top() + 2 &&
                              areaPo.top() < areaPlataforma.bottom();

        if (!coincideAltura) {
            continue;
        }

        // Po llega desde la izquierda
        if (desplazamiento > 0 &&
            xAnterior < areaPlataforma.left() &&
            areaPo.right() > areaPlataforma.left()) {

            imagen->setX(areaPlataforma.left() - anchoPo);
            break;
        }

        // Po llega desde la derecha
        if (desplazamiento < 0 &&
            xAnterior + anchoPo > areaPlataforma.right() &&
            areaPo.left() < areaPlataforma.right()) {

            imagen->setX(areaPlataforma.right());
            break;
        }
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

void Po::iniciarDash()
{
    if (!puedeIniciarDash()) {
        return;
    }

    if (izquierda != derecha) {
        mirandoIzquierda = izquierda;
    }

    direccionDash = mirandoIzquierda ? -1 : 1;
    tiempoDash = 0.3;
    tiempoRecargaDash = duracionRecargaDash;
}

bool Po::puedeIniciarDash() const
{
    return dashHabilitado && !(tiempoDash > 0 || tiempoRecargaDash > 0 || saltando || cayendo);
}

double Po::obtenerProgresoRecargaDash() const
{
    return qBound(0.0, 1.0 - tiempoRecargaDash / duracionRecargaDash, 1.0);
}

void Po::saltar()
{
    if (!saltando && !cayendo && tiempoDash <= 0) {
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
    if ((saltando || cayendo) && imagen->y() + alturaPo >= 520) {
        imagen->setY(520 - alturaPo);
        saltando = false;
        cayendo = false;
        velocidadCaida = 0;
    }
}


void Po::actualizarAnimacion(double dt)
{
    // Parpadeo durante la invulnerabilidad
    if (dashHabilitado) {
        if (tiempoInvulnerable > 0) {
            imagen->setOpacity(int(tiempoInvulnerable * 10) % 2 == 0 ? 1.0 : 0.35);
        }
        else {
            imagen->setOpacity(1.0);
        }
    }

    bool caminando = (izquierda != derecha) && !saltando && !cayendo;
    bool cambioDireccion = false;

    // Guardar la direccion hacia la que mira Po
    if (izquierda != derecha) {
        cambioDireccion = mirandoIzquierda != izquierda;
        mirandoIzquierda = izquierda;
    }


    // Animacion de choque
    if (tiempoChoque > 0) {
        int indice = ((0.4 - tiempoChoque) / 0.1);

        if (indice > 3) {
            indice = 3;
        }

        QPixmap nuevaImagen = imagenChoque[indice];

        if (mirandoIzquierda) {
            nuevaImagen = nuevaImagen.transformed(QTransform().scale(-1, 1));
        }

        imagen->setPixmap(nuevaImagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }

    // Animacion del dash
    else if (tiempoDash > 0) {
        int indice = (0.3 - tiempoDash) / 0.06;

        if (indice > 4) {
            indice = 4;
        }

        QPixmap nuevaImagen = imagenDash[indice];

        if (direccionDash == -1) {
            nuevaImagen = nuevaImagen.transformed(QTransform().scale(-1, 1));
        }

        imagen->setPixmap(nuevaImagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }

    // Animacion del salto y la caida
    else if (saltando || cayendo) {
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
    QRectF area = imagen->sceneBoundingRect();

    // Reducir la hitbox en el nivel 2
    if (dashHabilitado) {
        double ancho = area.width();
        double alto = area.height();

        return area.adjusted(ancho * 0.2, alto * 0.1, -ancho * 0.2, 0);
    }

    return area;
}

void Po::sumarFlor()
{
    flores++;
}

void Po::sumarFlores(int cantidad)
{
    flores += cantidad;
}

void Po::quitarFlores(int cantidad)
{
    flores -= cantidad;

    if (flores < 0) {
        flores = 0;
    }
}

void Po::activarVelocidad()
{
    tiempoRapido = 4.0;
}

int Po::obtenerFlores() const
{
    return flores;
}

double Po::recibirChoque(double velocidadCabra)
{
    const double masaPo = 80;
    const double masaCabra = 100;

    double velocidadPo = 0;

    if (izquierda) {
        velocidadPo -= velocidad;
    }

    if (derecha) {
        velocidadPo += velocidad;
    }

    velocidadChoque = ((masaPo - masaCabra) * velocidadPo + 2 * masaCabra * velocidadCabra) / (masaPo + masaCabra);
    tiempoChoque = 0.4;

    // Penalizacion por choque
    flores -= 2;

    if (flores < 0) {
        flores = 0;
    }

    tiempoPesado = 3.0;

    return (2 * masaPo * velocidadPo + (masaCabra - masaPo) * velocidadCabra) / (masaPo + masaCabra);
}

void Po::recibirDanio(double empuje)
{
    if (tiempoInvulnerable <= 0 && vidas > 0) {
        vidas--;
        tiempoInvulnerable = 1.0;

        // Aplicar el empuje del enemigo
        if (empuje != 0) {
            velocidadChoque = empuje;
            tiempoChoque = 0.4;
            tiempoDash = 0;
        }
    }
}

int Po::obtenerVidas() const
{
    return vidas;
}

void Po::ralentizar()
{
    // Activar el estado pesado
    tiempoPesado = 3.0;
}

bool Po::estaRalentizado() const
{
    return tiempoPesado > 0;
}


void Po::mostrarQuemado()
{
    QPixmap sprite(":/imagenes/imagenes/po_quemado.png");

    if (sprite.isNull()) {
        return;
    }

    sprite = sprite.scaledToHeight(100, Qt::SmoothTransformation);

    if (mirandoIzquierda) {
        sprite = sprite.transformed(QTransform().scale(-1, 1));
    }

    imagen->setPixmap(sprite);

    // Mostrar a Po por encima de la lava
    imagen->setZValue(5);
}
