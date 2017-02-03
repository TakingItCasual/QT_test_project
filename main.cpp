#include <QtWidgets/QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  MainWindow QtWindow;

  QtWindow.show();

  return app.exec();
}
