#ifndef SEARCHMUSIC_H
#define SEARCHMUSIC_H

#include <QMainWindow>

namespace Ui {
class SearchMusic;
}

class SearchMusic : public QMainWindow
{
    Q_OBJECT

public:
    explicit SearchMusic(QWidget *parent = 0);
    ~SearchMusic();

private:
    Ui::SearchMusic *ui;
};

#endif // SEARCHMUSIC_H
