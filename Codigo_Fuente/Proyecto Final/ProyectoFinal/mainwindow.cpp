#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <cmath>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);

    // Crear el escenario
    escena = new QGraphicsScene(this);
    escena->setSceneRect(0, 0, 900, 500);

    // Mostrar el escenario en graphicsView
    ui->graphicsView->setScene(escena);

    // Color del cielo
    escena->setBackgroundBrush(QColor(170, 220, 255));

    // Dibujar un suelo provisional
    escena->addRect(0, 420, 900, 80,QPen(Qt::NoPen),QBrush(QColor(90, 170, 90)));


    //Abro la imagen
    QPixmap imagenPo(":/imagenes/imagenes/po_reposo.png");

    po = escena->addPixmap(imagenPo.scaledToHeight(100, Qt::SmoothTransformation)); //SmoothTransformation mejora la calidad del escalado
    po->setPos(100, 320); //Posicion de la imagen

    plataforma = escena->addRect(0, 0, 120, 15, QPen(Qt::NoPen), QBrush(QColor(140, 90, 50)));
    plataforma->setPos(450, 340);

    QTimer *temporizador = new QTimer(this);   //Se cre el temporizador
    connect(temporizador, &QTimer::timeout, this, &MainWindow::actualizarJuego); //Se conecta con la función actualizarJuego().
    reloj.start();
    temporizador->start(16); //cada 16 milisegundos se actva
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A) {
        izquierda = true;
    }

    if (event->key() == Qt::Key_D) {
        derecha = true;
    }

    if (event->key() == Qt::Key_Space && !saltando && !event->isAutoRepeat()) {
        saltando = true;
        tiempoSalto = 0;
        yInicioSalto = po->y();
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        return;
    }

    if (event->key() == Qt::Key_A) {
        izquierda = false;
    }

    if (event->key() == Qt::Key_D) {
        derecha = false;
    }
}

void MainWindow::actualizarJuego()
{
    double dt = reloj.restart() / 1000.0;

    tiempoJuego += dt;

    // Movimiento armonico simple de la plataforma
    const double centroY = 340;
    const double amplitud = 30;
    const double periodo = 2.5;

    double yPlataforma = centroY + amplitud * sin(2 * 3.1416 * tiempoJuego / periodo);
    plataforma->setY(yPlataforma);

    // Movimiento horizontal
    if (izquierda) {
        po->moveBy(-250 * dt, 0);
    }

    if (derecha) {
        po->moveBy(250 * dt, 0);
    }

    // Limites horizontales
    if (po->x() < 0) {
        po->setX(0);
    }

    if (po->x() + po->boundingRect().width() > 900) {
        po->setX(900 - po->boundingRect().width());
    }

    // Salto parabolico
    if (saltando) {
        tiempoSalto += dt;
        double y = yInicioSalto - 500 * tiempoSalto + 0.5 * 1000 * tiempoSalto * tiempoSalto;
        po->setY(y);

        if (po->y() >= yInicioSalto) {
            po->setY(yInicioSalto);
            saltando = false;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}




