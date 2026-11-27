#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QLabel>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QPushButton>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "nivel1.h"
#include "nivel2.h"

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
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escenaMenu = nullptr;
    Nivel1 *nivel1 = nullptr;
    Nivel2 *nivel2 = nullptr;
    bool modoDificil = false;
    QWidget *panelResultado = nullptr;
    QLabel *tituloResultado = nullptr;
    QLabel *mensajeResultado = nullptr;
    QBoxLayout *botonesResultado = nullptr;
    QPushButton *botonReiniciar = nullptr;
    QPushButton *botonVolverMenu = nullptr;
    QPushButton *botonContinuarNivel2 = nullptr;
    QMediaPlayer *musicaNivel1 = nullptr;
    QAudioOutput *salidaAudio = nullptr;

    void iniciarNivel();
    void iniciarNivel2();
    void mostrarResultado(const QString &texto, bool victoria = false);
    void mostrarVictoria();
    void posicionarResultado();
};

#endif // MAINWINDOW_H
