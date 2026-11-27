#include "nivel2.h"
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

    // Titulo
    QGraphicsTextItem *titulo = escena->addText(
        "NIVEL 2 - LA BATALLA POR LA SILLETA",
        QFont("Arial", 20, QFont::Bold));

    titulo->setDefaultTextColor(Qt::darkGreen);
    titulo->setPos(100, 25);

    // Contador de flores entregadas
    textoEntregas = escena->addText(
        "Flores entregadas: 0/10",
        QFont("Arial", 16, QFont::Bold));

    textoEntregas->setDefaultTextColor(Qt::darkGreen);
    textoEntregas->setPos(20, 75);

    // Contador de vidas
    textoVidas = escena->addText("Vidas: 5/5", QFont("Arial", 16, QFont::Bold));
    textoVidas->setDefaultTextColor(Qt::darkRed);
    textoVidas->setPos(700, 75);

    // Indicador del estado de Po
    textoEstado = escena->addText("¡RALENTIZADO!", QFont("Arial", 14, QFont::Bold));
    textoEstado->setDefaultTextColor(Qt::darkCyan);
    textoEstado->setPos(20, 105);
    textoEstado->hide();

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

    // Detectar danio causado por el enemigo
    if (enemigo->golpea(personaje->obtenerArea())) {
        personaje->recibirDanio(enemigo->obtenerVelocidadEmpuje());
    }

    // Detectar ralentizacion causada por el enemigo
    if (enemigo->ralentiza(personaje->obtenerArea())) {
        personaje->ralentizar();
    }

    // Mostrar el estado de ralentizacion
    textoEstado->setVisible(personaje->estaRalentizado());

    // Actualizar vidas
    textoVidas->setPlainText(QString("Vidas: %1/5").arg(personaje->obtenerVidas()));

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

        // Ajustar la dificultad del enemigo
        enemigo->ajustarDificultad(floresEntregadas);
        actualizarSpritesFlores();

        textoEntregas->setPlainText(QString("Flores entregadas: %1/10").arg(floresEntregadas));
    }

    // Comprobar si Po completo la silleta
    if (floresEntregadas >= 10) {
        temporizador->stop();
        emit nivelCompletado();
        return;
    }


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

