#include "nivel1.h"
#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QFont>
#include <QRandomGenerator>
#include <QKeyEvent>
#include <cmath>

Nivel1::Nivel1(QGraphicsView *vista, bool dificil, QObject *parent)
    : QObject(parent), vista(vista), modoDificil(dificil)
{
    escena = new QGraphicsScene(this);
    escena->setSceneRect(0, 70, 35000, 600);

    // Mostrar el escenario en graphicsView
    vista->setScene(escena);


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


    // Franja de la interfaz
    fondoInterfaz = escena->addRect(0, 0, 900, 82, QPen(Qt::NoPen), QColor(34, 37, 34, 225));
    fondoInterfaz->setZValue(20);
    bordeInterfaz = new QGraphicsRectItem(fondoInterfaz);
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

    // Contador de flores
    QPixmap florIcono = QPixmap(":/imagenes/imagenes/flor.png").scaledToHeight(30, Qt::FastTransformation);
    QGraphicsPixmapItem *iconoFlor = new QGraphicsPixmapItem(florIcono, fondoInterfaz);
    iconoFlor->setPos(22, 25);
    textoFlores = agregarTexto("00/10", 30, QColor("#FFF3D6"));
    textoFlores->setPos(33 + florIcono.width(), 22);

    // Contador de tiempo
    etiquetaTiempo = agregarTexto("TIEMPO", 18, QColor("#D1B88D"));
    textoTiempo = agregarTexto("15 s", 36, QColor("#FFF3D6"));

    // Nivel y dificultad
    textoNivel = agregarTexto("NIVEL 1", 20, QColor("#FFF3D6"));
    textoDificultad = agregarTexto(modoDificil ? "DIFÍCIL" : "FÁCIL", 19, QColor(modoDificil ? "#EF6666" : "#A8D57C"));

    // Temporizador del juego
    temporizador = new QTimer(this);
    connect(temporizador, &QTimer::timeout, this, &Nivel1::actualizarJuego);

}

Nivel1::~Nivel1()
{
    // Primero eliminar las clases que utilizan elementos del escenario.
    delete personaje;
    for (Flor *flor : flores) {
        delete flor;
    }
    for (Dumpling *dumpling : dumplings) {
        delete dumpling;
    }
    for (PlataformaOscilante *plataforma : plataformas) {
        delete plataforma;
    }
    delete cabra;
    // QGraphicsScene y QTimer se eliminan automaticamente al ser hijos de Nivel1.
}

void Nivel1::iniciar()
{
    if (activo) {
        return;
    }
    // Crear plataformas aleatorias
    for (int x = 350; x < 35000;) {

        int inicioTrampa = x;

        // Crear lava y escaleras solamente en modo dificil
        if (modoDificil && QRandomGenerator::global()->bounded(100) < 25) {

            for (int i = 0; i < 3; i++) {
                int yEscalon = 400 - i * 70;

                PlataformaOscilante *escalon = new PlataformaOscilante(escena, x, yEscalon, 0, 3.0, 1);
                plataformas.push_back(escalon);

                x += escalon->obtenerRectangulo()->rect().width() + 130;
            }

            int anchoTrampa = x - inicioTrampa - 40;

            QGraphicsRectItem *trampa = escena->addRect(0, 0, anchoTrampa, 60, QPen(Qt::NoPen), QBrush(imagenLava[0]));
            trampa->setPos(inicioTrampa, 520);
            trampas.push_back(trampa);

            x += QRandomGenerator::global()->bounded(250, 401);
            continue;
        }

        // Plataformas normales para ambas dificultades
        int tipo = QRandomGenerator::global()->bounded(1, 4);
        int y = QRandomGenerator::global()->bounded(425, 451);
        int amplitud = QRandomGenerator::global()->bounded(15, 26);
        double periodo = 2.0 + QRandomGenerator::global()->bounded(0, 16) / 10.0;

        PlataformaOscilante *plataforma = new PlataformaOscilante(escena, x, y, amplitud, periodo, tipo);
        plataformas.push_back(plataforma);

        // Algunas plataformas tienen una flor
        if (QRandomGenerator::global()->bounded(100) < 60) {
            double ancho = plataforma->obtenerRectangulo()->rect().width();
            flores.push_back(new Flor(escena, (ancho - 30) / 2, -30, plataforma->obtenerRectangulo()));
        }

        int anchoPlataforma = plataforma->obtenerRectangulo()->rect().width();
        x += anchoPlataforma + QRandomGenerator::global()->bounded(250, 400);
    }

    // Crear flores aleatorias segun la dificultad
    int distanciaMinima = modoDificil ? 190 : 220;
    int distanciaMaxima = modoDificil ? 271 : 301;

    for (int x = 240; x < 34500; x += QRandomGenerator::global()->bounded(distanciaMinima, distanciaMaxima)) {
        // Verificar si hay lava en la posicion de la flor
        bool sobreLava = false;

        for (QGraphicsRectItem *trampa : trampas) {
            QRectF areaLava = trampa->sceneBoundingRect();

            // Evitar flores sobre la lava y muy cerca de sus bordes
            if (x + 40 > areaLava.left() &&
                x < areaLava.right() + 10) {
                sobreLava = true;
                break;
            }
        }

        // Crear la flor solamente si no hay lava
        if (!sobreLava) {
            flores.push_back(new Flor(escena, x, 490));
        }
    }


    // Crear dumplings solamente en modo dificil
    if (modoDificil) {

        for (int x = 800; x < 34000; x += QRandomGenerator::global()->bounded(650, 951)) {

            // Evitar dumplings encima de la lava
            bool sobreLava = false;

            for (QGraphicsRectItem *trampa : trampas) {
                QRectF areaLava = trampa->sceneBoundingRect();

                if (x + 42 > areaLava.left() && x < areaLava.right()) {
                    sobreLava = true;
                    break;
                }
            }

            if (!sobreLava) {

                // 50% buenos y 50% malos
                bool bueno = QRandomGenerator::global()->bounded(2) == 0;

                dumplings.push_back(new Dumpling(escena, x, 478, bueno));
            }
        }
    }

    // Iniciar el nivel y su camara
    vista->setScene(escena);
    vista->fitInView(QRectF(0, 0, 900, 500), Qt::KeepAspectRatio);
    vista->centerOn(personaje->obtenerArea().center().x(), 300);
    vista->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    posicionarInterfaz();

    activo = true;
    cabra->configurarDificultad(modoDificil);

    reloj.start();
    temporizador->start(16);
}

