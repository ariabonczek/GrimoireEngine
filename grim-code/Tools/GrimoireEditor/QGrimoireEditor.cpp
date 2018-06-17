#include "Precompiled.h"

#include "QGrimoireEditor.h"

#include <Generated\generated_Transform.h>
#include <Engine/Gfx.h>

static const char* DATA_PATH = getenv("DATA_PATH");
static const char* CODE_PATH = getenv("CODE_PATH");
static const char* m_lmdbFilename = "datashare.lmdb";

QGrimoireEditor::QGrimoireEditor(QWidget* parent) :
	QMainWindow(parent)
{
	resize(1600, 900);
	setWindowTitle("Grimoire Editor");

	// Menu Bar
	m_fileMenu = menuBar()->addMenu("File");
	
	m_loadGrimoireAction = new QAction("Open Grimoire", this);
	m_loadGrimoireAction->setShortcuts(QKeySequence::Open);
	m_loadGrimoireAction->setStatusTip("Select and load a .grimoire file");
	connect(m_loadGrimoireAction, &QAction::triggered, this, &QGrimoireEditor::LoadGrimoireFile);

	m_fileMenu->addAction(m_loadGrimoireAction);

	// Tool Bar
	toolBar = addToolBar("ToolBar");
	toolBar->addAction("Create");

	// Widgets
	m_gameWindow = new QGameWindow(parent);
	m_gameWindow->resize(1280, 720);
	setCentralWidget(m_gameWindow);

	m_elementBrowser = new QElementBrowser(this);
	m_resourceBrowser = new QResourceBrowser(this);
	m_elementEditor = new QWidget(this);
	m_debugConsole = new QWidget(this);

	m_elementBrowser->setBaseSize(QSize(100, height() * 0.5f));
	m_resourceBrowser->setBaseSize(QSize(100, height() * 0.5f));
	m_elementEditor->setBaseSize(QSize(100, height() * 0.5f));
	m_debugConsole->setBaseSize(QSize(100, height() * 0.5f));

	// Dock Widgets
	m_elementBrowserDock = new QDockWidget("Element Browser", this);
	m_elementBrowserDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	m_elementBrowserDock->setWidget(m_elementBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, m_elementBrowserDock);

	m_resourceBrowserDock = new QDockWidget("Resource Browser", this);
	m_resourceBrowserDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	m_resourceBrowserDock->setWidget(m_resourceBrowser);
	addDockWidget(Qt::LeftDockWidgetArea, m_resourceBrowserDock);

	m_elementEditorDock = new QDockWidget("Element Editor", this);
	m_elementEditorDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	m_elementEditorDock->setWidget(m_elementEditor);
	addDockWidget(Qt::RightDockWidgetArea, m_elementEditorDock);

	m_debugConsoleDock = new QDockWidget("Debug Console", this);
	m_debugConsoleDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	m_debugConsoleDock->setWidget(m_debugConsole);
	addDockWidget(Qt::RightDockWidgetArea, m_debugConsoleDock);

	// Open the LMDB environment
	char path[256];
	snprintf(path, 256, "%s\\%s", CODE_PATH, m_lmdbFilename);
	env.Open(path);

	// Send relevant data
	env.WriteValue<HWND>("windowHandle", (HWND)m_gameWindow->winId());

	m_currentGrimoire = new grim::Grimoire();
	show();
}

QGrimoireEditor::~QGrimoireEditor()
{}

void QGrimoireEditor::closeEvent(QCloseEvent *event)
{
	// trigger a close event on the game
	gfx::TriggerClose();

	event->accept();
	exit(0);	// TODO: Remove this hack and find the root cause
}

void QGrimoireEditor::LoadGrimoireFile()
{
	QString chosenFile = QFileDialog::getOpenFileName(this, "Open Grimoire File", DATA_PATH, "Grimoire Files(*.grimoire)");
	if (chosenFile.isNull())
		return;
	
	m_currentGrimoire->ReadFromFile_Json(chosenFile.toStdString().c_str());
	m_currentGrimoire->WriteToFile_Binary("helloGrimBinary.grimBinary");
	m_currentGrimoire->ClearGrimoire();
	m_currentGrimoire->ReadFromFile_Binary("helloGrimBinary.grimBinary");

	// TEST
	char path[256];
	snprintf(path, 256, "%s\\meshes\\cube.fbx", DATA_PATH);
	grim::MeshResource resource;
	LoadMeshesFromFile(resource, path);
	// TEST
}