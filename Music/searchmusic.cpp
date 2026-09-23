#include "searchmusic.h"
#include "ui_searchmusic.h"

SearchMusic::SearchMusic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SearchMusic)
{
    ui->setupUi(this);
}

SearchMusic::~SearchMusic()
{
    delete ui;
}
