#include <QtCore/QCoreApplication>
#include "FTPProxy.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	FTPProxy ftpProxy;
	return a.exec();
}
