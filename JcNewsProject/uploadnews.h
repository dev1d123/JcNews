#ifndef UPLOADNEWS_H
#define UPLOADNEWS_H

#include <QDialog>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QToolButton;
class QTextEdit;
class QDateTimeEdit;
class QDialogButtonBox;
QT_END_NAMESPACE

class UploadNews : public QDialog
{
    Q_OBJECT

public:
    explicit UploadNews(QWidget *parent = nullptr);

private slots:
    void loadFile();
    void showNewsInfo();

private:
    void parseFile(const QString& fileName);

    QLineEdit* dirLine;
    QLineEdit* titleLine;
    QToolButton* toolButton;
    QDateTimeEdit* fromTimeEdit;
    QDateTimeEdit* toTimeEdit;
    QTextEdit* contentEdit;
    QDialogButtonBox* buttonBox;
};

#endif // UPLOADNEWS_H
