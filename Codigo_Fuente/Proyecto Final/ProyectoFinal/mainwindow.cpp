#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QDebug>
#include <QTimer>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QFont>
#include <QResizeEvent>
#include <QUrl>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Preparar la musica del primer nivel
    musicaNivel1 = new QMediaPlayer(this);
    salidaAudio = new QAudioOutput(this);

    musicaNivel1->setAudioOutput(salidaAudio);
    salidaAudio->setVolume(0.25);

    // Comprobar si Qt encuentra el archivo
    qDebug() << "Existe la musica:"
             << QFile::exists(":/audios/musica_nivel1.mp3");

    // Mostrar errores de reproduccion
    connect(musicaNivel1, &QMediaPlayer::errorOccurred, this,
            [](QMediaPlayer::Error, const QString &mensaje) {
                qDebug() << "Error de musica:" << mensaje;
            });

    musicaNivel1->setSource(QUrl("qrc:/audios/musica_nivel1.mp3"));
    musicaNivel1->setLoops(QMediaPlayer::Infinite);


    // Boton para reiniciar el nivel
    botonReiniciar = new QPushButton("REINICIAR", ui->graphicsView->viewport());
    botonReiniciar->setFixedSize(150, 40);
    botonReiniciar->setFocusPolicy(Qt::NoFocus);
    botonReiniciar->hide();

    // Boton para volver al menu
    botonVolverMenu = new QPushButton("VOLVER AL MENU", ui->graphicsView->viewport());
    botonVolverMenu->setFixedSize(150, 40);
    botonVolverMenu->setFocusPolicy(Qt::NoFocus);
    botonVolverMenu->hide();

    // Boton para continuar al segundo nivel
    botonContinuarNivel2 = new QPushButton("CONTINUAR AL NIVEL 2", ui->graphicsView->viewport());
    botonContinuarNivel2->setFixedSize(240, 45);
    botonContinuarNivel2->setFocusPolicy(Qt::NoFocus);
    botonContinuarNivel2->hide();

    connect(botonContinuarNivel2, &QPushButton::clicked, this, [this]() {
        // Crear el segundo nivel
        nivel2 = new Nivel2();

        // Cambiar de escenario
        ui->graphicsView->setScene(nivel2->obtenerEscena());
        ui->graphicsView->fitInView(nivel2->obtenerEscena()->sceneRect(), Qt::KeepAspectRatio);
        ui->graphicsView->setAlignment(Qt::AlignCenter);

        botonContinuarNivel2->hide();

        // Liberar el primer nivel, que ya terminamos
        delete nivel1;
        nivel1 = nullptr;

        setFocus();
    });

    // Reiniciar conservando la dificultad
    connect(botonReiniciar, &QPushButton::clicked, this, [this]() {
        MainWindow *nueva = new MainWindow();
        nueva->setAttribute(Qt::WA_DeleteOnClose);
        nueva->setGeometry(geometry());
        nueva->modoDificil = modoDificil;
        nueva->show();
        nueva->iniciarNivel();
        close();
    });

    // Volver al menu principal
    connect(botonVolverMenu, &QPushButton::clicked, this, [this]() {
        MainWindow *nueva = new MainWindow();
        nueva->setAttribute(Qt::WA_DeleteOnClose);
        nueva->setGeometry(geometry());
        nueva->show();
        close();
    });


    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);

    escenaMenu = new QGraphicsScene(this);
    escenaMenu->setSceneRect(0, 0, 900, 500);

    // Colocar la portada como fondo del menu
    QPixmap portada(":/imagenes/imagenes/portada.png");

    QGraphicsPixmapItem *fondoPortada = escenaMenu->addPixmap(
        portada.scaled(900, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
        );

    fondoPortada->setPos(0, 0);
    fondoPortada->setZValue(-1);

    // Titulo del videojuego
    QGraphicsTextItem *titulo = escenaMenu->addText("KUNG FU PAISA", QFont("Arial", 36, QFont::Bold));
    titulo->setDefaultTextColor(QColor(30, 80, 45));
    titulo->setPos((900 - titulo->boundingRect().width()) / 2, 75);

    // Historia del juego
    QGraphicsTextItem *descripcion = escenaMenu->addText(
        "Ayuda a Po a recolectar las flores para su silleta\n"
        "y llevarlas a la ciudad de Medellin.\n"
        "Para lograrlo, debera superar diferentes desafios.\n"
        "¡Elige la dificultad de tu aventura!",
        QFont("Arial", 15));

    descripcion->setDefaultTextColor(QColor(40, 60, 50));
    descripcion->setPos(340, 175);

    // Botones de dificultad
    QPushButton *botonFacil = new QPushButton("FACIL");
    QPushButton *botonDificil = new QPushButton("DIFICIL");

    botonFacil->setFixedSize(180, 55);
    botonDificil->setFixedSize(180, 55);
    QString estiloBotones =
        "QPushButton {"
        "background-color: #2E703C;"
        "color: white;"
        "border: 2px solid #D5B66C;"
        "border-radius: 12px;"
        "font-size: 16px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background-color: #439653;"
        "}";

    botonFacil->setStyleSheet(estiloBotones);
    botonDificil->setStyleSheet(estiloBotones);

    // Agregar los botones al menu
    QGraphicsProxyWidget *botonEscenaFacil = escenaMenu->addWidget(botonFacil);
    QGraphicsProxyWidget *botonEscenaDificil = escenaMenu->addWidget(botonDificil);

    botonEscenaFacil->setPos(250, 350);
    botonEscenaDificil->setPos(470, 350);

    // Iniciar el modo facil
    connect(botonFacil, &QPushButton::clicked, this, [this]() {
        modoDificil = false;
        iniciarNivel();
    });

    // Iniciar el modo dificil
    connect(botonDificil, &QPushButton::clicked, this, [this]() {
        modoDificil = true;
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
    if (nivel2 != nullptr && ui->graphicsView->scene() == nivel2->obtenerEscena()) {
        nivel2->presionarTecla(event->key(), event->isAutoRepeat());
    }
    else if (nivel1 != nullptr) {
        nivel1->presionarTecla(event->key(), event->isAutoRepeat());
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (nivel2 != nullptr && ui->graphicsView->scene() == nivel2->obtenerEscena()) {
        nivel2->soltarTecla(event->key(), event->isAutoRepeat());
    }
    else if (nivel1 != nullptr) {
        nivel1->soltarTecla(event->key(), event->isAutoRepeat());
    }
}


MainWindow::~MainWindow()
{
    delete nivel1;
    delete nivel2;
    delete ui;
}

void MainWindow::iniciarNivel()
{
    if (nivel1 != nullptr) {
        return;
    }

    nivel1 = new Nivel1(ui->graphicsView, modoDificil, this);

    connect(nivel1, &Nivel1::nivelPerdido, this, [this](const QString &texto) {
        musicaNivel1->stop();
        mostrarDerrota(texto);
    });

    connect(nivel1, &Nivel1::nivelCompletado, this, [this]() {
        musicaNivel1->stop();
        mostrarVictoria();
    });

    nivel1->iniciar();
    musicaNivel1->play();
    setFocus();
}

void MainWindow::mostrarDerrota(const QString &texto)
{
    // El temporizador del nivel ya fue detenido por Nivel1.
    QGraphicsScene *escena = nivel1->obtenerEscena();

    QGraphicsTextItem *mensaje = escena->addText(texto, QFont("Arial", 24, QFont::Bold));
    mensaje->setDefaultTextColor(Qt::darkRed);
    mensaje->setZValue(10);

    QPointF centro = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
    mensaje->setPos(centro.x() - mensaje->boundingRect().width() / 2, centro.y() - 60);

    int centroX = ui->graphicsView->viewport()->width() / 2;
    int posicionY = ui->graphicsView->viewport()->height() / 2 + 65;

    botonReiniciar->move(centroX - 160, posicionY);
    botonVolverMenu->move(centroX + 10, posicionY);
    botonReiniciar->show();
    botonVolverMenu->show();
    botonReiniciar->raise();
    botonVolverMenu->raise();
}

void MainWindow::mostrarVictoria()
{
    QGraphicsScene *escena = nivel1->obtenerEscena();
    QGraphicsTextItem *mensaje = escena->addText("¡NIVEL 1 COMPLETADO!", QFont("Arial", 24, QFont::Bold));
    mensaje->setDefaultTextColor(Qt::darkGreen);
    mensaje->setZValue(10);

    QPointF centro = ui->graphicsView->mapToScene(ui->graphicsView->viewport()->rect().center());
    mensaje->setPos(centro.x() - mensaje->boundingRect().width() / 2,
                    centro.y() - mensaje->boundingRect().height() / 2);

    // Mostrar boton para continuar
    int centroX = ui->graphicsView->viewport()->width() / 2;
    int centroY = ui->graphicsView->viewport()->height() / 2;

    botonContinuarNivel2->move(centroX - botonContinuarNivel2->width() / 2, centroY + 65);
    botonContinuarNivel2->show();
    botonContinuarNivel2->raise();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (!ui->graphicsView->scene()) {
        return;
    }

    if (nivel2 != nullptr && ui->graphicsView->scene() == nivel2->obtenerEscena()) {
        ui->graphicsView->fitInView(nivel2->obtenerEscena()->sceneRect(), Qt::KeepAspectRatio);
        return;
    }

    ui->graphicsView->fitInView(QRectF(0, 0, 900, 500), Qt::KeepAspectRatio);

    if (nivel1 != nullptr && ui->graphicsView->scene() == nivel1->obtenerEscena()) {
        ui->graphicsView->centerOn(nivel1->obtenerXPo(), 250);
    }
}
