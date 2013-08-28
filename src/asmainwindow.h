#ifndef ASMAINWINDOW_H
#define ASMAINWINDOW_H

#include <QMainWindow>
#include <QFont>
#include <QTextEdit>
#include <QAction>
#include <QtWebKitWidgets>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>
#include <string>
#include <QString>
#include <boost/thread.hpp>

#include "autoscript/asengine.h"
#include "autoscript/iscriptsimulationui.h"

class ASMainWindow : public QMainWindow, public IScriptSimulationUI
{
	Q_OBJECT

	public:
		explicit ASMainWindow(ASEngine *e, QWidget *parent = 0);
		~ASMainWindow();

		void handleScriptOutput(std::string output);

		void printAction(std::string action);
		bool getSimulatedBoolInput(std::string text);
		float getSimulatedFloatInput(std::string text, std::string unit);
	private Q_SLOTS:
		void newFile();
		void openFile();
		void saveFile();
		void saveFileAs();
		void textEdited();

		void startScript();
		void simulateScript();
		void abortScript();
		void toggleLandOnError(bool activate);

		void updateOutputTextWidget(QString output);
		void scriptStopped();

		void setSimulatedBoolInput(std::string text);  // Asks the user for a bool and stores it to _simBoolInput to be processed by getSimulatedBoolInput
		void setSimulatedFloatInput(std::string text, std::string unit);  // Asks the user for a float and stores it to _simFloatInput to be processed by getSimulatedFloatInput

		void highlightError(int lineNo);

		void cursorChanged(int line, int index);

		void showDocs();

		void closeEvent(QCloseEvent *e);
	private:
		void setWindowAttributes();
		void setTabName();
		void applyTheme(QsciLexerPython *lexer, QFont f);
		bool writeFile(std::string path, std::string content);
		void runScript(bool simulate);
		void deleteScriptThread();
		void showDocForFunc(std::string func);

		QsciScintilla *_editor = NULL;
		QTextEdit *_output = NULL;
		QWebView *_docs = NULL;
		QTabWidget *tabs = NULL;

		ASEngine *_ase = NULL;

		QAction *abort = NULL;
		QAction *abort_m = NULL;
		QAction *sim = NULL;
		QAction *sim_m = NULL;
		QAction *exec = NULL;
		QAction *exec_m = NULL;

		bool documentEdited = false;
		bool fileAlreadySavedAs = false;
		std::string fileSavedAs;

		boost::thread *scriptThread = NULL;
		bool _scriptRunning = false;

		bool _simBoolInput = false; // See setSimulatedBoolInput
		float _simFloatInput = 0.0f; // See setSimulatedFloatInput

		bool landOnError = false;

		int currentLine = -1;
	Q_SIGNALS:
		void scriptOutputAvailable(QString output);
		void scriptStoppedSignal();
		void simulatedBoolInputSignal(std::string text);
		void simulatedFloatInputSignal(std::string text, std::string unit);
		void highlightErrorSignal(int lineNo);
};

#endif
