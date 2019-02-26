#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent)
{
    fileMenu = new QMenu(TRANSFER("文件"),this);
    plainMenu = new QMenu(TRANSFER("明文"),this);
    fontMenu = new QMenu(TRANSFER("字体"),this);
    aboutMenu = new QMenu(TRANSFER("关于"),this);
    newAction = new QAction(TRANSFER("新建空白文档"),this);
    openAction = new QAction(TRANSFER("打开密文文件"),this);
    saveAction = new QAction(TRANSFER("保存密文"),this);
    saveAsAction = new QAction(TRANSFER("另存为"),this);
    shutAction = new QAction(TRANSFER("关闭文档"),this);
    exitAction = new QAction(TRANSFER("退出应用"),this);
    importAction = new QAction(TRANSFER("导入代码"),this);
    exportCodeAction = new QAction(TRANSFER("导出代码"),this);
    exportTextAction = new QAction(TRANSFER("导出无样式结果"),this);
    fontAction = new QAction(TRANSFER("字体设置"),this);
    aboutSepAction = new QAction(TRANSFER("关于Sep"),this);
    aboutQtAction = new QAction(TRANSFER("关于Qt"),this);
    mainWidget = new QSplitter(Qt::Horizontal,this);
    codeEdit = new QTextEdit(this);
    resultEdit = new QTextEdit(this);
    settings = new QSettings(qApp->organizationName(),
                             qApp->applicationName(),
                             this);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(plainMenu);
    menuBar()->addMenu(fontMenu);
    menuBar()->addMenu(aboutMenu);

    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(shutAction);
    fileMenu->addAction(exitAction);

    plainMenu->addAction(importAction);
    plainMenu->addAction(exportCodeAction);
    plainMenu->addAction(exportTextAction);

    fontMenu->addAction(fontAction);

    aboutMenu->addAction(aboutSepAction);
    aboutMenu->addAction(aboutQtAction);

    mainWidget->addWidget(codeEdit);
    mainWidget->addWidget(resultEdit);

    newAction->setShortcut(QKeySequence(tr("Ctrl+N")));
    openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
    saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    saveAsAction->setShortcut(QKeySequence(tr("Ctrl+Shift+S")));
    shutAction->setShortcut(QKeySequence(tr("Ctrl+Alt+F4")));
    exitAction->setShortcut(QKeySequence(tr("Alt+F4")));
    importAction->setShortcut(QKeySequence(tr("Ctrl+I")));
    exportCodeAction->setShortcut(QKeySequence(tr("Ctrl+E")));
    exportTextAction->setShortcut(QKeySequence(tr("Ctrl+Alt+E")));

    codeEdit->setReadOnly(false);
    resultEdit->setReadOnly(true);
    codeEdit->setStyleSheet(tr("font-family: Courier New; background-color: #FFFFFF"));
    resultEdit->setStyleSheet(tr("font-family: Courier New; background-color: #DDDDDD"));

    setSaved(true);
    setCentralWidget(mainWidget);
    setWindowIcon(QIcon(TRANSFER("sep.ico")));

    connect(newAction,SIGNAL(triggered()),
            this,SLOT(newDocument()));
    connect(openAction,SIGNAL(triggered()),
            this,SLOT(openEncryptedFile()));
    connect(saveAction,SIGNAL(triggered()),
            this,SLOT(saveEncryptedly()));
    connect(saveAsAction,SIGNAL(triggered()),
            this,SLOT(saveAsEncryptedly()));
    connect(shutAction,SIGNAL(triggered()),
            this,SLOT(closeCurrentDocument()));
    connect(exitAction,SIGNAL(triggered()),
            qApp,SLOT(quit()));
    connect(importAction,SIGNAL(triggered()),
            this,SLOT(importPlainCode()));
    connect(exportCodeAction,SIGNAL(triggered()),
            this,SLOT(exportPlainCode()));
    connect(exportTextAction,SIGNAL(triggered()),
            this,SLOT(exportPlainText()));
    connect(fontAction,SIGNAL(triggered()),
            this,SLOT(setFont()));
    connect(aboutSepAction,SIGNAL(triggered()),
            this,SLOT(aboutSep()));
    connect(aboutQtAction,SIGNAL(triggered()),
            qApp,SLOT(aboutQt()));
    connect(codeEdit,SIGNAL(textChanged()),
            this,SLOT(codeChanged()));

    settings->beginGroup(tr("MainWindow"));
    resize(settings->value(tr("size"),sizeHint()).toSize());
    QFont f = settings->value(tr("font"),qApp->font()).value<QFont>();
    codeEdit->setFont(f);
    resultEdit->setFont(f);
    settings->endGroup();

    if (qApp->argc() >= 2)
    {
        QString filePath = qApp->arguments()[1];
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this,TRANSFER("错误"),
                                  TRANSFER("无法打开文件'%1'")
                                  .arg(filePath));
            return;
        }
        QDataStream in(&file);
        quint32 afterHash;
        QString encryptedCode;
        in >> afterHash >> encryptedCode;
        file.close();
        if (qHash(KEY) != afterHash)
        {
            QMessageBox::critical(this,TRANSFER("错误"),TRANSFER("文件损坏(秘钥不一致)"));
            return;
        }
        newDocument();
        codeEdit->setText(decrypt(encryptedCode));
        codeChanged();
        path = filePath;
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    if (!saved)
    {
        int res = QMessageBox::question(this,TRANSFER("是否保存"),TRANSFER("您正在编辑一个文档，没有保存。是否保存？"),
                                        TRANSFER("保存"),TRANSFER("不保存"),TRANSFER("取消退出"));
        if (res == 0 && !saveEncryptedly())
            res = 2;
        if (res == 2)
            return ev->ignore();
    }
    QFileInfo info(path);
    settings->beginGroup(tr("MainWindow"));
    settings->setValue(tr("size"),size());
    settings->setValue(tr("font"),codeEdit->font());
    if (!path.isEmpty())
    {
        QString d = info.absoluteDir().path();
        settings->setValue(tr("dir"),d);
    }
    settings->endGroup();
    ev->accept();
}

