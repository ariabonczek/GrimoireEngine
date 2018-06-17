#pragma once

#include <QtWidgets/QtWidgets>

class QElementModel : public QStandardItemModel
{
public:
	QElementModel();
	~QElementModel();
private:

};

class QElementBrowser : public QTreeView
{
public:
	QElementBrowser(QWidget* parent = nullptr);
	~QElementBrowser();
private:
	QElementModel* m_elementModel;
};