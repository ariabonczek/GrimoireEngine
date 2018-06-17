#include "Precompiled.h"

#include "QGameWindow.h"

QGameWindow::QGameWindow(QWidget* parent) :
	QFrame(parent)
{
	this->setFrameStyle(QFrame::Panel | QFrame::Raised);
	this->setLineWidth(2);
}

QGameWindow::~QGameWindow()
{}