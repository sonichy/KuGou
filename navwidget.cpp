#include "navwidget.h"
#include <QVBoxLayout>

NavWidget::NavWidget(QWidget *parent) : QWidget(parent)
{
    setFixedWidth(200);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    pushButton_albumPic = new QPushButton;
    pushButton_albumPic->setFixedSize(74,74);
    pushButton_albumPic->setIconSize(QSize(70,70));
    pushButton_albumPic->setFocusPolicy(Qt::NoFocus);
    pushButton_albumPic->setFlat(true);
    hbox->addWidget(pushButton_albumPic);
    label_songname = new QLabel;
    label_songname->setStyleSheet("font-size:13px;");
    hbox->addWidget(label_songname);
    hbox->addStretch();
    vbox->addLayout(hbox);
    vbox->addStretch();
    setLayout(vbox);
}
