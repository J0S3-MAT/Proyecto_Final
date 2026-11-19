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
#include <QResizeEvent> //Ajustar la ventana

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Boton para reiniciar el nivel
    botonReiniciar = new QPushButton("REINICIAR", ui->graphicsView->viewport());
    botonReiniciar->setGeometry(10, 55, 120, 35);
    botonReiniciar->setFocusPolicy(Qt::NoFocus);
    botonReiniciar->hide();

    connect(botonReiniciar, &QPushButton::clicked, this, [this]() {
        MainWindow *nueva = new MainWindow();
        nueva->setAttribute(Qt::WA_DeleteOnClose);
        nueva->setGeometry(geometry());
        nueva->show();
        nueva->iniciarNivel();
        close();
    });

    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    // Crear el escenario
    escena = new QGraphicsScene(this);
    escena->setSceneRect(0, 70, 35000, 600);

    // Mostrar el escenario en graphicsView
    ui->graphicsView->setScene(escena);


    // Color del cielo
    QPixmap fondo(":/imagenes/imagenes/fondo2.png");
    escena->setBackgroundBrush(QBrush(fondo.scaled(fondo.width() * 0.65, fondo.height() * 0.65, Qt::KeepAspectRatio, Qt::SmoothTransformation)));


    //Abro la imagen
    // Crear a Po mediante su propia clase
    personaje = new Po(escena);
    // Crear la cabra
    cabra = new Cabra(escena);
    // Imagenes de la montana
    for (int i = 0; i < 4; i++) {
        imagenMontana[i] = QPixmap(QString(":/imagenes/imagenes/montana%1.png").arg(i + 1)).scaledToHeight(350, Qt::SmoothTransformation);
    }

    // Montana de dumplings animada
    montana = escena->addPixmap(imagenMontana[0]);
    montana->setPos(-650, 190);

    // Cargar animacion de lava
    for (int i = 0; i < 5; i++) {
        QPixmap sprite(QString(":/imagenes/imagenes/lava%1.png").arg(i + 1));
        imagenLava[i] = sprite.scaledToHeight(60, Qt::FastTransformation);
    }

    // Crear plataformas aleatorias
    for (int x = 350; x < 35000;) {

        // Probabilidad de generar una escalera
        int inicioTrampa = x;
        if (QRandomGenerator::global()->bounded(100) < 50) {

            for (int i = 0; i < 3; i++) {
                int yEscalon = 400 - i * 70;

                PlataformaOscilante *escalon = new PlataformaOscilante(escena, x, yEscalon, 0, 3.0, 1);
                plataformas.push_back(escalon);

                x += escalon->obtenerRectangulo()->rect().width() + 80;
            }

            int anchoTrampa = x - inicioTrampa - 40;

            QGraphicsRectItem *trampa = escena->addRect(0, 0, anchoTrampa, 60, QPen(Qt::NoPen), QBrush(imagenLava[0]));
            trampa->setPos(inicioTrampa, 520);
            trampas.push_back(trampa);

            x += QRandomGenerator::global()->bounded(250, 401);
            continue;
        }

        int tipo = QRandomGenerator::global()->bounded(1, 4);
        int y = QRandomGenerator::global()->bounded(425, 451);
        int amplitud = QRandomGenerator::global()->bounded(15, 26);
        double periodo = 2.0 + QRandomGenerator::global()->bounded(0, 16) / 10.0;

        PlataformaOscilante *plataforma = new PlataformaOscilante(escena, x, y, amplitud, periodo, tipo);
        plataformas.push_back(plataforma);

        // Algunas plataformas tienen una flor
        if (QRandomGenerator::global()->bounded(100) < 70) {
            double ancho = plataforma->obtenerRectangulo()->rect().width();
            flores.push_back(new Flor(escena, (ancho - 30) / 2, -30, plataforma->obtenerRectangulo()));
        }

        int anchoPlataforma = plataforma->obtenerRectangulo()->rect().width();
        x += anchoPlataforma + QRandomGenerator::global()->bounded(250, 400);
    }

    // Crear flores aleatorias en el suelo
    for (int x = 160; x < 34500; x += QRandomGenerator::global()->bounded(450, 751)) {
        flores.push_back(new Flor(escena, x, 490));
    }

    // Contador de flores
    textoFlores = escena->addText("Flores: 0/30", QFont("Arial", 16, QFont::Bold));
    textoFlores->setDefaultTextColor(Qt::darkGreen);
    textoFlores->setPos(20, 15);

    // Contador de tiempo
    textoTiempo = escena->addText("Tiempo: 60 s", QFont("Arial", 16, QFont::Bold));
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
        iniciarNivel();
    });

    // Mostrar el menu al abrir el videojuego
    ui->graphicsView->setScene(escenaMenu);

    //Mostrar bien dimensiones
    QTimer::singleShot(0, this, [this]() {
        ui->graphicsView->fitInView(escenaMenu->sceneRect(), Qt::KeepAspectRatio);
    });
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

    // Animacion de lava
    tiempoAnimacionLava += dt;

    if (tiempoAnimacionLava >= 0.12) {
        tiempoAnimacionLava -= 0.12;
        fotogramaLava = (fotogramaLava + 1) % 5;

        for (QGraphicsRectItem *trampa : trampas) {
            trampa->setBrush(QBrush(imagenLava[fotogramaLava]));
        }
    }

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
        mostrarDerrota("TIEMPO AGOTADO\nNo reuniste las 30 flores");
        return;
    }

    // Movimiento armonico simple de la plataforma
    for (PlataformaOscilante *plataforma : plataformas) {
        plataforma->actualizar(tiempoJuego);
    }

    // Movimiento horizontal de Po
    personaje->mover(dt, izquierda, derecha);

    // Colision con la lava
    QRectF areaPo = personaje->obtenerArea();

    for (QGraphicsRectItem *trampa : trampas) {
        QRectF areaTrampa = trampa->sceneBoundingRect();

        if (areaPo.right() > areaTrampa.left() && areaPo.left() < areaTrampa.right() && areaPo.bottom() >= 520) {
            mostrarDerrota("¡CAISTE EN LA LAVA!");
            return;
        }
    }

    // Movimiento de la cabra
    cabra->actualizar(dt, personaje->obtenerArea().center().x());

    // Actualizar salto, caida y contacto con la plataforma
    personaje->actualizarMovimientoVertical(dt, plataformas);

    // Colision elastica entre Po y la cabra
    bool tocandoCabra = personaje->obtenerArea().intersects(cabra->obtenerArea());

    if (tocandoCabra && !contactoCabra) {
        double nuevaVelocidadCabra = personaje->recibirChoque(cabra->obtenerVelocidad(), izquierda, derecha);
        cabra->recibirChoque(nuevaVelocidadCabra);
        textoFlores->setPlainText(QString("Flores: %1/30").arg(personaje->obtenerFlores()));
    }

    contactoCabra = tocandoCabra;

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
    ui->graphicsView->centerOn(personaje->obtenerArea().center().x(), 300);

    // Mantener los contadores visibles
    QPointF esquinaVista = ui->graphicsView->mapToScene(0, 0);

    textoFlores->setPos(esquinaVista.x() + 20, esquinaVista.y() + 15);
    textoTiempo->setPos(esquinaVista.x() + 710, esquinaVista.y() + 15);

    // Movimiento de la montana
    double distanciaMontana = personaje->obtenerArea().left() - montana->sceneBoundingRect().right();

    double velocidadMontana = 200;

    if (distanciaMontana > 450) {
        velocidadMontana = 350;
    }

    montana->moveBy(velocidadMontana * dt, 0);

    // Animacion de la montana
    tiempoAnimacionMontana += dt;

    if (tiempoAnimacionMontana >= 0.12) {
        fotogramaMontana = (fotogramaMontana + 1) % 4;
        montana->setPixmap(imagenMontana[fotogramaMontana]);
        tiempoAnimacionMontana = 0;
    }

    // Comprobar si la montana alcanza a Po
    if (montana->sceneBoundingRect().right() >= personaje->obtenerArea().left()) {
        mostrarDerrota("¡LA MONTAÑA TE ALCANZÓ!");
        return;
    }
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

