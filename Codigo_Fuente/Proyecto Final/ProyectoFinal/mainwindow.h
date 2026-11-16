#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QGraphicsRectItem>
#include <QPixmap> //para que camine

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

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escena;
    QGraphicsScene *escenaMenu;
    bool juegoIniciado = false;
    QGraphicsPixmapItem *po = nullptr;
    bool izquierda = false;
    bool derecha = false;
    QElapsedTimer reloj;
    bool saltando = false;
    bool sobrePlataforma = false;
    bool cayendo = false;
    double velocidadCaida = 0;
    double tiempoSalto = 0;
    double yInicioSalto = 0;
    QGraphicsRectItem *plataforma;
    double tiempoJuego = 0;
    //para que camine
    QPixmap imagenReposo;
    QPixmap imagenCaminar[6];
    int fotograma = 0;
    double tiempoAnimacion = 0;
    bool estabaCaminando = false;
    bool mirandoIzquierda = false;
    //para que salte
    QPixmap imagenSalto[3];

    void actualizarJuego();
};
#endif // MAINWINDOW_H
