/**
 * Copyright (c) 2014 Ralph Janke. All rights reserved.
 *
 * Author: Ralph Janke hexnosis@jankeconsulting.ca
 */

#include "hexnosiswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("Janke Consulting");
    a.setOrganizationDomain("jankeconsulting.ca");
    a.setApplicationName("Hexnosis");
    a.setApplicationVersion("0.1");
    HexnosisWindow w;
    w.show();

    return a.exec();
}
