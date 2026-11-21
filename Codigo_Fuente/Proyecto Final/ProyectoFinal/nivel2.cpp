#include "nivel2.h"
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFont>

Nivel2::Nivel2()
{
    // Crear el escenario fijo del nivel 2
    escena = new QGraphicsScene();
    escena->setSceneRect(0, 0, 900, 600);
    escena->setBackgroundBrush(QColor(195, 225, 245));

    // Crear el suelo
    escena->addRect(0, 520, 900, 80,
                    QPen(Qt::NoPen),
                    QBrush(QColor(100, 160, 80)));

    // Crear a Po en el segundo nivel
    personaje = new Po(escena);

    // Armazon provisional de la silleta
    armazon = escena->addRect(750, 320, 100, 200,
                              QPen(QColor(125, 75, 40), 7),
                              QBrush(Qt::NoBrush));

    // Titulo
    QGraphicsTextItem *titulo = escena->addText(
        "NIVEL 2 - LA BATALLA POR LA SILLETA",
        QFont("Arial", 20, QFont::Bold));

    titulo->setDefaultTextColor(Qt::darkGreen);
    titulo->setPos(100, 25);

    // Contador de flores entregadas
    textoEntregas = escena->addText(
        "Flores entregadas: 0/30",
        QFont("Arial", 16, QFont::Bold));

    textoEntregas->setDefaultTextColor(Qt::darkGreen);
    textoEntregas->setPos(20, 75);
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
    delete escena;
}

QGraphicsScene *Nivel2::obtenerEscena() const
{
    return escena;
}


void Nivel2::presionarTecla(int tecla, bool repetida)
{
    if (repetida) {
        return;
    }

    if (tecla == Qt::Key_A) {
        izquierda = true;
    }

    if (tecla == Qt::Key_D) {
        derecha = true;
    }
}

void Nivel2::soltarTecla(int tecla, bool repetida)
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


void Nivel2::actualizarJuego()
{
    // Calcular el tiempo transcurrido
    double dt = reloj.restart() / 1000.0;

    // Mover a Po horizontalmente
    personaje->mover(dt, izquierda, derecha, {});

    // Actualizar su animacion
    personaje->actualizarAnimacion(dt, izquierda, derecha);
}

