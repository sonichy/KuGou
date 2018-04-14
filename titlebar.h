#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QAction>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);
    QPushButton *pushButton_login, *pushButton_register, *pushButton_lastPage, *pushButton_nextPage, *pushButton_minimize, *pushButton_maximize, *pushButton_close;
    QLineEdit *lineEdit_search, *lineEdit_page;
    QAction *action_search;

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    bool MLBD;
    QPoint relativePos;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:
    void moveMainWindow(QPoint);

private slots:
    void about();
};

#endif // TITLEBAR_H
