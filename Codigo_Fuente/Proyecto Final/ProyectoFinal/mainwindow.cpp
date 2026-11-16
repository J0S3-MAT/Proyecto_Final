#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <cmath>
#include <QTransform> //para que camine
//para tener el menu
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QFont>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

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
    // Imagen de Po en reposo
    imagenReposo = QPixmap(":/imagenes/imagenes/po_reposo.png").scaledToHeight(100, Qt::SmoothTransformation);

    po = escena->addPixmap(imagenReposo);
    po->setPos(100, 320); //posición de la imagen

    // Cargar las 6 imagenes para caminar //para que camine
    for (int i = 0; i < 6; i++) {
        imagenCaminar[i] = QPixmap(QString(":/imagenes/imagenes/po_caminar%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    // Cargar las 3 imagenes para saltar
    for (int i = 0; i < 3; i++) {
        imagenSalto[i] = QPixmap(QString(":/imagenes/imagenes/po_salto%1.png").arg(i + 1)).scaledToHeight(100, Qt::SmoothTransformation);
    }

    plataforma = escena->addRect(0, 0, 120, 15, QPen(Qt::NoPen), QBrush(QColor(140, 90, 50)));
    plataforma->setPos(450, 340);

    // Temporizador del juego
    QTimer *temporizador = new QTimer(this);
    connect(temporizador, &QTimer::timeout, this, &MainWindow::actualizarJuego);

    // Crear la escena del menu
    escenaMenu = new QGraphicsScene(this);
    escenaMenu->setSceneRect(0, 0, 900, 500);
    escenaMenu->setBackgroundBrush(QColor(170, 220, 255));

    // Titulo del videojuego
    QGraphicsTextItem *titulo = escenaMenu->addText("PO SILLETERO", QFont("Arial", 36, QFont::Bold));
    titulo->setDefaultTextColor(QColor(30, 80, 45));
    titulo->setPos(270, 145);

    // Boton para iniciar el juego
    QPushButton *botonJugar = new QPushButton("JUGAR");
    botonJugar->setFixedSize(180, 55);

    QGraphicsProxyWidget *botonEscena = escenaMenu->addWidget(botonJugar);
    botonEscena->setPos(360, 260);

    // Iniciar el juego al presionar el boton
    connect(botonJugar, &QPushButton::clicked, this, [this, temporizador]() {
        ui->graphicsView->setScene(escena);
        juegoIniciado = true;
        reloj.start();
        temporizador->start(16);
        setFocus();
    });

    // Mostrar el menu al abrir el videojuego
    ui->graphicsView->setScene(escenaMenu);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A) {
        izquierda = true;
    }

    if (event->key() == Qt::Key_D) {
        derecha = true;
    }

    if (event->key() == Qt::Key_Space && !saltando && !cayendo && !event->isAutoRepeat()) {
        saltando = true;
        sobrePlataforma = false;
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

    // Po acompaña el movimiento de la plataforma
    if (sobrePlataforma && !saltando) {
        po->setY(plataforma->y() - po->boundingRect().height());
    }

    // Movimiento horizontal
    if (izquierda) {
        po->moveBy(-250 * dt, 0);
        mirandoIzquierda = true;
    }

    if (derecha) {
        po->moveBy(250 * dt, 0);
        mirandoIzquierda = false;
    }

    // Limites horizontales
    if (po->x() < 0) {
        po->setX(0);
    }

    if (po->x() + po->boundingRect().width() > 900) {
        po->setX(900 - po->boundingRect().width());
    }

    // Detectar si Po sale de la plataforma
    if (sobrePlataforma && !saltando) {
        bool coincideX = po->x() + po->boundingRect().width() > plataforma->x() && po->x() < plataforma->x() + plataforma->rect().width();

        if (!coincideX) {
            sobrePlataforma = false;
            cayendo = true;
            velocidadCaida = 0;
        }
    }

    // Caida de Po por gravedad
    if (cayendo) {
        velocidadCaida += 1000 * dt;
        po->moveBy(0, velocidadCaida * dt);

        // Detectar cuando Po llega al suelo
        if (po->y() + po->boundingRect().height() >= 420) {
            po->setY(420 - po->boundingRect().height());
            cayendo = false;
            velocidadCaida = 0;
        }
    }

    // Salto parabolico
    if (saltando) {
        double yAnterior = po->y();

        tiempoSalto += dt;
        double y = yInicioSalto - 500 * tiempoSalto + 0.5 * 1000 * tiempoSalto * tiempoSalto;
        po->setY(y);

        // Verificar si Po esta cayendo
        bool cayendo = tiempoSalto >= 0.5;

        // Verificar si Po coincide horizontalmente con la plataforma
        bool coincideX = po->x() + po->boundingRect().width() > plataforma->x() && po->x() < plataforma->x() + plataforma->rect().width();

        // Verificar si los pies de Po cruzan la superficie de la plataforma
        bool coincideY = yAnterior + po->boundingRect().height() <= plataforma->y() && po->y() + po->boundingRect().height() >= plataforma->y();

        // Aterrizaje sobre la plataforma
        if (cayendo && coincideX && coincideY) {
            po->setY(plataforma->y() - po->boundingRect().height());
            saltando = false;
            sobrePlataforma = true;
        }

        // Aterrizaje en el suelo
        else if (po->y() + po->boundingRect().height() >= 420) {
            po->setY(420 - po->boundingRect().height());
            saltando = false;
        }
    }

    //para que camine
    // Animacion de Po
    bool caminando = (izquierda != derecha) && !saltando && !cayendo;

    if (saltando || cayendo) {
        QPixmap imagen;

        if (saltando) {
            double velocidadVertical = -500 + 1000 * tiempoSalto;

            if (velocidadVertical < -50) {
                imagen = imagenSalto[0];
            }
            else if (velocidadVertical <= 50) {
                imagen = imagenSalto[1];
            }
            else {
                imagen = imagenSalto[2];
            }
        }
        else {
            imagen = imagenSalto[2];
        }

        if (mirandoIzquierda) {
            imagen = imagen.transformed(QTransform().scale(-1, 1));
        }

        po->setPixmap(imagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }
    else if (caminando) {
        tiempoAnimacion += dt;

        if (tiempoAnimacion >= 0.12 || !estabaCaminando) {
            if (tiempoAnimacion >= 0.12) {
                fotograma = (fotograma + 1) % 6;
            }

            QPixmap imagen = imagenCaminar[fotograma];

            if (mirandoIzquierda) {
                imagen = imagen.transformed(QTransform().scale(-1, 1));
            }

            po->setPixmap(imagen);
            tiempoAnimacion = 0;
        }

        estabaCaminando = true;
    }
    else {
        QPixmap imagen = imagenReposo;

        if (mirandoIzquierda) {
            imagen = imagen.transformed(QTransform().scale(-1, 1));
        }

        po->setPixmap(imagen);
        estabaCaminando = false;
        fotograma = 0;
        tiempoAnimacion = 0;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}




