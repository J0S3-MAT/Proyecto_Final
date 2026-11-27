#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Cargar la fuente del juego
    int idFuente = QFontDatabase::addApplicationFont(":/fuentes/PixelifySans.ttf");
    QStringList familias = QFontDatabase::applicationFontFamilies(idFuente);
    if (!familias.isEmpty()) {
        QFont fuente(familias.first());
        fuente.setPixelSize(20);
        a.setFont(fuente);
    }
    else {
        qWarning() << "No se pudo cargar Pixelify Sans desde los recursos";
    }

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ProyectoFinal_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return QApplication::exec();
}
