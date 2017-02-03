#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>
#include <QtCore/QTextStream>
#include <QtGui/QColor>
#include <QtCore/QVector>

#include "pugixml/pugixml.hpp"
#include "HiraKataTable.h"

class MainWindow : public QMainWindow {

  public:
    MainWindow(QMainWindow *parent = 0);
  private:
    QWidget *mainWidget;
    QGridLayout *mainLayout;

    //{ HiraKataTable
    QWidget *tableWidget;
    QGridLayout *tableLayout;

    QComboBox *HiraKataSelect;
    QComboBox *KanaYouoSelect;

    int TableState; // 0: Hiragana, 1: Katakana, 2: Hira Youo, 3: Kata Youo
    QTableWidget *HiraTable;

    QFont jFont; // Japanese font
    QFont nFont; // Normal font
    QTableWidgetItem *JcellBase;
    QTableWidgetItem *NcellBase;

    QVector< QVector<int> > activeKana;
    void FillTable(bool HiraKata, bool KanaYouo);

    void setButton(QString name, int row, int col);
    void AllBClick();
    void RowBClick(int Button);
    void ColBClick(int Button);
    void UpdateKana();
    //}
};

#endif // MAINWINDOW_H
