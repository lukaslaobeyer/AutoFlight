#include "autoflight.h"
#include "asmainwindow.h"

#include <QtWidgets>
#include <Qsci/qsciapis.h>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <limits>

#define PREF_WIDTH 600
#define PREF_HEIGHT 500

using namespace std;

void ASMainWindow::setWindowAttributes()
{
	setWindowTitle("AutoScript");
	setWindowIcon(QIcon(":/resources/icon.png"));
	setMinimumSize(PREF_WIDTH, PREF_HEIGHT);
}

ASMainWindow::ASMainWindow(ASEngine *e, QWidget *parent) : QMainWindow(parent)
{
	_ase = e;

	_iv = new ImageVisualizer;

	QWidget *w = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	w->setLayout(layout);

	setCentralWidget(w);

	tabs = new QTabWidget();
	tabs->setTabPosition(QTabWidget::North);
	layout->addWidget(tabs);

	///// Set up signal/slot for simulated script inputs

	QObject::connect(this, SIGNAL(simulatedBoolInputSignal(std::string)), this, SLOT(setSimulatedBoolInput(std::string)), Qt::BlockingQueuedConnection);
	QObject::connect(this, SIGNAL(simulatedFloatInputSignal(std::string, std::string)), this, SLOT(setSimulatedFloatInput(std::string, std::string)), Qt::BlockingQueuedConnection);

	///// Code editor

	QFont editorfont;
	editorfont.setFamily("Monospace");
	editorfont.setFixedPitch(true);
	editorfont.setPointSize(11);
	
#ifdef __MINGW32__
	editorfont.setFamily("Consolas");
#endif

	QFontMetrics metrics(editorfont);

	_editor = new QsciScintilla();
	_editor->setMarginWidth(0, metrics.width("00000") + 5);
	_editor->setMarginsBackgroundColor(QColor("#474747"));
	_editor->setMarginsForegroundColor(QColor("#A6A6A6"));
	_editor->setMarginLineNumbers(0, true);
	_editor->setFont(editorfont);
	_editor->setMarginsFont(editorfont);
	_editor->setAutoIndent(true);
	_editor->setIndentationsUseTabs(false);
	_editor->setIndentationWidth(4);
	_editor->setCaretForegroundColor(QColor("#839E83"));
	_editor->setCaretLineVisible(true);
	_editor->setCaretLineBackgroundColor(QColor("#424242"));
	_editor->setSelectionBackgroundColor(QColor("#2C3631"));
	_editor->setSelectionForegroundColor(QColor("#BABABA"));
	_editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
	_editor->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QsciLexerPython *pythonLexer = new QsciLexerPython();
	applyTheme(pythonLexer, editorfont);
	_editor->setLexer(pythonLexer);

	QsciAPIs *api = new QsciAPIs(pythonLexer);
	vector<string> commands = _ase->getAvailableFunctions();
	for(unsigned int i = 0; i < commands.size(); i++)
	{
		api->add(QString::fromStdString(commands[i]));
	}

	api->prepare();
	_editor->setAutoCompletionCaseSensitivity(false);
	_editor->setAutoCompletionThreshold(4);
	_editor->setAutoCompletionSource(QsciScintilla::AcsAPIs);

	_editor->setText(tr( "# \n"
						 "# AutoScript is based on Python. You can use all of\n"
    					 "# Python's features. There are predefined functions to\n"
    					 "# control the behavior of the Drone and more. Check out\n"
    					 "# the tutorials and documentation in the help menu.\n"
    					 "# \n"
    					 "# \n"
    					 "# Enjoy! \n"
						 "# \n"
    					 "\nprint('Hello World')"));

	tabs->addTab(_editor, tr("Untitled.ascr"));
	QObject::connect(_editor, SIGNAL(textChanged()), this, SLOT(textEdited()));
	QObject::connect(_editor, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(cursorChanged(int, int)));
	QObject::connect(this, SIGNAL(highlightErrorSignal(int)), this, SLOT(highlightError(int)));

	///// Script output

	_output = new QTextEdit();
	_output->setFont(editorfont);
	_output->setStyleSheet("color: white; background: black;");
	_output->setTextInteractionFlags(Qt::NoTextInteraction);

	tabs->addTab(_output, tr("Script output"));

	QObject::connect(this, SIGNAL(scriptOutputAvailable(QString)), this, SLOT(updateOutputTextWidget(QString)));
	QObject::connect(this, SIGNAL(scriptStoppedSignal()), this, SLOT(scriptStopped()));

	///// Menu Bar

	QMenu *file = menuBar()->addMenu(tr("File"));
		QAction *newFile = new QAction(tr("New"), file);
		newFile->setShortcut(QKeySequence::New);
		file->addAction(newFile);
		QObject::connect(newFile, SIGNAL(triggered()), this, SLOT(newFile()));

		QAction *openFile = new QAction(tr("Open"), file);
		openFile->setShortcut(QKeySequence::Open);
		file->addAction(openFile);
		QObject::connect(openFile, SIGNAL(triggered()), this, SLOT(openFile()));

		file->addSeparator();

		QAction *saveFile = new QAction(tr("Save"), file);
		saveFile->setShortcut(QKeySequence::Save);
		file->addAction(saveFile);
		QObject::connect(saveFile, SIGNAL(triggered()), this, SLOT(saveFile()));

		QAction *saveFileAs = new QAction(tr("Save as"), file);
		saveFileAs->setShortcut(QKeySequence::SaveAs);
		file->addAction(saveFileAs);
		QObject::connect(saveFileAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));

	QMenu *script = menuBar()->addMenu(tr("Script"));
		sim_m = new QAction(tr("Simulate"), script);
		script->addAction(sim_m);
		QObject::connect(sim_m, SIGNAL(triggered()), this, SLOT(simulateScript()));

		exec_m = new QAction(tr("Execute"), file);
		script->addAction(exec_m);
		QObject::connect(exec_m, SIGNAL(triggered()), this, SLOT(startScript()));

		abort_m = new QAction(tr("Abort script execution"), script);
		abort_m->setEnabled(false);
		script->addAction(abort_m);
		QObject::connect(abort_m, SIGNAL(triggered()), this, SLOT(abortScript()));

	QMenu *help = menuBar()->addMenu(tr("Help"));
		QAction *documentation = new QAction(tr("Documentation"), help);
		documentation->setShortcut(QKeySequence::HelpContents);
		help->addAction(documentation);
		QObject::connect(documentation, SIGNAL(triggered()), this, SLOT(showDocs()));

		help->addSeparator();

		QAction *about = new QAction(tr("About AutoScript"), help);
		help->addAction(about);

	///// Toolbar

	QToolBar *bar = new QToolBar();
	bar->setContextMenuPolicy(Qt::PreventContextMenu);
	bar->setMovable(false);
	addToolBar(Qt::BottomToolBarArea, bar);

	QAction *save = new QAction(QIcon(":/resources/as_save.png"), tr("Save script"), bar);
	QAction *open = new QAction(QIcon(":/resources/as_open.png"), tr("Open script"), bar);
	QObject::connect(save, SIGNAL(triggered()), this, SLOT(saveFile()));
	QObject::connect(open, SIGNAL(triggered()), this, SLOT(openFile()));

	bar->addAction(save);
	bar->addAction(open);
	bar->addSeparator();

	sim = new QAction(QIcon(":/resources/as_sim.png"), tr("Simulate script"), bar);
	exec = new QAction(QIcon(":/resources/as_exec.png"), tr("Execute script"), bar);
	abort = new QAction(QIcon(":/resources/as_abort.png"), tr("Abort execution"), bar);
	abort->setEnabled(false);
	QObject::connect(sim, SIGNAL(triggered()), this, SLOT(simulateScript()));
	QObject::connect(exec, SIGNAL(triggered()), this, SLOT(startScript()));
	QObject::connect(abort, SIGNAL(triggered()), this, SLOT(abortScript()));

	bar->addAction(sim);
	bar->addAction(exec);
	bar->addAction(abort);
	bar->addSeparator();

	QAction *landOnError = new QAction(QIcon(":/resources/as_landonerror.png"), tr("Land on error"), bar);
	landOnError->setCheckable(true);

	bar->addAction(landOnError);

	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	bar->addWidget(spacer);

	QLabel *python_v = new QLabel(QString("Python ").append(QString::fromStdString(_ase->getPythonVersion())));
	python_v->setStyleSheet("color: #BBBBBB; font-size: 24px;");

	bar->addWidget(python_v);

	///// Documentation

	_docs = new QWebView();
	_docs->load(QUrl::fromLocalFile(QString::fromStdString(AutoFlight::getProgramDirectory()).append("docs/AutoScript/index.html")));
	tabs->addTab(_docs, tr("Documentation"));

	setWindowAttributes();
}

