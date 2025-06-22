#ifndef FAKERESULTS_H
#define FAKERESULTS_H

#include <QDialog>

namespace Ui {
class FakeResults;
}

class FakeResults : public QDialog
{
    Q_OBJECT

public:
    explicit FakeResults(QWidget *parent = nullptr);
    ~FakeResults();

private:
    Ui::FakeResults *ui;
};

#endif // FAKERESULTS_H
