#include "MainWindow.h"

MainWindow::MainWindow(QMainWindow *parent) : QMainWindow(parent){

  mainWidget = new QWidget(this);
  mainLayout = new QGridLayout(mainWidget);
  setCentralWidget(mainWidget);

  //{ HiraKataTable
  tableWidget = new QWidget(mainWidget);
  tableLayout = new QGridLayout(tableWidget);

  HiraKataSelect = new QComboBox(tableWidget);
  HiraKataSelect->addItems(QStringList({"Hiragana","Katakana"}));
  connect(HiraKataSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [=]( int i ) { emit( FillTable(i, 0) ); });
  tableLayout->addWidget(HiraKataSelect, 0, 0);

  KanaYouoSelect = new QComboBox(tableWidget);
  KanaYouoSelect->addItems(QStringList({"Kana","Youo"}));
  connect(KanaYouoSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          [=]( int i ) { emit( FillTable(HiraKataSelect->currentIndex(), i) ); });
  tableLayout->addWidget(KanaYouoSelect, 0, 1);

  HiraTable = new QTableWidget(17, 6, tableWidget);

  HiraTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  HiraTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
  HiraTable->horizontalHeader()->setMinimumSectionSize(28);
  HiraTable->horizontalHeader()->setDefaultSectionSize(28);
  HiraTable->horizontalHeader()->setMaximumSectionSize(50);
  HiraTable->horizontalHeader()->setVisible(false);

  HiraTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  HiraTable->verticalHeader()->setDefaultSectionSize(26);
  HiraTable->verticalHeader()->setVisible(false);

  HiraTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  HiraTable->setSelectionMode(QTableWidget::NoSelection);
  HiraTable->setShowGrid(false);

  jFont = QFont("IPAexGothic", 16);
  jFont.setStyleStrategy(QFont::PreferAntialias);
  nFont = QFont("Myriad Pro", 12);
  nFont.setStyleStrategy(QFont::PreferAntialias);

  JcellBase = new QTableWidgetItem();
  JcellBase->setTextAlignment(Qt::AlignCenter);
  JcellBase->setFont(jFont);
  NcellBase = new QTableWidgetItem();
  NcellBase->setTextAlignment(Qt::AlignCenter);
  NcellBase->setFont(nFont);
  FillTable(0, 0);

  tableLayout->addWidget(HiraTable, 1, 0, 1, 2);
  //}
  mainLayout->addWidget(tableWidget, 0, 0);
}

void MainWindow::FillTable(bool HiraKata, bool KanaYouo){
  if(!HiraKata && !KanaYouo) TableState = 0;
  if(HiraKata && !KanaYouo) TableState = 1;
  if(!HiraKata && KanaYouo) TableState = 2;
  if(HiraKata && KanaYouo) TableState = 3;

  pugi::xml_document Hira;
  if(!HiraKata){
    if(!Hira.load_file("xml_files/Hiragana.xml")) return;
  }else{
    if(!Hira.load_file("xml_files/Katakana.xml")) return;
  }

  if(!KanaYouo){
    int rowMax = 0;
    int colMax = 0;
    pugi::xpath_node_set xNode = Hira.select_nodes("Kana");
    for(pugi::xpath_node_set::const_iterator it = xNode.begin(); it != xNode.end(); ++it){
      pugi::xml_text rowStr = it->node().first_child().first_child().text();
      int row = rowStr.as_int();
      pugi::xml_text colStr = it->node().first_child().next_sibling().first_child().text();
      int col = colStr.as_int();
      QString kanaStr = it->node().first_child().next_sibling().next_sibling().next_sibling().child_value();

      if(row > rowMax) rowMax = row;
      if(col > colMax) colMax = col;

      QTableWidgetItem *tableCell = JcellBase->clone();
      tableCell->setText(kanaStr);
      HiraTable->setItem(row, col, tableCell);
    }
    activeKana.clear();
    activeKana.resize(rowMax);
    for(int row=0; row<activeKana.size(); row++){
      activeKana[row].resize(colMax);
      for(int col=0; col<activeKana[row].size(); col++){
        if(!HiraTable->item(row+1, col+1)){
          activeKana[row][col] = -1; // Empty cell
        }else{
          activeKana[row][col] = 0; // Initializing a non-empty cell to false
        }
      }
    }

    setButton("( )", 0, 0);

    int i = 1;
    xNode = Hira.select_nodes("Kana[row=1]/Sound");
    for(pugi::xpath_node_set::const_iterator it = xNode.begin(); it != xNode.end(); ++it,i++){
      QString rowStr = it->node().child_value();
      rowStr = rowStr.toUpper();

      setButton(rowStr, 0, i);
    }

    i = 2;
    setButton("-", 1, 0);
    xNode = Hira.select_nodes("Kana[col=1 and not(row=1)]/Sound");
    for(pugi::xpath_node_set::const_iterator it = xNode.begin(); it != xNode.end(); ++it,i++){
      QString rowStr = it->node().child_value();
      rowStr = rowStr[0].toUpper();

      setButton(rowStr, i, 0);
    }
  }
}

