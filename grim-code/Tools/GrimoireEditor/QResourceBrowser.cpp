#include "Precompiled.h"

#include "QResourceBrowser.h"

static const char* kDataPath = getenv("DATA_PATH");

QResourceModel::QResourceModel()
{}

QResourceModel::~QResourceModel()
{}

QResourceBrowser::QResourceBrowser(QWidget* parent) :
	QTreeView(parent)
{
	m_resourceModel = new QResourceModel();
	m_resourceModel->setRootPath(kDataPath);

	this->setModel(m_resourceModel);
	this->setRootIndex(m_resourceModel->index(kDataPath));

}

QResourceBrowser::~QResourceBrowser()
{}