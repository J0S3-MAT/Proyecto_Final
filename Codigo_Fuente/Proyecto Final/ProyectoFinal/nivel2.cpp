#include "nivel2.h"
#include <QApplication>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFont>

Nivel2::Nivel2(bool modoDificil)
{
    // Crear el escenario fijo del nivel 2
    escena = new QGraphicsScene();
    escena->setSceneRect(0, 0, 900, 600);
    // Cargar el fondo del nivel 2
    QPixmap fondo(":/imagenes/imagenes/fondoNVL2.png");
    fondo = fondo.scaled(900, 600, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QGraphicsPixmapItem *imagenFondo = escena->addPixmap(fondo);
    imagenFondo->setPos(0, 0);
    imagenFondo->setZValue(-10);

    // Crear a Po en el segundo nivel
    personaje = new Po(escena, true);
    personaje->sumarFlores(10);

    // Crear el enemigo segun la dificultad
    if (modoDificil) {
        enemigo = new TaiLung(escena);
    }
    else {
        enemigo = new LordShen(escena);
    }

    // Area de entrega de flores
    armazon = escena->addRect(750, 320, 100, 200, QPen(Qt::NoPen), QBrush(Qt::NoBrush));

    // Cargar los sprites de la canasta y la silleta
    for (int i = 0; i < 6; i++) {
        imagenCanasta[i] = QPixmap(QString(":/imagenes/imagenes/canasta%1.png").arg(i + 1)).scaledToHeight(100, Qt::FastTransformation);
        imagenSilleta[i] = QPixmap(QString(":/imagenes/imagenes/silleta%1.png").arg(i + 1)).scaledToHeight(200, Qt::FastTransformation);
    }

    // Crear la canasta y la silleta
    canasta = escena->addPixmap(imagenCanasta[0]);
    silleta = escena->addPixmap(imagenSilleta[0]);

    canasta->setZValue(-1);
    silleta->setZValue(-1);

    actualizarSpritesFlores();

    // Franja de la interfaz
    QGraphicsRectItem *fondoInterfaz = escena->addRect(0, 0, 900, 82, QPen(Qt::NoPen), QColor(34, 37, 34, 225));
    fondoInterfaz->setZValue(20);
    QGraphicsRectItem *bordeInterfaz = new QGraphicsRectItem(0, 80, 900, 2, fondoInterfaz);
    bordeInterfaz->setPen(Qt::NoPen);
    bordeInterfaz->setBrush(QColor("#C9A563"));

    QFont fuente = QApplication::font();
    fuente.setBold(true);
    auto agregarTexto = [&](const QString &texto, int tamano, const QColor &color) {
        fuente.setPixelSize(tamano);
        QGraphicsSimpleTextItem *elemento = new QGraphicsSimpleTextItem(texto, fondoInterfaz);
        elemento->setFont(fuente);
        elemento->setBrush(color);
        return elemento;
    };

    // Titulo
    QGraphicsSimpleTextItem *titulo = agregarTexto("SILLETA", 18, QColor("#D1B88D"));
    titulo->setPos(450 - titulo->boundingRect().width() / 2, 6);

    // Contador de flores entregadas
    textoEntregas = agregarTexto("00/10", 34, QColor("#FFF3D6"));
    textoEntregas->setPos(450 - textoEntregas->boundingRect().width() / 2, 31);

    // Contador de vidas
    QGraphicsSimpleTextItem *textoVidas = agregarTexto("VIDAS", 18, QColor("#D1B88D"));
    textoVidas->setPos(22, 6);
    QPixmap iconoVida = QPixmap(":/imagenes/imagenes/po_vida.png").scaledToHeight(30, Qt::FastTransformation);
    for (int i = 0; i < 5; i++) {
        iconosVidas[i] = new QGraphicsPixmapItem(iconoVida, fondoInterfaz);
        iconosVidas[i]->setPos(22 + i * (iconoVida.width() + 8), 34);
    }

    // Indicador del dash
    QGraphicsSimpleTextItem *etiquetaDash = agregarTexto("DASH", 18, QColor("#D1B88D"));
    etiquetaDash->setPos(780 - etiquetaDash->boundingRect().width() / 2, 6);
    textoDash = agregarTexto("", 20, QColor("#FFF3D6"));
    fondoBarraDash = new QGraphicsRectItem(0, 0, 156, 7, fondoInterfaz);
    fondoBarraDash->setPos(702, 65);
    fondoBarraDash->setPen(Qt::NoPen);
    fondoBarraDash->setBrush(QColor("#454945"));
    rellenoBarraDash = new QGraphicsRectItem(fondoBarraDash);
    rellenoBarraDash->setPen(Qt::NoPen);
    actualizarDash();

    // Indicador del estado de Po
    fondoEstado = escena->addRect(0, 0, 0, 32, QPen(Qt::NoPen), QColor(34, 37, 34, 225));
    fondoEstado->setPos(20, 92);
    fondoEstado->setZValue(20);
    QGraphicsSimpleTextItem *textoEstado = agregarTexto("¡RALENTIZADO!", 18, QColor("#87D7EC"));
    textoEstado->setParentItem(fondoEstado);
    textoEstado->setPos(10, 5);
    fondoEstado->setRect(0, 0, textoEstado->boundingRect().width() + 20, 32);
    fondoEstado->hide();

    // Temporizador para actualizar el segundo nivel
    temporizador = new QTimer(escena);

    QObject::connect(temporizador, &QTimer::timeout, escena, [this]() {
        actualizarJuego();
    });

    reloj.start();
    temporizador->start(16);
}

Nivel2::~Nivel2()
{
    delete personaje;
    delete enemigo;
    delete escena;
}

QGraphicsScene *Nivel2::obtenerEscena() const
{
    return escena;
}


void Nivel2::presionarTecla(int tecla, bool repetida)
{
    if (!temporizador->isActive()) {
        return;
    }

    // Enviar al agente las acciones de esquive de Po
    if (personaje->presionarTecla(tecla, repetida)) {
        enemigo->percibir(tecla == Qt::Key_Space);
    }
}

void Nivel2::soltarTecla(int tecla, bool repetida)
{
    if (!temporizador->isActive()) {
        return;
    }

    personaje->soltarTecla(tecla, repetida);
}


void Nivel2::actualizarJuego()
{
    double dt = reloj.restart() / 1000.0;

    personaje->mover(dt, {});
    personaje->actualizarMovimientoVertical(dt, {});
    personaje->actualizarAnimacion(dt);
    enemigo->actuar(dt, personaje->obtenerArea());


    // Detectar daño causado por el enemigo
    if (enemigo->golpea(personaje->obtenerArea())) {

        // Guardar las vidas antes de recibir el ataque
        int vidasAntes = personaje->obtenerVidas();

        personaje->recibirDanio(enemigo->obtenerVelocidadEmpuje());

        // Reproducir sonido solo si realmente perdio una vida
        if (personaje->obtenerVidas() < vidasAntes) {
            emit vidaPerdida();
        }
    }


    // Detectar ralentizacion causada por el enemigo
    if (enemigo->ralentiza(personaje->obtenerArea())) {
        personaje->ralentizar();
    }

    // Mostrar el estado de ralentizacion
    fondoEstado->setVisible(personaje->estaRalentizado());

    // Actualizar los iconos de vida
    for (int i = 0; i < 5; i++) {
        iconosVidas[i]->setVisible(i < personaje->obtenerVidas());
    }
    actualizarDash();

    // Comprobar derrota
    if (personaje->obtenerVidas() <= 0) {
        temporizador->stop();
        emit nivelPerdido("¡TE HAS QUEDADO SIN VIDAS!");
        return;
    }

    // Tiempo entre entregas
    if (tiempoEspera > 0) {
        tiempoEspera -= dt;
    }

    // Habilitar otra entrega al regresar
    if (personaje->obtenerArea().center().x() <= 180) {
        entregaHabilitada = true;
    }

    // Entregar flores a la silleta
    if (entregaHabilitada && tiempoEspera <= 0 && floresEntregadas < 10 && personaje->obtenerArea().intersects(armazon->sceneBoundingRect())) {
        personaje->quitarFlores(2);
        floresEntregadas += 2;
        entregaHabilitada = false;
        tiempoEspera = 3.0;

        // Avisar que Po entrego flores
        emit florEntregada();

        // Ajustar la dificultad del enemigo
        enemigo->ajustarDificultad(floresEntregadas);
        actualizarSpritesFlores();

        textoEntregas->setText(QString("%1/10").arg(floresEntregadas, 2, 10, QChar('0')));
        textoEntregas->setX(450 - textoEntregas->boundingRect().width() / 2);
    }

    // Comprobar si Po completo la silleta
    if (floresEntregadas >= 10) {
        temporizador->stop();
        emit nivelCompletado();
        return;
    }


}

void Nivel2::actualizarDash()
{
    bool listo = personaje->puedeIniciarDash();
    double progreso = personaje->obtenerProgresoRecargaDash();

    textoDash->setText(listo ? "LISTO" : (progreso < 1 ? "RECARGANDO" : "EN EL AIRE"));
    textoDash->setBrush(QColor(listo ? "#A8D57C" : "#FFF3D6"));
    textoDash->setPos(fondoBarraDash->x() + (fondoBarraDash->rect().width() - textoDash->boundingRect().width()) / 2, 31);
    rellenoBarraDash->setRect(0, 0, fondoBarraDash->rect().width() * progreso, fondoBarraDash->rect().height());
    rellenoBarraDash->setBrush(QColor(listo ? "#A8D57C" : "#C9A563"));
}

void Nivel2::actualizarSpritesFlores()
{
    // Seleccionar los sprites segun las flores entregadas
    int indice = floresEntregadas / 2;

    canasta->setPixmap(imagenCanasta[indice]);
    silleta->setPixmap(imagenSilleta[indice]);

    // Posicionar las imagenes sobre el suelo
    canasta->setPos(90 - canasta->boundingRect().width() / 2, 520 - canasta->boundingRect().height());
    silleta->setPos(800 - silleta->boundingRect().width() / 2, 520 - silleta->boundingRect().height());
}

