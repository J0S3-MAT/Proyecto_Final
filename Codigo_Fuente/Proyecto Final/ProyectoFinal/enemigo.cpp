#include "enemigo.h"
#include <QRandomGenerator>
#include <QDebug>

Enemigo::Enemigo(QGraphicsScene *escena)
{
    imagen = escena->addPixmap(QPixmap());
}

Enemigo::~Enemigo()
{
    delete imagen;
}

QRectF Enemigo::obtenerArea() const
{
    return imagen->sceneBoundingRect();
}

// Percepcion de las acciones del jugador
void Enemigo::percibir(bool salto)
{
    if (puedeRegistrarEsquive) {
        aprender(salto);
        puedeRegistrarEsquive = false;
    }
}

// Aprendizaje a partir de los esquives
void Enemigo::aprender(bool salto)
{
    historialEsquives.push_back(salto);

    // Conservar las ultimas diez acciones
    if (historialEsquives.size() > 10) {
        historialEsquives.pop_front();
    }
}

// Razonamiento para seleccionar el ataque
bool Enemigo::razonar()
{
    if (historialEsquives.empty()) {
        return QRandomGenerator::global()->bounded(100) < 50;
    }

    // Contar los saltos registrados
    int saltos = 0;

    for (bool salto : historialEsquives) {
        if (salto) {
            saltos++;
        }
    }

    // Calcular la probabilidad del ataque
    double probabilidad = 20 + 60.0 * saltos / historialEsquives.size();

    qDebug() << "Saltos:" << saltos << "Acciones:" << historialEsquives.size() << "Probabilidad:" << probabilidad;

    return QRandomGenerator::global()->bounded(100) < probabilidad;
}

void Enemigo::ajustarDificultad(int floresEntregadas)
{
    // Aumentar la dificultad segun las flores entregadas
    if (floresEntregadas >= 8) {
        factorDificultad = 1.30;
    }
    else if (floresEntregadas >= 4) {
        factorDificultad = 1.15;
    }
    else {
        factorDificultad = 1.0;
    }
}