QString MainWindow::encrypt(const QString &plain)
{
    QByteArray data = qPrintable(plain);
    QByteArray res = "";
    quint8 key = KEY;
    foreach (char i, data)
        res.append(key^i);
    return tr(res.toBase64().constData());
}
QString MainWindow::decrypt(const QString &encrypted)
{
    QByteArray data = qPrintable(encrypted);
    data = QByteArray(QByteArray::fromBase64(data));
    QByteArray res = "";
    quint8 key = KEY;
    foreach (char i, data)
        res.append(key^i);
    return tr(res.constData());
}
QString MainWindow::dir()
{
    settings->beginGroup(tr("MainWindow"));
    QString ret = settings->value(tr("dir"),tr(".")).toString();
    settings->endGroup();
    return ret;
}

void MainWindow::newDocument()
{
    if (!saved && !saveEncryptedly())
        return;
    codeEdit->clear();
    resultEdit->clear();
    path = "";
    setSaved(false);
}
bool MainWindow::openEncryptedFile()
{
    if (!saved)
    {
        int res = QMessageBox::question(this,TRANSFER("保存"),TRANSFER("当前文档未保存，是否保存?"),
                                        TRANSFER("保存"),TRANSFER("不保存"),TRANSFER("取消"));
        if (res == 2)
            return false;
        if (res == 0 && !saveEncryptedly())
            return false;
    }
    QString filePath = QFileDialog::getOpenFileName(this,TRANSFER("打开文件"),
                                                    dir(),tr("(*)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,TRANSFER("错误"),
                              TRANSFER("无法打开文件'%1'")
                              .arg(filePath));
        return false;
    }
    QDataStream in(&file);
    quint32 afterHash;
    QString encryptedCode;
    in >> afterHash >> encryptedCode;
    file.close();
    if (qHash(KEY) != afterHash)
    {
        QMessageBox::critical(this,TRANSFER("错误"),TRANSFER("文件损坏(秘钥不一致)"));
        return false;
    }
    newDocument();
    codeEdit->setText(decrypt(encryptedCode));
    codeChanged();
    path = filePath;
    setSaved(true);
    return true;
}
bool MainWindow::saveEncryptedly()
{
    if (!path.isEmpty())
    {
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            int res = QMessageBox::question(this,TRANSFER("是否另存为"),
                                            TRANSFER("无法打开文件'%1'，是否另存为？").arg(path),
                                            TRANSFER("另存为"),TRANSFER("不保存"));
            if (res == 0)
                return saveAsEncryptedly();
            return false;
        }
        QDataStream out(&file);
        out << (quint32)qHash(KEY) << encrypt(codeEdit->toPlainText());
        file.close();
        setSaved(true);
        return true;
    }
    return saveAsEncryptedly();
}
bool MainWindow::saveAsEncryptedly()
{
    QString filePath = QFileDialog::getSaveFileName(this,TRANSFER("另存为"),
                                                dir(),tr("SEP (*.sep);(*)"));
    if (filePath.isEmpty())
        return false;
    QString oldPath = path;
    path = filePath;
    if (!saveEncryptedly())
    {
        path = oldPath;
        return false;
    }
    setSaved(true);
    return true;
}
bool MainWindow::closeCurrentDocument()
{
    if (!saved &&!saveEncryptedly())
        return false;
    resultEdit->clear();
    codeEdit->clear();
    codeEdit->setEnabled(false);
    path = "";
    return true;
}
bool MainWindow::importPlainCode()
{
    if (!saved && !saveEncryptedly())
        return false;
    QString filePath = QFileDialog::getOpenFileName(this,TRANSFER("打开文件"),
                                                    dir(),tr("(*)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,TRANSFER("错误"),
                              TRANSFER("无法打开文件%1")
                              .arg(filePath));
        return false;
    }
    QTextStream in(&file);
    QString code = in.readAll();
    file.close();
    codeEdit->setText(code);
    codeChanged();
    setSaved(false);
    return true;
}
bool MainWindow::exportPlainCode()
{
    QString filePath = QFileDialog::getSaveFileName(this,TRANSFER("导出代码"),
                                                    dir(),tr("(*)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(this,TRANSFER("错误"),
                              TRANSFER("无法打开文件'%1'").arg(filePath));
        return false;
    }
    QTextStream out(&file);
    out << codeEdit->toPlainText();
    return true;
}
bool MainWindow::exportPlainText()
{
    QString filePath = QFileDialog::getSaveFileName(this,TRANSFER("导出结果"),
                                                    dir(),tr("(*)"));
    if (filePath.isEmpty())
        return false;
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(this,TRANSFER("错误"),
                              TRANSFER("无法打开文件'%1'").arg(filePath));
        return false;
    }
    QTextStream out(&file);
    out << resultEdit->toPlainText();
    return true;
}
void MainWindow::setFont()
{
    FontEditor *editor = new FontEditor(codeEdit->font(),this);
    bool ok;
    QFont f = QFontDialog::getFont(&ok,codeEdit->font(),this,TRANSFER("设置字体"));
    if (!ok)
        return;
    codeEdit->setFont(f);
    resultEdit->setFont(f);
    delete editor;
}