void Nivel1::posicionarInterfaz()
{
    // Mantener los contadores visibles
    QRectF areaVisible = vista->mapToScene(vista->viewport()->rect()).boundingRect();
    double ancho = areaVisible.width();

    fondoInterfaz->setPos(areaVisible.topLeft());
    fondoInterfaz->setRect(0, 0, ancho, 82);
    bordeInterfaz->setRect(0, 80, ancho, 2);

    etiquetaTiempo->setPos((ancho - etiquetaTiempo->boundingRect().width()) / 2, 6);
    textoTiempo->setPos((ancho - textoTiempo->boundingRect().width()) / 2, 30);
    textoNivel->setPos(ancho - textoNivel->boundingRect().width() - 22, 11);
    textoDificultad->setPos(ancho - textoDificultad->boundingRect().width() - 22, 39);
}

void Nivel1::presionarTecla(int tecla, bool repetida)
{
    if (activo) {
        personaje->presionarTecla(tecla, repetida);
    }
}

void Nivel1::soltarTecla(int tecla, bool repetida)
{
    if (activo) {
        personaje->soltarTecla(tecla, repetida);
    }
}

QGraphicsScene *Nivel1::obtenerEscena() const
{
    return escena;
}

bool Nivel1::estaActivo() const
{
    return activo;
}

double Nivel1::obtenerXPo() const
{
    return personaje->obtenerArea().center().x();
}

void Nivel1::terminarConDerrota(const QString &texto)
{
    temporizador->stop();
    activo = false;
    posicionarInterfaz();
    emit nivelPerdido(texto);
}

void Nivel1::actualizarJuego()
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
    textoTiempo->setText(QString("%1 s").arg(static_cast<int>(std::ceil(tiempoRestante))));
    textoTiempo->setBrush(QColor(tiempoRestante <= 5 ? "#EF6666" : "#FFF3D6"));

    // Verificar si se termino el tiempo
    if (tiempoRestante <= 0) {
        terminarConDerrota("TIEMPO AGOTADO\nNo reuniste las 10 flores");
        return;
    }

    // Movimiento armonico simple de la plataforma
    for (PlataformaOscilante *plataforma : plataformas) {
        plataforma->actualizar(tiempoJuego);
    }

    personaje->mover(dt, plataformas);

    // Colision con la lava
    QRectF areaPo = personaje->obtenerArea();

    for (QGraphicsRectItem *trampa : trampas) {
        QRectF areaTrampa = trampa->sceneBoundingRect();

        if (areaPo.right() > areaTrampa.left() && areaPo.left() < areaTrampa.right() && areaPo.bottom() >= 520) {
            // Mostrar a Po quemado
            personaje->mostrarQuemado();

            // Detener el juego para mantener el sprite
            temporizador->stop();
            activo = false;

            // Mostrar derrota después de 200 milisegundos
            QTimer::singleShot(200, this, [this]() {
                terminarConDerrota("¡CAÍSTE EN LA LAVA!");
            });
            return;
        }
    }

    // Movimiento de la cabra y contacto con la lava
    cabra->actualizar(dt, personaje->obtenerArea().center().x(), trampas);

    // Actualizar salto, caida y contacto con la plataforma
    personaje->actualizarMovimientoVertical(dt, plataformas);

    // Colision elastica entre Po y la cabra
    bool tocandoCabra = personaje->obtenerArea().intersects(cabra->obtenerArea());

    if (tocandoCabra && !contactoCabra) {
        double nuevaVelocidadCabra = personaje->recibirChoque(cabra->obtenerVelocidad());
        cabra->recibirChoque(nuevaVelocidadCabra);
    }

    contactoCabra = tocandoCabra;

    // Recoleccion de flores
    for (Flor *flor : flores) {
        if (flor->recolectarSiToca(personaje->obtenerArea())) {
            personaje->sumarFlor();
        }
    }

    // Recoleccion de dumplings
    for (Dumpling *dumpling : dumplings) {

        if (dumpling->recolectarSiToca(personaje->obtenerArea())) {

            if (dumpling->esBueno()) {
                // Dumpling bueno: dos flores y mas velocidad
                personaje->sumarFlores(2);
                personaje->activarVelocidad();
            }
            else {
                // Dumpling malo: quitar dos flores
                personaje->quitarFlores(2);
            }
        }
    }

    // Actualizar el contador
    textoFlores->setText(QString("%1/10").arg(personaje->obtenerFlores(), 2, 10, QChar('0')));

    // Verificar si Po completo el primer nivel
    if (personaje->obtenerFlores() >= 10) {
        temporizador->stop();
        activo = false;
        posicionarInterfaz();
        emit nivelCompletado();

        return;
    }

    //para que camine
    // Actualizar las animaciones de Po
    personaje->actualizarAnimacion(dt);

    // Camara horizontal
    vista->centerOn(personaje->obtenerArea().center().x(), 300);

    posicionarInterfaz();

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
        terminarConDerrota("¡LA MONTAÑA TE ALCANZÓ!");
        return;
    }
}
