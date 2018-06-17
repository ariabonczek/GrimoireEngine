#pragma once

#include <QtWidgets/QtWidgets>

#include "QGameWindow.h"
#include "QElementBrowser.h"
#include "QResourceBrowser.h"

#include <Shared/Core/grimLmdb.h>

#include <Tools/GrimoireBuild/Grimoire.h>

class QGrimoireEditor : public QMainWindow
{
	//Q_OBJECT
public:
	QGrimoireEditor(QWidget* parent = nullptr);
	~QGrimoireEditor();

	void closeEvent(QCloseEvent *event)override;
private slots:
	void LoadGrimoireFile();

private:
	grim::Grimoire* m_currentGrimoire;
	QGameWindow* m_gameWindow;

	QMenu* m_fileMenu;
	QAction* m_loadGrimoireAction;

	QToolBar* toolBar;
	
	QElementBrowser* m_elementBrowser;
	QResourceBrowser* m_resourceBrowser;
	QWidget* m_elementEditor;
	QWidget* m_debugConsole;

	QDockWidget* m_elementBrowserDock;
	QDockWidget* m_resourceBrowserDock;
	QDockWidget* m_elementEditorDock;
	QDockWidget* m_debugConsoleDock;

	grimLmdb::Environment env;
};