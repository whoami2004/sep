#include "fonteditor.h"

FontEditor::FontEditor(QFont f, QWidget *parent) :
    QDialog(parent)
{
    fontLabel = new QLabel(TRANSFER("字体设置:"),this);
    sizeLabel = new QLabel(TRANSFER("大小设置:"),this);
    fontEdit = new QLineEdit(this);
    sizeBox = new QSpinBox(this);
    cancelButton = new QPushButton(TRANSFER("放弃更改"),this);
    okButton = new QPushButton(TRANSFER("确认更改"),this);
    fontEdit->setText(f.family());
    sizeBox->setRange(1,50);
    sizeBox->setValue(f.pointSize());
    okButton->setEnabled(false);

    QHBoxLayout *fontLayout = new QHBoxLayout;
    fontLayout->addWidget(fontLabel);
    fontLayout->addWidget(fontEdit);

    QHBoxLayout *sizeLayout = new QHBoxLayout;
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(sizeBox);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(fontLayout);
    mainLayout->addLayout(sizeLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle(TRANSFER("字体设置"));
    setWindowIcon(parent->windowIcon());

    connect(fontEdit,SIGNAL(textChanged(QString)),
            this,SLOT(changed()));
    connect(sizeBox,SIGNAL(valueChanged(int)),
            this,SLOT(changed()));
    connect(cancelButton,SIGNAL(clicked()),
            this,SLOT(reject()));
    connect(okButton,SIGNAL(clicked()),
            this,SLOT(accept()));
}

QString FontEditor::fontSet() const
{return fontEdit->text();}
int FontEditor::sizeSet() const
{return sizeBox->value();}

void FontEditor::changed()
{okButton->setEnabled(true);}
