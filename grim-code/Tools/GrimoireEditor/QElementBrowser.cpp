#include "Precompiled.h"

#include "QElementBrowser.h"

static const char* kDataPath = getenv("DATA_PATH");

QElementModel::QElementModel()
{}

QElementModel::~QElementModel()
{}

QElementBrowser::QElementBrowser(QWidget* parent) :
	QTreeView(parent)
{
	m_elementModel = new QElementModel();

} 

QElementBrowser::~QElementBrowser()
{}