void MainWindow::iniciarNivel()
{
    ui->graphicsView->setScene(escena);
    ui->graphicsView->fitInView(QRectF(0, 0, 900, 500), Qt::KeepAspectRatio); //ajustar la ventana
    ui->graphicsView->centerOn(personaje->obtenerArea().center().x(), 300);
    ui->graphicsView->setAlignment(Qt::AlignHCenter | Qt::AlignBottom); //el escenario se alinee con la parte inferior de la ventana

    juegoIniciado = true;
    izquierda = false;
    derecha = false;

    reloj.start();
    temporizador->start(16);
    setFocus();
}

void MainWindow::mostrarDerrota(const QString &texto)
{
    temporizador->stop();
    juegoIniciado = false;

    // Mostrar mensaje en el centro de la pantalla
    QGraphicsTextItem *mensaje = escena->addText(texto, QFont("Arial", 24, QFont::Bold));
    mensaje->setDefaultTextColor(Qt::darkRed);
    mensaje->setZValue(10);

    QPointF centro = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
    mensaje->setPos(centro.x() - mensaje->boundingRect().width() / 2, centro.y() - 60);

    // Mostrar boton debajo del mensaje
    botonReiniciar->move((ui->graphicsView->viewport()->width() - botonReiniciar->width()) / 2, (ui->graphicsView->viewport()->height() - botonReiniciar->height()) / 2 + 65);
    botonReiniciar->show();
    botonReiniciar->raise();
}

//ajustar la ventana
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (!ui->graphicsView->scene()) {
        return;
    }

    ui->graphicsView->fitInView(QRectF(0, 0, 900, 500), Qt::KeepAspectRatio);

    if (ui->graphicsView->scene() == escena) {
        ui->graphicsView->centerOn(personaje->obtenerArea().center().x(), 250);
    }
}