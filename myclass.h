#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QDebug>
class MyClass : public QObject
{
    Q_OBJECT
public:
    explicit MyClass(QObject * _stackView, QObject *parent = nullptr);

signals:

public slots:
    void cppSlot(const QString &msg) {
        qDebug() << "Called the C++ slot with message:" << msg;
        this->stackView->setProperty( "currentIndex", 2 );
    }

private:
    QObject * stackView;
};

#endif // MYCLASS_H