void MainWindow::aboutSep()
{
    QMessageBox::about(this,TRANSFER("关于Sep"),
                       TRANSFER("%1\n%2\n%3\n%4\n%5")
                       .arg(TRANSFER("sep提供加密数据和编辑效果"))
                       .arg(TRANSFER("使用简洁的代码实现较好的文字效果"))
                       .arg(TRANSFER("同时提供数据加密与保存"))
                       .arg(TRANSFER("也可以导入或导出代码文件"))
                       .arg(TRANSFER("具体使用方法请见http://www.topfyf.com/sep/")));
}
void MainWindow::codeChanged()
{
    setSaved(false);
    QString code = codeEdit->toPlainText();
    code = code.replace(tr("<"),tr("&lt;"));
    code = code.replace(tr(">"),tr("&gt;"));
    code = code.replace(tr("\t"),tr("    "));
    code = code.replace(tr(" "),tr("&nbsp;"));
    code = code.replace(tr("&;"),tr("&zwj;"));
    QString html;
    QStringList lines1 = code.split("\r\n");
    QStringList lines2;
    foreach (QString line, lines1)
        lines2.append(line.split('\n'));
    QStringList lines;
    foreach (QString line, lines2)
        lines.append(line.split('\r'));
    foreach (QString line, lines)
    {
        int num = 0;
        for (int i = 0;i<line.size();i++)
        {
            if (line[i] == '#')
                num++;
            else
                break;
            if (num == 6)
                break;
        }
        int start = 0;
        if (num>0)
        {
            line = tr("<h%1>%2</h%3>")
                    .arg(num)
                    .arg(line.mid(num))
                    .arg(num);
            start = 4;
        }
        if (line.mid(start,2) == "$$")
        {
            if (start == 0)
                line = tr("<center>%1</center>").arg(line.mid(2));
            else
                line = tr("<center>%1%2</center>")
                        .arg(line.mid(0,4))
                        .arg(line.mid(6));
        }
        QRegExp italic("@@(.*)@@");
        QRegExp bold("``(.*)``");
        QRegExp color("\\*(.*)\\*(.*)\\*(.*)\\*");
        italic.setMinimal(true);
        bold.setMinimal(true);
        color.setMinimal(true);
        line = line.replace(italic,tr("<i>\\1</i>"));
        line = line.replace(bold,tr("<b>\\1</b>"));
        line = line.replace(color,tr("<span style='color:\\1'><span style=' background-color: \\3'>\\2</span></span>"));
        html += tr("<p style='margin: 0'>%1</p>").arg(line);
    }
    resultEdit->setHtml(html);
}
void MainWindow::setSaved(bool save)
{
    saved = save;
    setWindowTitle(TRANSFER("Sep加密效果编辑器%1")
                   .arg(save?"":"*"));
}
