#include "titlebar.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QDialog>
#include <QIntValidator>

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("QPushButton { color:white; }"
                  "QPushButton:pressed { background:rgba(0,131,221,100); }"
                  "QPushButton::menu-indicator{ width:0px; }"
                  "QMenu::item:selected { }"
                  "QLineEdit { font-size:12px; background-color:#CFEBF9; border:2px solid #CFEBF9; border-radius:10px;}"
                  "#lineEditSearch { padding-left:10px;}");
    setFixedHeight(50);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setSpacing(0);

    QLabel *label_icon = new QLabel;
    label_icon->setFixedSize(40,40);
    label_icon->setPixmap(QPixmap(":/icon/KuGou.svg").scaled(label_icon->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    hbox->addWidget(label_icon);
    hbox->addStretch();

    lineEdit_search = new QLineEdit;
    lineEdit_search->setObjectName("lineEditSearch");
    lineEdit_search->setPlaceholderText("搜索音乐、歌手、歌词、用户");
    lineEdit_search->setFixedWidth(200);
    action_search = new QAction(this);
    //action_search->setIcon(QIcon(":/icon/search.svg"));
    action_search->setIcon(QIcon::fromTheme("search"));
    lineEdit_search->addAction(action_search, QLineEdit::TrailingPosition);
    hbox->addWidget(lineEdit_search);

    pushButton_lastPage = new QPushButton;
    pushButton_lastPage->setFixedSize(24,24);
    pushButton_lastPage->setIcon(QIcon(":/icon/lastpage.svg"));
    pushButton_lastPage->setIconSize(QSize(20,20));
    pushButton_lastPage->setFlat(true);
    pushButton_lastPage->setFocusPolicy(Qt::NoFocus);
    pushButton_lastPage->setCursor(Qt::PointingHandCursor);
    pushButton_lastPage->installEventFilter(this);
    hbox->addWidget(pushButton_lastPage);

    lineEdit_page = new QLineEdit;
    lineEdit_page->setObjectName("lineEditPage");
    lineEdit_page->setText("1");
    QValidator *validator = new QIntValidator(1,99,lineEdit_page);
    lineEdit_page->setValidator(validator);
    lineEdit_page->setFixedWidth(30);
    lineEdit_page->setAlignment(Qt::AlignCenter);
    hbox->addWidget(lineEdit_page);

    pushButton_nextPage = new QPushButton;
    pushButton_nextPage->setFixedSize(24,24);
    pushButton_nextPage->setIcon(QIcon(":/icon/nextpage.svg"));
    pushButton_nextPage->setIconSize(QSize(20,20));
    pushButton_nextPage->setFlat(true);
    pushButton_nextPage->setFocusPolicy(Qt::NoFocus);
    pushButton_nextPage->setCursor(Qt::PointingHandCursor);
    pushButton_nextPage->installEventFilter(this);
    hbox->addWidget(pushButton_nextPage);

    hbox->addStretch();

    QPushButton *pushButton_menu = new QPushButton;
    pushButton_menu->setFixedSize(24,24);
    pushButton_menu->setIcon(QIcon(":/icon/menu.svg"));
    pushButton_menu->setIconSize(QSize(20,20));
    pushButton_menu->setFlat(true);
    pushButton_menu->setFocusPolicy(Qt::NoFocus);
    pushButton_menu->setCursor(Qt::PointingHandCursor);
    QMenu *submenu = new QMenu(this);
    QAction *action_login = new QAction("登录", this);
    action_set = new QAction("设置", this);
    QAction *action_about = new QAction("关于", this);
    QAction *action_quit = new QAction("退出", this);
    submenu->addAction(action_login);
    submenu->addAction(action_set);
    submenu->addAction(action_about);
    submenu->addAction(action_quit);
    pushButton_menu->setMenu(submenu);
//  connect(action_login, SIGNAL(triggered()), this, SLOT(login()));
    connect(action_about, SIGNAL(triggered()), this, SLOT(about()));
    connect(action_quit, SIGNAL(triggered()), qApp, SLOT(quit()));
    hbox->addWidget(pushButton_menu);

    pushButton_minimize = new QPushButton;
    pushButton_minimize->setFixedSize(24,24);
    pushButton_minimize->setIcon(QIcon(":/icon/minimize.svg"));
    pushButton_minimize->setIconSize(QSize(20,20));
    pushButton_minimize->setFlat(true);
    pushButton_minimize->setFocusPolicy(Qt::NoFocus);
    pushButton_minimize->setCursor(Qt::PointingHandCursor);
    pushButton_minimize->installEventFilter(this);
    hbox->addWidget(pushButton_minimize);
    hbox->addSpacing(5);

    pushButton_maximize = new QPushButton;
    pushButton_maximize->setFixedSize(24,24);
    pushButton_maximize->setIcon(QIcon(":/icon/maximize.svg"));
    pushButton_maximize->setIconSize(QSize(20,20));
    pushButton_maximize->setFlat(true);
    pushButton_maximize->setFocusPolicy(Qt::NoFocus);
    pushButton_maximize->setCursor(Qt::PointingHandCursor);
    pushButton_maximize->installEventFilter(this);
    hbox->addWidget(pushButton_maximize);
    hbox->addSpacing(5);

    pushButton_close = new QPushButton;
    pushButton_close->setFixedSize(24,24);
    pushButton_close->setIcon(QIcon(":/icon/close.svg"));
    pushButton_close->setIconSize(QSize(20,20));
    pushButton_close->setFlat(true);
    pushButton_close->setFocusPolicy(Qt::NoFocus);
    pushButton_close->setCursor(Qt::PointingHandCursor);
    pushButton_close->installEventFilter(this);
    hbox->addWidget(pushButton_close);

    setLayout(hbox);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        MLBD = true;
        //qDebug() << "mousePress" << event->pos();
        relativePos = event->pos();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (MLBD) {
        setCursor(Qt::ClosedHandCursor);
        emit moveMainWindow(event->globalPos() - relativePos);
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    MLBD = false;
    setCursor(Qt::ArrowCursor);
}

bool TitleBar::eventFilter(QObject *obj, QEvent *event)
{
    //qDebug() << obj->metaObject()->className();
    if ( QString(obj->metaObject()->className()) == "QPushButton" && event->type() == QEvent::MouseMove ) {
        return true;    // filter
    } else {
        return false;
    }
    // pass the event on to the parent class
    return QWidget::eventFilter(obj, event);
}

void TitleBar::about()
{
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("关于");
    dialog->setFixedSize(500,350);
    QVBoxLayout *vbox = new QVBoxLayout;
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap(":/icon/KuGou.svg").scaled(90, 90, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    label->setAlignment(Qt::AlignCenter);
    vbox->addWidget(label);
    label = new QLabel;
    label->setText("酷狗音乐 V2.0");
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size:20px;");
    vbox->addWidget(label);
    label = new QLabel;
    label->setText("         一款基于 Qt5 的酷狗音乐播放器。\n作者：海天鹰\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy/KuGou\n界面和音乐版权：酷狗\n参考：\nhttps://github.com/ecitlm/Kugou-api\nhttps://www.cnblogs.com/xuhui24/p/6555761.html");
    label->setStyleSheet("font-size:15px;");
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignTop);
    vbox->addWidget(label);
    vbox->addStretch();
    dialog->setLayout(vbox);
    dialog->show();
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(82,146,254));
    painter.drawRect(rect());    
}