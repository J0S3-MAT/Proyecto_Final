#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QElapsedTimer>
#include <QGraphicsRectItem>

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
    QGraphicsPixmapItem *po = nullptr;
    bool izquierda = false;
    bool derecha = false;
    QElapsedTimer reloj;
    bool saltando = false;
    double tiempoSalto = 0;
    double yInicioSalto = 0;
    QGraphicsRectItem *plataforma;
    double tiempoJuego = 0;

    void actualizarJuego();
};
#endif // MAINWINDOW_H
