#pragma once

#include <QtWidgets/QtWidgets>

class QResourceModel : public QFileSystemModel
{
public:
	QResourceModel();
	~QResourceModel();
private:

};

class QResourceBrowser : public QTreeView
{
public:
	QResourceBrowser(QWidget* parent = nullptr);
	~QResourceBrowser();
private:
	QResourceModel* m_resourceModel;
};