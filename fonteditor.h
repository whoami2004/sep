#ifndef FONTEDITOR_H
#define FONTEDITOR_H

#include <QtGui>

#define TRANSFER(x) QString::fromUtf8(x)

class FontEditor: public QDialog
{
    Q_OBJECT

public:
    explicit FontEditor(QFont f = qApp->font(), QWidget *parent = 0);

    QString fontSet() const;
    int sizeSet() const;

private:
    QLabel *fontLabel;
    QLabel *sizeLabel;
    QLineEdit *fontEdit;
    QSpinBox *sizeBox;
    QPushButton *cancelButton;
    QPushButton *okButton;

private slots:
    void changed();
};

#endif // FONTEDITOR_H
