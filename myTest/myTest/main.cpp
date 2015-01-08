#include <QCoreApplication>
#include <iostream>
#include "QDebug"
#include "foobar.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "LMAA";

    foobar blubb;

    blubb.getTest();

    return a.exec();
}
