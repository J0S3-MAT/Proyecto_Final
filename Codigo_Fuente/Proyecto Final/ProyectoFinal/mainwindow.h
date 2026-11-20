#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsEllipseItem>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QGraphicsRectItem>
#include <QPixmap> //para que camine
#include "po.h"
#include "plataformaoscilante.h"
#include "flor.h"
#include <QGraphicsTextItem>
#include <vector> //para flores
#include <QTimer> //para el contador
#include "cabra.h"
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "dumpling.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override; //Ajustar la ventana

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escena;
    QGraphicsScene *escenaMenu;
    bool juegoIniciado = false;
    bool modoDificil = false;
    Po *personaje = nullptr;
    bool izquierda = false;
    bool derecha = false;
    QElapsedTimer reloj;
    std::vector<PlataformaOscilante*> plataformas;
    double tiempoJuego = 0;
    double tiempoRestante = 60.0;
    std::vector<Flor*> flores;
    std::vector<Dumpling*> dumplings;
    QGraphicsTextItem *textoFlores = nullptr;
    QGraphicsTextItem *textoTiempo = nullptr;
    QTimer *temporizador = nullptr;
    Cabra *cabra = nullptr;
    bool contactoCabra = false;
    QGraphicsPixmapItem *montana = nullptr;
    QPixmap imagenMontana[4];
    int fotogramaMontana = 0;
    double tiempoAnimacionMontana = 0;
    std::vector<QGraphicsRectItem*> trampas;
    QPixmap imagenLava[5];
    int fotogramaLava = 0;
    double tiempoAnimacionLava = 0;
    QPushButton *botonReiniciar = nullptr;
    QPushButton *botonVolverMenu = nullptr;
    void iniciarNivel();
    void mostrarDerrota(const QString &texto);
    QMediaPlayer *musicaNivel1;
    QAudioOutput *salidaAudio;


    void actualizarJuego();
};
#endif // MAINWINDOW_H