ASMainWindow::~ASMainWindow()
{
	deleteScriptThread();
}

void ASMainWindow::applyTheme(QsciLexerPython *l, QFont font)
{
	QFont fontB = font;
	fontB.setBold(true);
	QFont fontI = font;
	fontI.setItalic(true);

	l->setDefaultPaper(QColor("#1D1D1D"));

	l->setColor(QColor("#E08056"), QsciLexerPython::Default);
	l->setFont(font, QsciLexerPython::Default);
	l->setColor(QColor("#7F9F7F"), QsciLexerPython::Comment);
	l->setFont(fontI, QsciLexerPython::Comment);
	l->setColor(QColor("#7F9F7F"), QsciLexerPython::CommentBlock);
	l->setFont(fontI, QsciLexerPython::CommentBlock);
	l->setColor(QColor("#85C2C2"), QsciLexerPython::Number);
	l->setFont(font, QsciLexerPython::Number);
	l->setColor(QColor("#CEDAE3"), QsciLexerPython::SingleQuotedString);
	l->setFont(fontI, QsciLexerPython::SingleQuotedString);
	l->setColor(QColor("#CEDAE3"), QsciLexerPython::DoubleQuotedString);
	l->setFont(fontI, QsciLexerPython::DoubleQuotedString);
	l->setColor(QColor("#E08056"), QsciLexerPython::Keyword);
	l->setFont(fontB, QsciLexerPython::Keyword);
	l->setColor(QColor("#85C2C2"), QsciLexerPython::TripleSingleQuotedString);
	l->setFont(fontI, QsciLexerPython::TripleSingleQuotedString);
	l->setColor(QColor("#85C2C2"), QsciLexerPython::TripleDoubleQuotedString);
	l->setFont(fontI, QsciLexerPython::TripleDoubleQuotedString);
	l->setColor(QColor("#E08056"), QsciLexerPython::ClassName);
	l->setFont(font, QsciLexerPython::ClassName);
	l->setColor(QColor("#E08056"), QsciLexerPython::FunctionMethodName);
	l->setFont(font, QsciLexerPython::FunctionMethodName);
	l->setColor(QColor("#9F9D6D"), QsciLexerPython::Operator);
	l->setFont(font, QsciLexerPython::Operator);
	l->setColor(QColor("#DFC47D"), QsciLexerPython::Identifier);
	l->setFont(font, QsciLexerPython::Identifier);
	l->setColor(QColor("#AEC2D2"), QsciLexerPython::UnclosedString);
	l->setFont(font, QsciLexerPython::UnclosedString);
}

