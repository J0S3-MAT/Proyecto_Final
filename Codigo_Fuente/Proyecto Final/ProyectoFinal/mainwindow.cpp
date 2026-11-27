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
#include <QPen>
#include <QTextDocument>
#include <QTextOption>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Kung Fu Paisa");

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


    // Estilo de los botones
    const QString estiloBotones =
        "QPushButton { background-color: #222522; color: #FFF3D6; border: 2px solid #C9A563; font-size: 22px; font-weight: bold; }"
        "QPushButton:hover { background-color: #353B32; border-color: #FFF3D6; }"
        "QPushButton:pressed { background-color: #C9A563; color: #222522; border-color: #C9A563; }";

    // Panel de resultados
    panelResultado = new QWidget(ui->graphicsView->viewport());
    panelResultado->setObjectName("panelResultado");
    panelResultado->setAttribute(Qt::WA_StyledBackground);
    panelResultado->setStyleSheet("QWidget#panelResultado { background-color: rgba(34, 37, 34, 245); border: 3px solid #C9A563; }");
    QVBoxLayout *contenidoResultado = new QVBoxLayout(panelResultado);
    contenidoResultado->setContentsMargins(24, 20, 24, 24);
    contenidoResultado->setSpacing(12);

    QFont fuenteResultado = font();
    fuenteResultado.setBold(true);
    fuenteResultado.setPixelSize(44);
    tituloResultado = new QLabel(panelResultado);
    tituloResultado->setFont(fuenteResultado);
    tituloResultado->setAlignment(Qt::AlignCenter);
    contenidoResultado->addWidget(tituloResultado);
    fuenteResultado.setBold(false);
    fuenteResultado.setPixelSize(26);
    mensajeResultado = new QLabel(panelResultado);
    mensajeResultado->setFont(fuenteResultado);
    mensajeResultado->setTextFormat(Qt::PlainText);
    mensajeResultado->setAlignment(Qt::AlignCenter);
    mensajeResultado->setWordWrap(true);
    mensajeResultado->setStyleSheet("color: #FFF3D6;");
    contenidoResultado->addWidget(mensajeResultado);

    botonesResultado = new QBoxLayout(QBoxLayout::LeftToRight);
    botonesResultado->setAlignment(Qt::AlignCenter);
    botonesResultado->setSpacing(12);
    contenidoResultado->addLayout(botonesResultado);

    // Boton para reiniciar el nivel
    botonReiniciar = new QPushButton("REINTENTAR", panelResultado);

    // Boton para volver al menu
    botonVolverMenu = new QPushButton("MENÚ PRINCIPAL", panelResultado);

    // Boton para continuar al segundo nivel
    botonContinuarNivel2 = new QPushButton("CONTINUAR", panelResultado);

    for (QPushButton *boton : {botonReiniciar, botonContinuarNivel2, botonVolverMenu}) {
        boton->setFixedSize(190, 48);
        boton->setFocusPolicy(Qt::NoFocus);
        boton->setStyleSheet(estiloBotones);
        botonesResultado->addWidget(boton);
    }
    panelResultado->hide();

    // Continuar al segundo nivel
    connect(botonContinuarNivel2, &QPushButton::clicked, this, [this]() {
        iniciarNivel2();
    });

    // Reiniciar conservando el nivel y la dificultad
    connect(botonReiniciar, &QPushButton::clicked, this, [this]() {

        if (nivel2 != nullptr) {
            iniciarNivel2();
            return;
        }

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

    QGraphicsPixmapItem *fondoPortada = escenaMenu->addPixmap(portada.scaled(900, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    fondoPortada->setPos(0, 0);
    fondoPortada->setZValue(-1);

    // Panel del menu
    escenaMenu->addRect(128, 72, 644, 350, QPen(QColor("#C9A563"), 3), QColor(34, 37, 34, 232));

    // Titulo del videojuego
    QFont fuenteMenu = font();
    fuenteMenu.setBold(true);
    fuenteMenu.setPixelSize(58);
    QGraphicsTextItem *titulo = escenaMenu->addText("KUNG FU PAISA", fuenteMenu);
    titulo->setDefaultTextColor(QColor("#FFF3D6"));
    titulo->setPos((900 - titulo->boundingRect().width()) / 2, 92);

    // Historia del juego
    fuenteMenu.setBold(false);
    fuenteMenu.setPixelSize(23);
    QString historia = "Ayuda a Po a recolectar las flores para su silleta\ny llevarlas a la ciudad de Medellín.\nPara lograrlo, deberá superar diferentes desafíos.\n¡Elige la dificultad de tu aventura!";
    QGraphicsTextItem *descripcion = escenaMenu->addText(historia, fuenteMenu);
    QTextOption alineacion = descripcion->document()->defaultTextOption();
    alineacion.setAlignment(Qt::AlignHCenter);
    descripcion->document()->setDefaultTextOption(alineacion);
    descripcion->setTextWidth(596);
    descripcion->setDefaultTextColor(QColor("#D1B88D"));
    descripcion->setPos(152, 183);

    // Botones de dificultad
    QPushButton *botonFacil = new QPushButton("FÁCIL");
    QPushButton *botonDificil = new QPushButton("DIFÍCIL");
    botonFacil->setFixedSize(180, 54);
    botonDificil->setFixedSize(180, 54);
    botonFacil->setStyleSheet(estiloBotones);
    botonDificil->setStyleSheet(estiloBotones);

    // Agregar los botones al menu
    QGraphicsProxyWidget *botonEscenaFacil = escenaMenu->addWidget(botonFacil);
    QGraphicsProxyWidget *botonEscenaDificil = escenaMenu->addWidget(botonDificil);

    botonEscenaFacil->setPos(258, 333);
    botonEscenaDificil->setPos(462, 333);

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
        mostrarResultado(texto);
    });

    connect(nivel1, &Nivel1::nivelCompletado, this, [this]() {
        musicaNivel1->stop();
        mostrarVictoria();
    });

    nivel1->iniciar();
    musicaNivel1->play();
    setFocus();
}

void MainWindow::mostrarResultado(const QString &texto, bool victoria)
{
    // Mostrar el mensaje del nivel
    tituloResultado->setText(victoria ? "VICTORIA" : "DERROTA");
    tituloResultado->setStyleSheet(victoria ? "color: #C9A563;" : "color: #EF6666;");
    mensajeResultado->setText(texto);

    // Mostrar las opciones del resultado
    bool continuar = victoria && nivel2 == nullptr;
    botonContinuarNivel2->setVisible(continuar);
    botonReiniciar->setVisible(!victoria);
    botonVolverMenu->setVisible(!continuar);
    panelResultado->show();
    posicionarResultado();
    panelResultado->raise();
}

void MainWindow::mostrarVictoria()
{
    mostrarResultado("¡NIVEL 1 COMPLETADO!", true);
}

void MainWindow::posicionarResultado()
{
    if (!panelResultado || panelResultado->isHidden()) {
        return;
    }

    // Posicionar el panel y los botones
    QWidget *viewport = ui->graphicsView->viewport();
    int ancho = qBound(1, viewport->width() - 24, 520);
    botonesResultado->setDirection(ancho >= 440 ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    panelResultado->setFixedWidth(ancho);
    panelResultado->adjustSize();
    panelResultado->move((viewport->width() - panelResultado->width()) / 2, qMax(0, (viewport->height() - panelResultado->height()) / 2));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    posicionarResultado();

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
        nivel1->posicionarInterfaz();
    }
}

void MainWindow::iniciarNivel2()
{
    // Crear o reiniciar el segundo nivel
    delete nivel2;
    nivel2 = new Nivel2(modoDificil);

    // Conectar las senales del segundo nivel
    connect(nivel2, &Nivel2::nivelPerdido, this, [this](const QString &texto) {
        mostrarResultado(texto);
    });

    connect(nivel2, &Nivel2::nivelCompletado, this, [this]() {
        mostrarResultado("¡PO COMPLETÓ SU SILLETA!", true);
    });

    // Cambiar de escenario
    ui->graphicsView->setScene(nivel2->obtenerEscena());
    ui->graphicsView->fitInView(nivel2->obtenerEscena()->sceneRect(), Qt::KeepAspectRatio);
    ui->graphicsView->setAlignment(Qt::AlignCenter);

    // Ocultar los botones
    panelResultado->hide();

    // Liberar el primer nivel
    delete nivel1;
    nivel1 = nullptr;

    setFocus();
}