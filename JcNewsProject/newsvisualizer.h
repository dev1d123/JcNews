#ifndef NEWSVISUALIZER_H
#define NEWSVISUALIZER_H

#include <QDialog>

namespace Ui {
class NewsVisualizer;
}

class NewsVisualizer : public QDialog
{
    Q_OBJECT

public:
    explicit NewsVisualizer(QWidget *parent = nullptr);
    ~NewsVisualizer();

private:
    Ui::NewsVisualizer *ui;
};

#endif // NEWSVISUALIZER_H