void MainWindow::setButton(QString name, int row, int col){
  QWidget* ButtonWidget = new QWidget(tableWidget);
  QPushButton* Button = new QPushButton(tableWidget);
  Button->setFont(nFont);
  Button->setText(name);
  Button->setFlat(true);
  QHBoxLayout* ButtonLayout = new QHBoxLayout(ButtonWidget);
  ButtonLayout->addWidget(Button);
  ButtonLayout->setAlignment(Qt::AlignCenter);
  ButtonLayout->setContentsMargins(0, 0, 0, 0);
  ButtonWidget->setLayout(ButtonLayout);
  HiraTable->setCellWidget(row, col, ButtonWidget);

  if(row+col == 0) connect(Button, &QPushButton::clicked, [=]{ MainWindow::AllBClick(); });
  if(row > 0 && col == 0) connect(Button, &QPushButton::clicked, [=]{ MainWindow::RowBClick(row); });
  if(col > 0 && row == 0) connect(Button, &QPushButton::clicked, [=]{ MainWindow::ColBClick(col); });
}
void MainWindow::AllBClick(){
  int Count = 0;
  int MaxCount = 0;
  for(int row = 0; row < activeKana.size(); row++){
    for(int col = 0; col < activeKana[row].size(); col++){
      if(activeKana[row][col] != -1){
        MaxCount++;
        if(activeKana[row][col] == 1){
          Count++;
        }
      }
    }
  }
  bool Activate = false;
  if(2*Count <= MaxCount){
    Activate = true;
  }
  for(int row = 0; row < activeKana.size(); row++){
    for(int col = 0; col < activeKana[row].size(); col++){
      if(activeKana[row][col] != -1){
        activeKana[row][col] = Activate;
      }
    }
  }

  UpdateKana();
}
void MainWindow::RowBClick(int Button){
  int Count = 0;
  int MaxCount = 0;
  for(int col = 0; col < activeKana[Button-1].size(); col++){
    if(activeKana[Button-1][col] != -1){
      MaxCount++;
      if(activeKana[Button-1][col] == 1){
        Count++;
      }
    }
  }
  bool Activate = false;
  if(2*Count <= MaxCount){
    Activate = true;
  }
  for(int col=0; col<activeKana[Button-1].size(); col++){
    if(activeKana[Button-1][col] != -1){
      activeKana[Button-1][col] = Activate;
    }
  }

  UpdateKana();
}
void MainWindow::ColBClick(int Button){
  int Count = 0;
  int MaxCount = 0;
  for(int row = 0; row < activeKana.size(); row++){
    if(activeKana[row][Button-1] != -1){
      MaxCount++;
      if(activeKana[row][Button-1] == 1){
        Count++;
      }
    }
  }
  bool Activate = false;
  if(2*Count <= MaxCount){
    Activate = true;
  }
  for(int row=0; row<activeKana.size(); row++){
    if(activeKana[row][Button-1] != -1){
      activeKana[row][Button-1] = Activate;
    }
  }

  UpdateKana();
}
void MainWindow::UpdateKana(){
  QBrush green = QBrush(QColor(179, 255, 166));
  QBrush white = QBrush(QColor(255, 255, 255));
  for(int row=0; row<activeKana.size(); row++){
    for(int col=0; col<activeKana[row].size(); col++){
      if(activeKana[row][col] == -1){
      }else if(activeKana[row][col]){
        HiraTable->item(row+1, col+1)->setBackground(green);
      }else{
        HiraTable->item(row+1, col+1)->setBackground(white);
      }
    }
  }
}
