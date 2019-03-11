#include "myclass.h"

MyClass::MyClass(QObject * _stackView, QObject *parent) : QObject(parent)
{
    this->stackView = _stackView;
}
