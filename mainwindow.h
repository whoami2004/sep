#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fonteditor.h"
#include <QtGui>

#define KEY 0x5F

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *ev);

private:
    QMenu *fileMenu;
    QMenu *plainMenu;
    QMenu *fontMenu;
    QMenu *aboutMenu;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *shutAction;
    QAction *exitAction;
    QAction *importAction;
    QAction *exportCodeAction;
    QAction *exportTextAction;
    QAction *fontAction;
    QAction *aboutSepAction;
    QAction *aboutQtAction;
    QSplitter *mainWidget;
    QTextEdit *codeEdit;
    QTextEdit *resultEdit;
    QSettings *settings;
    QString path;
    bool saved;

private:
    static QString encrypt(const QString &plain);
    static QString decrypt(const QString &encrypted);
    QString dir();

private slots:
    void newDocument();
    bool openEncryptedFile();
    bool saveEncryptedly();
    bool saveAsEncryptedly();
    bool closeCurrentDocument();
    bool importPlainCode();
    bool exportPlainCode();
    bool exportPlainText();
    void setFont();
    void aboutSep();
    void codeChanged();
    void setSaved(bool);
};

#endif // MAINWINDOW_H
