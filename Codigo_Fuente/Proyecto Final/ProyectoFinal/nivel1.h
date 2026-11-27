#ifndef NIVEL1_H
#define NIVEL1_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QElapsedTimer>
#include <QTimer>
#include <QPixmap>
#include <QString>
#include <array>
#include <vector>
#include "po.h"
#include "plataformaoscilante.h"
#include "flor.h"
#include "cabra.h"
#include "dumpling.h"

class Nivel1 : public QObject
{
    Q_OBJECT

private:
    QGraphicsScene *escena = nullptr;
    QGraphicsView *vista = nullptr;
    bool modoDificil = false;
    bool activo = false;
    Po *personaje = nullptr;
    QElapsedTimer reloj;
    std::vector<PlataformaOscilante*> plataformas;
    double tiempoJuego = 0;
    double tiempoRestante = 15.0;
    std::vector<Flor*> flores;
    std::vector<Dumpling*> dumplings;
    QGraphicsRectItem *fondoInterfaz = nullptr;
    QGraphicsRectItem *bordeInterfaz = nullptr;
    QGraphicsSimpleTextItem *textoFlores = nullptr;
    QGraphicsSimpleTextItem *etiquetaTiempo = nullptr;
    QGraphicsSimpleTextItem *textoTiempo = nullptr;
    QGraphicsSimpleTextItem *textoNivel = nullptr;
    QGraphicsSimpleTextItem *textoDificultad = nullptr;
    QTimer *temporizador = nullptr;
    Cabra *cabra = nullptr;
    bool contactoCabra = false;
    QGraphicsPixmapItem *montana = nullptr;
    std::array<QPixmap, 4> imagenMontana;
    int fotogramaMontana = 0;
    double tiempoAnimacionMontana = 0;
    std::vector<QGraphicsRectItem*> trampas;
    std::array<QPixmap, 5> imagenLava;
    int fotogramaLava = 0;
    double tiempoAnimacionLava = 0;

    void actualizarJuego();
    void terminarConDerrota(const QString &texto);

public:
    Nivel1(QGraphicsView *vista, bool dificil, QObject *parent = nullptr);
    ~Nivel1() override;

    void iniciar();
    void posicionarInterfaz();
    void presionarTecla(int tecla, bool repetida);
    void soltarTecla(int tecla, bool repetida);
    QGraphicsScene *obtenerEscena() const;
    bool estaActivo() const;
    double obtenerXPo() const;

signals:
    void nivelCompletado();
    void nivelPerdido(const QString &mensaje);
};

#endif // NIVEL1_H
