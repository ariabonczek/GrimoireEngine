#include "Precompiled.h"

#include <QtWidgets/QApplication>
#include "QGrimoireEditor.h"

#include <Engine/EngineMain.h>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QGrimoireEditor editor;

	char* engineArgs[] = { "", "-bootingFromEditor" };
	EngineMain(2, engineArgs);

	int ret = app.exec();
	return ret;
}