void ASMainWindow::textEdited()
{
	if(!documentEdited)
	{
		documentEdited = true;
		tabs->setTabText(0, QString("*").append(tabs->tabText(0)));
	}
}

void ASMainWindow::highlightError(int lineNo)
{
	// TODO: Add an error marker to the editor. Somehow I have not been able to get Scintilla markers to work yet.
}

void ASMainWindow::newFile()
{
	// Confirmation dialog when there are unsaved changes
	if(documentEdited)
	{
		if(QMessageBox::question(this, tr("Discard changes"), tr("The current script has unsaved changes. Really proceed? All changes will be lost."), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
		{
			return;
		}
	}

	_editor->setText("");
	tabs->setTabText(0, tr("Untitled.ascr"));
	documentEdited = false;
	fileAlreadySavedAs = false;
	fileSavedAs = "";
}

void ASMainWindow::openFile()
{
	// Confirmation dialog when there are unsaved changes
	if(documentEdited)
	{
		if(QMessageBox::question(this, tr("Discard changes"), tr("The current script has unsaved changes. Really proceed? All changes will be lost."), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
		{
			return;
		}
	}


	QString filePath = QFileDialog::getOpenFileName(this, tr("Open script"), "", tr("AutoFlight Scripts (*.ascr *.py);;Any File (*.*)"));
	if(filePath.isNull())
	{
		return;
	}

	QString fileName;

	if(filePath.indexOf("\\") > 0)
	{
		fileName = filePath.mid(filePath.lastIndexOf("\\") + 1);
	}
	else
	{
		fileName = filePath.mid(filePath.lastIndexOf("/") + 1);
	}

	try
	{
		ifstream ifs;
		stringstream fileContents;
		ifs.open(filePath.toStdString(), ifstream::in);
		fileContents << ifs.rdbuf();
		ifs.close();

		_editor->setText(QString::fromStdString(fileContents.str()));
		tabs->setTabText(0, fileName);

		documentEdited = false;
		fileAlreadySavedAs = true;
		fileSavedAs = filePath.toStdString();
	}
	catch(ifstream::failure &e)
	{
		QMessageBox::critical(this, tr("Could not open file"), tr("Error reading file.\n").append(e.what()));
	}
}

bool ASMainWindow::writeFile(std::string path, std::string content)
{
	try
	{
		ofstream ofs;
		ofs.open(path);
		ofs << content;
		ofs.close();

		return true;
	}
	catch(ofstream::failure &e)
	{
		cout << "Could not save file: " << e.what() << endl;

		return false;
	}
}

void ASMainWindow::setTabName()
{
	QString fileName;
	QString filePath = QString::fromStdString(fileSavedAs);

	if(filePath.indexOf("\\") > 0)
	{
		fileName = filePath.mid(filePath.lastIndexOf("\\") + 1);
	}
	else
	{
		fileName = filePath.mid(filePath.lastIndexOf("/") + 1);
	}

	tabs->setTabText(0, fileName);
}

void ASMainWindow::saveFile()
{
	if(!fileAlreadySavedAs)
	{
		saveFileAs();
		return;
	}

	if(writeFile(fileSavedAs, _editor->text().toStdString()))
	{
		documentEdited = false;

		setTabName();
	}
	else
	{
		QMessageBox::critical(this, tr("Could not save file"), tr("The file could not be saved. Please make sure you have write permissions for the directory you attempted to save the file in."));
	}
}

void ASMainWindow::saveFileAs()
{
	QString filePath = QFileDialog::getSaveFileName(this, tr("Save script"), "", tr("AutoFlight Scripts (*.ascr *.py);;Any File (*.*)"));
	if(filePath.isNull())
	{
		return;
	}

	if((filePath.indexOf(".") < filePath.lastIndexOf("/")) || (filePath.indexOf(".") < filePath.lastIndexOf("\\")))
	{
		// Extension missing
		filePath.append(".ascr");
	}

	if(writeFile(filePath.toStdString(), _editor->text().toStdString()))
	{
		documentEdited = false;

		fileAlreadySavedAs = true;
		fileSavedAs = filePath.toStdString();

		setTabName();
	}
	else
	{
		QMessageBox::critical(this, tr("Could not save file"), tr("The file could not be saved. Please make sure you have write permissions for the directory you attempted to save the file in."));
	}
}

void ASMainWindow::deleteScriptThread()
{
	if(scriptThread != NULL)
	{
		if(scriptThread->joinable())
		{
			if(_scriptRunning)
			{
				_ase->stopRunningScript();
			}
			scriptThread->join();
		}
		delete scriptThread;
	}
}

void ASMainWindow::startScript()
{
	abort->setEnabled(true);
	abort_m->setEnabled(true);

	sim->setEnabled(false);
	sim_m->setEnabled(false);
	exec->setEnabled(false);
	exec_m->setEnabled(false);

	deleteScriptThread();
	scriptThread = new boost::thread(&ASMainWindow::runScript, this, false);
}

void ASMainWindow::simulateScript()
{
	abort->setEnabled(true);
	abort_m->setEnabled(true);

	sim->setEnabled(false);
	sim_m->setEnabled(false);
	exec->setEnabled(false);
	exec_m->setEnabled(false);

	deleteScriptThread();
	scriptThread = new boost::thread(&ASMainWindow::runScript, this, true);
}

void ASMainWindow::abortScript()
{
	if(_scriptRunning)
	{
		_ase->stopRunningScript();
	}
}

void ASMainWindow::runScript(bool simulate)
{
	_scriptRunning = true;

	if(simulate)
	{
		Q_EMIT scriptOutputAvailable(tr("---------[SCRIPT SIMULATION STARTED]----------\n"));
	}
	else
	{
		Q_EMIT scriptOutputAvailable(tr("---------------[SCRIPT STARTED]---------------\n"));
	}

	ASError exception;

	function<void(string)> callback = std::bind(&ASMainWindow::handleScriptOutput, this, std::placeholders::_1);

	if(!_ase->runScript(_editor->text().toStdString(), simulate, this, _iv, &exception, callback))
	{
		Q_EMIT scriptOutputAvailable(QString::fromStdString(exception.message));
		Q_EMIT highlightErrorSignal(exception.linenumber);
		if(simulate)
		{
			Q_EMIT scriptOutputAvailable(tr("---------[SCRIPT SIMULATION ABORTED]----------\n\n"));
		}
		else
		{
			Q_EMIT scriptOutputAvailable(tr("---------------[SCRIPT ABORTED]---------------\n\n"));
		}

		if(landOnError)
		{
			_ase->ardrone()->drone_land();
		}
	}
	else
	{
		if(simulate)
		{
			Q_EMIT scriptOutputAvailable(tr("---------[SCRIPT SIMULATION STOPPED]----------\n\n"));
		}
		else
		{
			Q_EMIT scriptOutputAvailable(tr("---------------[SCRIPT STOPPED]---------------\n\n"));
		}
	}

	Q_EMIT scriptStoppedSignal();

	_scriptRunning = false;
}

void ASMainWindow::handleScriptOutput(string output)
{
	Q_EMIT scriptOutputAvailable(QString::fromStdString(output));
}

void ASMainWindow::updateOutputTextWidget(QString output)
{
	tabs->setCurrentIndex(1); // Switch to output tab
	_output->moveCursor(QTextCursor::End);
	_output->insertPlainText(output);
}

void ASMainWindow::scriptStopped()
{
	abort->setEnabled(false);
	abort_m->setEnabled(false);

	sim->setEnabled(true);
	sim_m->setEnabled(true);
	exec->setEnabled(true);
	exec_m->setEnabled(true);
}

void ASMainWindow::setSimulatedBoolInput(string text)
{
	if(QMessageBox::question(this, tr("AutoScript Simulation"), QString::fromStdString(text), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
		_output->append(tr("[SIMULATION] ").append(QString::fromStdString(text)).append(" True\n"));
		_simBoolInput = true;
	}
	else
	{
		_output->append(tr("[SIMULATION] ").append(QString::fromStdString(text)).append(" False\n"));
		_simBoolInput = false;
	}
}

void ASMainWindow::setSimulatedFloatInput(string text, string unit)
{
	bool ok = false;
	double input = 0.0;

	while(!ok)
	{
		input = QInputDialog::getDouble(this, tr("AutoScript Simulation"), QString::fromStdString(text).append(" (in ").append(QString::fromStdString(unit)).append(")"), 0.0, -9999999999.0, 9999999999.0, 3, &ok);
	}

	_output->append(tr("[SIMULATION] ").append(QString::fromStdString(text)).append(": %1 \n").arg(input));

	_simFloatInput = input;
}

void ASMainWindow::printAction(string action)
{
	Q_EMIT scriptOutputAvailable(QString(tr("[SIMULATION] ")).append(QString::fromStdString(action).append("\n")));
}

bool ASMainWindow::getSimulatedBoolInput(string text)
{
	Q_EMIT simulatedBoolInputSignal(text); // Blocks until there's some input
	return _simBoolInput;
}

float ASMainWindow::getSimulatedFloatInput(string text, string unit)
{
	Q_EMIT simulatedFloatInputSignal(text, unit); // Blocks until there's some input
	return _simFloatInput;
}

void ASMainWindow::cursorChanged(int line, int index)
{
	currentLine = line;
}

void ASMainWindow::showDocs()
{
	tabs->setCurrentIndex(2);

	// Try to open the documentation for the function in the current line, if it is an AutoScript-specific one
	if(currentLine >= 0)
	{
		string lineText = _editor->text(currentLine).toStdString();
		vector<string> functions = _ase->getAvailableFunctions();
		string foundFunction;

		for(unsigned int i = 0; i < functions.size(); i++)
		{
			string currentFunction = functions[i];
			// Remove parameter spec
			currentFunction = currentFunction.substr(0, currentFunction.find("("));

			if(lineText.find(currentFunction) != string::npos)
			{
				foundFunction = currentFunction;
			}
		}

		if(!foundFunction.empty())
		{
			// Found an AutoScript function in the current line
			showDocForFunc(foundFunction);
		}
	}
}

void ASMainWindow::showDocForFunc(string func)
{
	QUrl functionDoc = QUrl::fromLocalFile(QString::fromStdString(AutoFlight::getProgramDirectory()).append("docs/AutoScript/functions.html"));
	functionDoc.setFragment(QString::fromStdString(func));

	_docs->load(functionDoc);
}

void ASMainWindow::toggleLandOnError(bool activate)
{
	landOnError = activate;
}

void ASMainWindow::closeEvent(QCloseEvent *event)
{
	if(documentEdited)
	{
		int result = QMessageBox::question(this, tr("Unsaved changes"), tr("The current script has unsaved changes that will be lost when exiting. Do you really want to exit?"), QMessageBox::Save, QMessageBox::Ok, QMessageBox::Cancel);

		if(result == QMessageBox::Cancel)
		{
			event->ignore();
		}
		else if(result == QMessageBox::Save)
		{
			saveFile();
			event->accept();
		}
		else if(result == QMessageBox::Ok)
		{
			event->accept();
		}
	}
}
