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
//para plataformas aleatorias
#include <QRandomGenerator>

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
    escena->setSceneRect(0, 0, 2400, 500);

    // Mostrar el escenario en graphicsView
    ui->graphicsView->setScene(escena);

    // Color del cielo
    escena->setBackgroundBrush(QColor(170, 220, 255));

    // Dibujar un suelo provisional
    escena->addRect(0, 420, 2400, 80, QPen(Qt::NoPen), QBrush(QColor(90, 170, 90)));


    //Abro la imagen
    // Crear a Po mediante su propia clase
    personaje = new Po(escena);
    // Crear la cabra
    cabra = new Cabra(escena);

    // Crear 5 plataformas con parametros aleatorios
    for (int i = 0; i < 5; i++) {
        int x = 300 + i * 400 + QRandomGenerator::global()->bounded(-70, 71);
        int y = QRandomGenerator::global()->bounded(325, 351);
        int amplitud = QRandomGenerator::global()->bounded(15, 26);
        double periodo = 2.0 + QRandomGenerator::global()->bounded(0, 16) / 10.0;

        plataformas.push_back(new PlataformaOscilante(escena, x, y, amplitud, periodo));
    }

    // Crear 15 flores en el suelo
    for (int x = 150; x <= 2250; x += 150) {
        int variacion = QRandomGenerator::global()->bounded(-40, 41);
        flores.push_back(new Flor(escena, x + variacion, 390));
    }

    // Crear tres flores sobre cada plataforma
    for (PlataformaOscilante *plataforma : plataformas) {
        for (int x = 15; x <= 75; x += 30) {
            flores.push_back(new Flor(escena, x, -30, plataforma->obtenerRectangulo()));
        }
    }

    // Contador de flores
    textoFlores = escena->addText("Flores: 0/30", QFont("Arial", 16, QFont::Bold));
    textoFlores->setDefaultTextColor(Qt::darkGreen);
    textoFlores->setPos(20, 15);

    // Contador de tiempo
    textoTiempo = escena->addText("Tiempo: 90 s", QFont("Arial", 16, QFont::Bold));
    textoTiempo->setDefaultTextColor(Qt::darkGreen);
    textoTiempo->setPos(710, 15);

    // Temporizador del juego
    temporizador = new QTimer(this);
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
    connect(botonJugar, &QPushButton::clicked, this, [this]() {
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
    if (!juegoIniciado) {
        return;
    }

    if (event->key() == Qt::Key_A) {
        izquierda = true;
    }

    if (event->key() == Qt::Key_D) {
        derecha = true;
    }

    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        personaje->saltar();
    }
}


void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!juegoIniciado) {
        return;
    }

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

    // Actualizar el tiempo restante
    tiempoRestante -= dt;

    if (tiempoRestante < 0) {
        tiempoRestante = 0;
    }

    // Mostrar el tiempo en pantalla
    textoTiempo->setPlainText(QString("Tiempo: %1 s").arg(static_cast<int>(std::ceil(tiempoRestante))));

    // Verificar si se termino el tiempo
    if (tiempoRestante <= 0) {
        temporizador->stop();
        juegoIniciado = false;

        if (personaje->obtenerFlores() < 30) {
            QGraphicsTextItem *mensaje = escena->addText("TIEMPO AGOTADO\nNo reuniste las 30 flores", QFont("Arial", 22, QFont::Bold));
            mensaje->setDefaultTextColor(Qt::darkRed);
            mensaje->setPos(220, 190);
            mensaje->setZValue(10);
        }

        return;
    }

    // Movimiento armonico simple de la plataforma
    for (PlataformaOscilante *plataforma : plataformas) {
        plataforma->actualizar(tiempoJuego);
    }

    // Movimiento horizontal de Po
    personaje->mover(dt, izquierda, derecha);

    // Movimiento de la cabra
    cabra->actualizar(dt);

    // Actualizar salto, caida y contacto con la plataforma
    personaje->actualizarMovimientoVertical(dt, plataformas);

    // Recoleccion de flores
    for (Flor *flor : flores) {
        if (flor->recolectarSiToca(personaje->obtenerArea())) {
            personaje->sumarFlor();
            textoFlores->setPlainText(QString("Flores: %1/30").arg(personaje->obtenerFlores()));
        }
    }

    // Verificar si Po completo el primer nivel
    if (personaje->obtenerFlores() >= 30) {
        temporizador->stop();
        juegoIniciado = false;

        QGraphicsTextItem *mensaje = escena->addText("¡NIVEL 1 COMPLETADO!", QFont("Arial", 24, QFont::Bold));
        mensaje->setDefaultTextColor(Qt::darkGreen);
        mensaje->setZValue(10);

        // Mostrar el mensaje en el centro de la pantalla
        QPointF centro = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
        mensaje->setPos(centro.x() - mensaje->boundingRect().width() / 2, centro.y() - mensaje->boundingRect().height() / 2);

        return;
    }

    //para que camine
    // Actualizar las animaciones de Po
    personaje->actualizarAnimacion(dt, izquierda, derecha);

    // Camara horizontal
    ui->graphicsView->centerOn(personaje->obtenerArea().center().x(), 250);

    // Mantener los contadores visibles
    QPointF esquinaVista = ui->graphicsView->mapToScene(0, 0);

    textoFlores->setPos(esquinaVista.x() + 20, esquinaVista.y() + 15);
    textoTiempo->setPos(esquinaVista.x() + 710, esquinaVista.y() + 15);
}

MainWindow::~MainWindow()
{
    delete personaje;
    // Liberar las plataformas
    for (PlataformaOscilante *plataforma : plataformas) {
        delete plataforma;
    }
    // Liberar la memoria de las flores
    for (Flor *flor : flores) {
        delete flor;
    }
    delete cabra;
    delete ui;
}


