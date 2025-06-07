/***************************************************************************
 *   Copyright (C) 2008 by Oliver Becker   *
 *   der.ole.becker@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mainwindow.h"
#include "tools.h"

MainWindow::MainWindow ( QWidget *parent )
		: QWidget ( parent )
{
	m_languages["en"] = tr("English", "Output language");
	m_languages["de"] = tr("German", "Output language");
	
	init();
}


MainWindow::~MainWindow()
{
	QSettings set;
	set.setValue("MainWindow.geometry", saveGeometry());
}


void MainWindow::init()
{
	// Set title of window
	setWindowTitle ( "QVideoConverter" );
	setWindowIcon(QIcon(":/icons/logo.png"));

	// Default: Get picture area
	m_getPictureArea		= true;
	// Default: normalize audio
	m_normalizeAudio		= true;
	// Default: No target size
	m_targetSize				= 0;
	// Default: No target framerate
	m_targetFramerate		= 0;
	// Default: No target resolution
	m_targetResolution	= QSize ( 0, 0 );
	// Default: No target bitrate
	m_targetBitrate			= 1300;
	
	// Aspect ratio
	m_aspectRatioWidth	= 0.0f;
	m_aspectRatioHeight	= 0.0f;

	// Input type
	m_inputType					= MainWindow::File;
	// Input chapter
	m_inputChapter			= 1;
	// Output type
	m_outputType				= MainWindow::DivX;

	m_userInterrupt			= false;
	m_running						= false;

	m_bugMsg1Printed		= false;

	// Connections

	connect ( &m_converterProcess, SIGNAL ( finished ( int, QProcess::ExitStatus ) ), SLOT ( processFinished ( int, QProcess::ExitStatus ) ) );

	connect ( this, SIGNAL ( finished() ), SLOT ( showSuccessMessage() ) );
	connect ( this, SIGNAL ( failed ( const QString& ) ), SLOT ( showErrorMessage ( const QString& ) ) );

	connect ( &m_converterProcess, SIGNAL ( readyReadStandardOutput() ), SLOT ( parseProcessOutput() ) );

	// Init gui
	initGui();
}


void MainWindow::initGui()
{	
	m_mainLayout				= new QGridLayout ( this );
	
	m_tabMain						= new QTabWidget();
	m_mainLayout->addWidget(m_tabMain,0,0,1,3);
	
	
	QWidget	*	widget		= new QWidget();
	m_tabMain->addTab(widget, QIcon(":/icons/files.png"), tr("Files", "Tab Page: Files"));
	m_layoutFiles				= new QGridLayout(widget);
	
	m_groupInput				= new QGroupBox(tr("Input", "Groupbox: Input"));
	m_groupLayoutInput	= new QGridLayout();
	m_groupInput->setLayout(m_groupLayoutInput);
	m_layoutFiles->addWidget(m_groupInput,0,0,1,3);
	
	m_labelInputType		= new QLabel ( tr ( "Type:", "Label: Input type" ) );
	m_groupLayoutInput->addWidget ( m_labelInputType, 0,0 );
	m_editInputType			= new QComboBox();
	m_editInputType->addItem ( QIcon(":/icons/file.png"), tr ( "File", "Input type" ) );
	m_editInputType->addItem ( QIcon(":/icons/dvd.png"), tr ( "DVD", "Input type" ) );
	m_editInputType->setToolTip(tr("Type of input", "Tooltip: input type"));
	m_groupLayoutInput->addWidget ( m_editInputType, 0, 1 );
	connect ( m_editInputType, SIGNAL ( currentIndexChanged ( const QString& ) ), SLOT ( setInputType ( const QString& ) ) );

	m_labelInputFile		= new QLabel ( tr ( "File:", "Label: Input file" ) );
	m_groupLayoutInput->addWidget ( m_labelInputFile, 1,0 );
	m_editInputFile			= new QLineEdit();
	m_editInputFile->setToolTip(tr("File to read from", "Tooltip: input file"));
	m_groupLayoutInput->addWidget ( m_editInputFile, 1, 1,1,2 );
	connect ( m_editInputFile, SIGNAL ( textChanged ( const QString& ) ), SLOT ( setInputFilename ( const QString& ) ) );
	m_buttonChooseInputFile	= new QPushButton ( QIcon ( ":/icons/folder.png" ),"" );
	m_buttonChooseInputFile->setToolTip(tr("Select input file", "Tooltip: input file selector"));
	m_buttonChooseInputFile->adjustSize();
	m_buttonChooseInputFile->setFixedSize(m_buttonChooseInputFile->size());
	m_groupLayoutInput->addWidget ( m_buttonChooseInputFile, 1, 3 );
	connect ( m_buttonChooseInputFile, SIGNAL ( clicked() ), SLOT ( launchInputFileDialog() ) );
	
	m_labelInputChapter	= new QLabel ( tr ( "Chapter:", "Label: Input chapter" ) );
	m_groupLayoutInput->addWidget ( m_labelInputChapter, 3,0 );
	m_editInputChapter	= new QSpinBox();
	m_editInputChapter->setMinimum ( 1 );
	m_editInputChapter->setMaximum ( 1000 );
	m_editInputChapter->setEnabled ( false );
	m_editInputChapter->setToolTip(tr("Chapter to convert", "Tooltip: input chapter"));
	m_groupLayoutInput->addWidget ( m_editInputChapter, 3, 1 );
	
	m_groupOutput				= new QGroupBox(tr("Output", "Groupbox: Output"));
	m_groupLayoutOutput	= new QGridLayout();
	m_groupOutput->setLayout(m_groupLayoutOutput);
	m_layoutFiles->addWidget(m_groupOutput,1,0,1,3);

	m_labelOutputFile		= new QLabel ( tr ( "File:", "Label: Output file" ) );
	m_groupLayoutOutput->addWidget ( m_labelOutputFile, 0,0 );
	m_editOutputFile		= new QLineEdit();
	m_editOutputFile->setToolTip(tr("File to write to", "Tooltip: output file"));
	m_groupLayoutOutput->addWidget ( m_editOutputFile, 0, 1,1,2  );
	connect ( m_editOutputFile, SIGNAL ( textChanged ( const QString& ) ), SLOT ( setOutputFilename ( const QString& ) ) );
	m_buttonChooseOutputFile	= new QPushButton ( QIcon ( ":/icons/folder.png" ),"" );
	m_buttonChooseOutputFile->setToolTip(tr("Select output file", "Tooltip: output file selector"));
	m_buttonChooseOutputFile->adjustSize();
	m_buttonChooseOutputFile->setFixedSize(m_buttonChooseInputFile->size());
	m_groupLayoutOutput->addWidget ( m_buttonChooseOutputFile, 0, 3 );
	connect ( m_buttonChooseOutputFile, SIGNAL ( clicked() ), SLOT ( launchOutputFileDialog() ) );
	
	m_labelOutputType		= new QLabel ( tr ( "Type:", "Label: Output type" ) );
	m_groupLayoutOutput->addWidget ( m_labelOutputType, 1,0 );
	m_editOutputType		= new QComboBox();
	m_editOutputType->setToolTip(tr("Type of output video", "Tooltip: output type"));
	m_editOutputType->addItem ( QIcon(":/icons/divx.png"), tr ( "DivX", "Output type" ) );
	//m_editOutputType->addItem ( tr ( "H264", "Output type" ) );
	m_groupLayoutOutput->addWidget ( m_editOutputType, 1, 1 );
	connect ( m_editOutputType, SIGNAL ( currentIndexChanged ( const QString& ) ), SLOT ( setOutputType ( const QString& ) ) );
	
	m_labelOutputLanguage		= new QLabel ( tr ( "Language:", "Label: Output language" ) );
	m_groupLayoutOutput->addWidget ( m_labelOutputLanguage, 2,0 );
	m_editOutputLanguage		= new QComboBox();
	m_editOutputLanguage->setToolTip(tr("Preffered language of output video", "Tooltip: output language"));
	QHashIterator<QString, QString> i(m_languages);
	while(i.hasNext())
	{
		i.next();
		m_editOutputLanguage->addItem ( i.value() );
	}
	m_editOutputLanguage->setEditable(false);
	m_editOutputLanguage->setCurrentIndex(m_editOutputLanguage->findText(tr("English", "Default selected language (Must be equal with the languages in \"Output language\"!)")));
	//m_editOutputType->addItem ( tr ( "H264", "Output type" ) );
	m_groupLayoutOutput->addWidget ( m_editOutputLanguage, 2, 1 );
	connect ( m_editOutputLanguage, SIGNAL ( currentIndexChanged ( const QString& ) ), SLOT ( setOutputType ( const QString& ) ) );
	
	
	
	widget		= new QWidget();
	m_tabMain->addTab(widget, QIcon(":/icons/options.png"), tr("Options", "Tab Page: Options"));
	m_layoutOptions			= new QGridLayout(widget);
	
	m_checkDetectArea		= new QCheckBox(tr("Detect real video area", "Checkbox"));
	m_checkDetectArea->setChecked(true);
	connect(m_checkDetectArea, SIGNAL(toggled(bool)), SLOT(setGetPictureArea(bool)));
	m_layoutOptions->addWidget(m_checkDetectArea, 0,0);
	
	m_checkNormalizeAudio		= new QCheckBox(tr("Normalize audio", "Checkbox"));
	m_checkNormalizeAudio->setChecked(true);
	connect(m_checkNormalizeAudio, SIGNAL(toggled(bool)), SLOT(setNormalizeAudio(bool)));
	m_layoutOptions->addWidget(m_checkNormalizeAudio, 1,0);
	
	m_checkTargetBitrate	= new QCheckBox(tr("Bitrate", "Checkbox"));
	m_checkTargetBitrate->setChecked(true);
	connect(m_checkTargetBitrate, SIGNAL(toggled(bool)), SLOT(setTargetBitrateEnabled(bool)));
	m_layoutOptions->addWidget(m_checkTargetBitrate, 2,0);

	m_editTargetBitrate	= new QSpinBox();
	m_editTargetBitrate->setAlignment(Qt::AlignRight);
	m_editTargetBitrate->setMinimum(1);
	m_editTargetBitrate->setMaximum(100000);
	m_editTargetBitrate->setValue(m_targetBitrate);
	connect(m_editTargetBitrate, SIGNAL(valueChanged(int)),SLOT(setTargetBitrate(int)));
	m_layoutOptions->addWidget(m_editTargetBitrate, 2,3);
	
	QLabel * tmpLabel = new QLabel("Kb");
	tmpLabel->adjustSize();
	tmpLabel->setFixedSize(tmpLabel->size());
	m_layoutOptions->addWidget(tmpLabel, 2,4);
	
	m_checkTargetSize	= new QCheckBox(tr("Size", "Checkbox"));
	m_checkTargetSize->setChecked(false);
	connect(m_checkTargetSize, SIGNAL(toggled(bool)), SLOT(setTargetSizeEnabled(bool)));
	m_layoutOptions->addWidget(m_checkTargetSize, 3,0);

	m_editTargetSize	= new QSpinBox();
	m_editTargetSize->setAlignment(Qt::AlignRight);
	m_editTargetSize->setMinimum(1);
	m_editTargetSize->setMaximum(100000);
	m_editTargetSize->setValue(700);
	m_editTargetSize->setEnabled(false);
	connect(m_editTargetSize, SIGNAL(valueChanged(int)),SLOT(setTargetSize(int)));
	m_layoutOptions->addWidget(m_editTargetSize, 3,3);
	
	tmpLabel = new QLabel("Mb");
	tmpLabel->adjustSize();
	tmpLabel->setFixedSize(tmpLabel->size());
	m_layoutOptions->addWidget(tmpLabel, 3,4);
	
	m_checkTargetFramerate	= new QCheckBox(tr("Framerate", "Checkbox"));
	m_checkTargetFramerate->setChecked(false);
	connect(m_checkTargetFramerate, SIGNAL(toggled(bool)), SLOT(setTargetFramerateEnabled(bool)));
	m_layoutOptions->addWidget(m_checkTargetFramerate, 4,0);

	m_editTargetFramerate	= new QSpinBox();
	m_editTargetFramerate->setAlignment(Qt::AlignRight);
	m_editTargetFramerate->setMinimum(1);
	m_editTargetFramerate->setMaximum(100000);
	m_editTargetFramerate->setValue(25);
	m_editTargetFramerate->setEnabled(false);
	connect(m_editTargetFramerate, SIGNAL(valueChanged(int)),SLOT(setTargetFramerate(int)));
	m_layoutOptions->addWidget(m_editTargetFramerate, 4,3);
	
	tmpLabel = new QLabel("FPS");
	tmpLabel->adjustSize();
	tmpLabel->setFixedSize(tmpLabel->size());
	m_layoutOptions->addWidget(tmpLabel, 4,4);

	m_checkTargetResolution	= new QCheckBox(tr("Resolution", "Checkbox"));
	m_checkTargetResolution->setChecked(false);
	connect(m_checkTargetResolution, SIGNAL(toggled(bool)), SLOT(setTargetResolutionEnabled(bool)));
	m_layoutOptions->addWidget(m_checkTargetResolution, 5,0);

	m_editTargetResolutionWidth	= new QSpinBox();
	m_editTargetResolutionWidth->setAlignment(Qt::AlignRight);
	m_editTargetResolutionWidth->setMinimum(1);
	m_editTargetResolutionWidth->setMaximum(100000);
	m_editTargetResolutionWidth->setValue(720);
	m_editTargetResolutionWidth->setEnabled(false);
	connect(m_editTargetResolutionWidth, SIGNAL(valueChanged(int)),SLOT(setTargetResolutionWidth(int)));
	m_layoutOptions->addWidget(m_editTargetResolutionWidth, 5,1);
	
	tmpLabel = new QLabel("x");
	tmpLabel->adjustSize();
	tmpLabel->setFixedSize(tmpLabel->size());
	m_layoutOptions->addWidget(tmpLabel, 5,2);
	
	m_editTargetResolutionHeight	= new QSpinBox();
	m_editTargetResolutionHeight->setAlignment(Qt::AlignRight);
	m_editTargetResolutionHeight->setMinimum(1);
	m_editTargetResolutionHeight->setMaximum(100000);
	m_editTargetResolutionHeight->setValue(576);
	m_editTargetResolutionHeight->setEnabled(false);
	connect(m_editTargetResolutionHeight, SIGNAL(valueChanged(int)),SLOT(setTargetResolutionHeight(int)));
	m_layoutOptions->addWidget(m_editTargetResolutionHeight, 5,3);
	
	tmpLabel = new QLabel("Px");
	tmpLabel->adjustSize();
	tmpLabel->setFixedSize(tmpLabel->size());
	m_layoutOptions->addWidget(tmpLabel, 5,4);
	
	

	m_buttonStartStop		= new QPushButton ( tr ( "Start", "Start/Stop button" ) );
	connect ( m_buttonStartStop, SIGNAL ( clicked() ), SLOT ( start() ) );
	m_mainLayout->addWidget ( m_buttonStartStop, 2,1 );
	
	m_labelStatus				= new QLabel("");
	m_labelStatus->setVisible(false);
	m_mainLayout->addWidget(m_labelStatus, 3,0,1,3);

	m_progress					= new QProgressBar();
	m_progress->setVisible(false);
	m_progress->setMinimum ( 0 );
	m_progress->setMaximum ( 100 );
	m_progress->setValue ( 0 );
	m_mainLayout->addWidget ( m_progress, 4,0, 1,3 );

	m_progressFpsLabel	= new QLabel ( tr ( "Frames per second:", "Label: Frames per second" ) );
	m_progressFpsLabel->setVisible(false);
	m_mainLayout->addWidget ( m_progressFpsLabel, 5,0 );
	m_progressFpsValue	= new QLabel ( tr ( "Unknown", "Frames per second default value" ) );
	m_progressFpsValue->setVisible(false);
	m_progressFpsValue->setAlignment ( Qt::AlignRight );
	m_mainLayout->addWidget ( m_progressFpsValue, 5,1,1,2 );

	m_progressTremLabel	= new QLabel ( tr ( "Time remaining:", "Label: Time remaining" ) );
	m_progressTremLabel->setVisible(false);
	m_mainLayout->addWidget ( m_progressTremLabel, 6,0 );
	m_progressTremValue	= new QLabel ( tr ( "Unknown", "Remaining time default value" ) );
	m_progressTremValue->setVisible(false);
	m_progressTremValue->setAlignment ( Qt::AlignRight );
	m_mainLayout->addWidget ( m_progressTremValue, 6,1,1,2 );


#ifdef DEBUG
	m_debugOutput				= new QTextEdit();
	m_debugOutput->setReadOnly ( true );
	m_mainLayout->addWidget ( m_debugOutput, 100,0,1,3 );
#endif
	
// 	adjustSize();
// 	setFixedHeight(height());
	
	// Load settings
	QSettings set;
	restoreGeometry(set.value("MainWindow.geometry").toByteArray());
}


QString MainWindow::inputFilename() const
{
	return m_inputFilename;
}


void MainWindow::setInputFilename ( const QString& theValue )
{
	m_inputFilename = theValue;
}


QString MainWindow::outputFilename() const
{
	return m_outputFilename;
}


void MainWindow::setOutputFilename ( const QString& theValue )
{
	m_outputFilename = theValue;
}


MainWindow::InputType MainWindow::inputType() const
{
	return m_inputType;
}


void MainWindow::setInputType ( const MainWindow::InputType& theValue )
{
	m_inputType = theValue;
}


int MainWindow::inputChapter() const
{
	return m_inputChapter;
}


void MainWindow::setInputChapter ( const int& theValue )
{
	m_inputChapter = theValue;
}


MainWindow::OutputType MainWindow::outputType() const
{
	return m_outputType;
}


void MainWindow::setOutputType ( const MainWindow::OutputType& theValue )
{
	m_outputType = theValue;
}


bool MainWindow::getPictureArea() const
{
	return m_getPictureArea;
}


void MainWindow::setGetPictureArea ( bool theValue )
{
	m_getPictureArea = theValue;
}


int MainWindow::targetSize() const
{
	return m_targetSize;
}


void MainWindow::setTargetSize ( const int& theValue )
{
	m_targetSize = theValue;
}


int MainWindow::targetFramerate() const
{
	return m_targetFramerate;
}


void MainWindow::setTargetFramerate ( const int& theValue )
{
	m_targetFramerate = theValue;
}


QSize MainWindow::targetResolution() const
{
	return m_targetResolution;
}


void MainWindow::setTargetResolution ( const QSize& theValue )
{
	m_targetResolution = theValue;
}


int MainWindow::targetBitrate() const
{
	return m_targetBitrate;
}


void MainWindow::setTargetBitrate ( const int& theValue )
{
	m_targetBitrate = theValue;
}


void MainWindow::start()
{
#ifdef DEBUG
	m_debugOutput->clear();
#endif

	/* Make parameters */
	
	QFileInfo info(m_inputFilename);
	if(!info.exists())
	{
		showErrorMessage ( tr ( "Please specify an valid input file!", "No input file specified" ) );
		return;
	}
	
	info.setFile(m_outputFilename);
	if(m_outputFilename.isEmpty())
	{
		showErrorMessage ( tr ( "Please specify an output file!", "No output file specified" ) );
		return;
	}
	
	m_labelStatus->setVisible(true);
	m_progress->setVisible(true);
	m_progressFpsLabel->setVisible(true);
	m_progressFpsValue->setVisible(true);
	m_progressTremLabel->setVisible(true);
	m_progressTremValue->setVisible(true);
// 	adjustSize();
// 	setFixedHeight(height());
	
// 	if(!info.isWritable())
// 	{
// 		showErrorMessage ( tr ( "You don't have permission to write the output file!", "No write permissions for output file" ) );
// 		return;
// 	}
	
	m_buttonStartStop->setEnabled(false);
	
	QStringList vfParams;
	
	bool autoscale = false;
	
	QRect crop;
	if(m_getPictureArea)
	{
		setStatus(tr("Detecting real video area ...", "Status message"));
		crop = detectCropArea();
		vfParams.append(QString("crop=%1:%2:%3:%4").arg(crop.width()).arg(crop.height()).arg(crop.x()).arg(crop.y()));
	}

	// Input type
	switch ( m_inputType )
	{
		case MainWindow::File:
		{
			m_processParams["input"] = m_inputFilename;
		}
		break;

		case MainWindow::Dvd:
		{
			m_processParams["input"] = QString ( "-dvd-device \"%1\" dvd://%2" ).arg ( m_inputFilename ).arg ( m_inputChapter );
		}
		break;
	}
	
	// Aspect ratio
	if((m_targetResolution.height() == 0 || m_targetResolution.width() == 0) && (m_aspectRatioWidth > 0.0f && m_aspectRatioHeight > 0.0f))
	{
		autoscale = true;
	}

	// Output type
	switch ( m_outputType )
	{
		case MainWindow::DivX:
		{
			QStringList divxopts;
			
			if(m_targetBitrate)
				divxopts.append(QString("bitrate=%1").arg(m_targetBitrate));
			else if(m_targetSize)
				divxopts.append(QString("bitrate=-%1").arg(m_targetSize*1024));

			m_processParams["output"] = QString ( "-ovc xvid -xvidencopts %1" ).arg ( divxopts.join ( "," ) );
		}
		break;

		default:
		{
			qDebug ( "Output type 0x%02X currently not handled!", m_outputType );
		}
		break;
	}
	
	if(m_targetResolution.height() > 0 && m_targetResolution.width() > 0)
		vfParams.append(QString("scale=%1:%2").arg(m_targetResolution.width()).arg(m_targetResolution.height()));
	else if(autoscale)
		vfParams.append("scale=0:0");
			
	if(m_targetFramerate)
		m_processParams["outputFps"] = QString("-ofps %1").arg(m_targetFramerate);
	
	if(m_normalizeAudio)
		m_processParams["outputNAudio"] = QString("-af volnorm=1");

	m_processParams["audioOutput"] = "-oac mp3lame";

	// Output file
	m_processParams["outputFile"] = QString ( "-o \"%1\"" ).arg ( m_outputFilename );
	
	// -vf
	if(vfParams.count() > 0)
		m_processParams["vf"] = QString("-vf %1").arg(vfParams.join(","));
	
	// Audio language
	QHashIterator<QString, QString> i(m_languages);
	while(i.hasNext())
	{
		i.next();
		if(i.value() == m_editOutputLanguage->currentText())
		{
			m_processParams["alang"] = QString("-alang %1").arg(i.key());
			break;
		}
	}

	const QString cmd = QString ( "mencoder %1" ).arg ( QStringList ( m_processParams.values() ).join ( " " ) );

	showDebugOutput ( cmd );

	m_buttonStartStop->disconnect ( SIGNAL ( clicked() ) );
	connect ( m_buttonStartStop, SIGNAL ( clicked() ), SLOT ( stop() ) );
	m_buttonStartStop->setText ( tr ( "Stop", "Start/Stop button" ) );
	m_buttonStartStop->setEnabled(true);
	m_running					= true;
	setStatus(tr("Converting video ...", "Status message"));
	m_converterProcess.start ( cmd );
}


void MainWindow::stop()
{
	m_buttonStartStop->disconnect ( SIGNAL ( clicked() ) );
	connect ( m_buttonStartStop, SIGNAL ( clicked() ), SLOT ( start() ) );
	m_buttonStartStop->setText ( tr ( "Start", "Start/Stop button" ) );

	m_userInterrupt = true;
	m_running				= false;
	m_converterProcess.close();
	
#ifndef DEBUG
	m_labelStatus->setVisible(false);
	m_progress->setVisible(false);
	m_progressFpsLabel->setVisible(false);
	m_progressFpsValue->setVisible(false);
	m_progressTremLabel->setVisible(false);
	m_progressTremValue->setVisible(false);
// 	adjustSize();
// 	setFixedHeight(height());
#endif
}


void MainWindow::processFinished ( int returnCode, QProcess::ExitStatus exitStatus )
{
	setStatus("");
	m_buttonStartStop->disconnect ( SIGNAL ( clicked() ) );
	connect ( m_buttonStartStop, SIGNAL ( clicked() ), SLOT ( start() ) );
	m_buttonStartStop->setText ( tr ( "Start", "Start/Stop button" ) );
	m_progress->setValue ( 0 );
	m_progressFpsValue->setText ( tr ( "Unknown", "Frames per second default value" ) );
	m_progressTremValue->setText ( tr ( "Unknown", "Remaining time default value" ) );
	setWindowTitle ( "QVideoConverter" );
	m_running = false;

	if ( exitStatus == QProcess::CrashExit )
	{
		if ( !m_userInterrupt )
			emit ( failed ( tr ( "Mencoder crashed unexpected", "Process: Mencoder crashed" ) ) );
	}
	else
	{
		if ( returnCode == 0 )
		{
			emit ( finished() );
		}
		else if ( !m_userInterrupt )
		{
			QString failMessage;
			emit ( failed ( tr ( "Mencoder failed!", "Process: Mencoder failed" ) ) );
		}
	}
	m_userInterrupt = false;
}


void MainWindow::showErrorMessage ( const QString & msg ) const
{
	QMessageBox msgBox ( QMessageBox::Critical, tr ( "Error", "Title of Error-Messagebox" ), msg );
	msgBox.exec();
}


void MainWindow::showSuccessMessage() const
{
	QMessageBox msgBox ( QMessageBox::Information, tr ( "Convertion successfull", "Title of Success-Messagebox" ), tr ( "The convertion was successfull!", "Message of Success-Messagebox" ) );
	msgBox.exec();
}

void MainWindow::showDebugOutput ( const QString & string )
{
#ifdef DEBUG
	m_debugOutput->append ( string );
#else
	Q_UNUSED(string);
#endif
}


void MainWindow::parseProcessOutput()
{
	QString pos,percentage,fps,fps10,trem,tremType,tsize, tmp = QString::fromLocal8Bit ( m_converterProcess.readAllStandardOutput().data() );

	if ( regCaps ( ".*Pos\\:[ \t]*([0-9\\.a-z]+)[ \t]*[0-9a-z]+[ \t]*\\([ \t]*([0-9]+)\\%\\)[ \t]*([0-9]+)\\.([0-9]+)fps[ \t]*Trem\\:[ \t]*([0-9]+)([a-z]+)[ \t]*([0-9a-z]+)[ \t]*.*",tmp,&pos,&percentage,&fps,&fps10,&trem,&tremType,&tsize ) )
	{
		m_progress->setValue ( percentage.toInt() );
		setWindowTitle ( QString("QVideoConverter (%1%)").arg(percentage.toInt()) );

		double fpsVal = fps.toInt() + fps10.toInt() /100.0f;
		if ( fpsVal == 0 ? m_progressFpsValue->text() != tr ( "Unknown", "Frames per second default value" ) : true )
			m_progressFpsValue->setText ( QString ( "%L1" ).arg ( fpsVal, 0, 'f', 2 ) );

		if ( trem.toInt() == 0 ? m_progressTremValue->text() != tr ( "Unknown", "Remaining time default value" ) : true )
		{
			if ( tremType == QString ( "min" ) )
			{
				int minutes	= trem.toInt();

				// Set weeks
				int weeks		= minutes/10800;
				minutes					= minutes%10800;

				// Set days
				int days		= minutes/1440;
				minutes					= minutes%1440;

				// Set hours
				int hours		= minutes/60;
				minutes					= minutes%60;

				QStringList texts;

				if ( weeks > 0 )
					texts.append ( tr ( "%n week(s)", "Remaining weeks", weeks ) );
				if ( days > 0 )
					texts.append ( tr ( "%n day(s)", "Remaining days", days ) );
				if ( hours > 0 )
					texts.append ( tr ( "%n hour(s)", "Remaining hours", hours ) );
				if ( minutes > 0 )
					texts.append ( tr ( "%n minute(s)", "Remaining minutes", minutes ) );

				m_progressTremValue->setText ( texts.join ( ", " ) );
			}
			else if ( !m_bugMsg1Printed )
			{
				qDebug ( "Please report following to <der.ole.becker@gmail.com>: \"tremType: %s\"\nThank you!", qPrintable ( tremType ) );
				m_bugMsg1Printed = true;
			}
		}
	}
#ifdef DEBUG
	showDebugOutput ( tmp );
#endif
}


void MainWindow::setInputType ( const QString & type )
{
	if ( type == QString ( "DVD" ) )
	{
		m_inputType = MainWindow::Dvd;
		m_editInputChapter->setEnabled ( true );
		disconnect(m_buttonChooseInputFile,SIGNAL(clicked()),0,0);
		connect(m_buttonChooseInputFile,SIGNAL(clicked()),SLOT(launchInputFolderDialog()));
		m_labelInputFile->setText ( tr ( "DVD folder:", "Label: DVD folder" ) );
		m_editInputFile->setToolTip(tr("VIDEO_TS folder of the dvd", "Tooltip: DVD folder"));
		m_buttonChooseInputFile->setToolTip(tr("Select input file", "Tooltip: input file selector"));
	}
	else
	{
		m_inputType = MainWindow::File;
		m_editInputChapter->setEnabled ( false );
		disconnect(m_buttonChooseInputFile,SIGNAL(clicked()),0,0);
		connect(m_buttonChooseInputFile,SIGNAL(clicked()),SLOT(launchInputFileDialog()));
		m_labelInputFile->setText ( tr ( "File:", "Label: Input file" ) );
		m_editInputFile->setToolTip(tr("File to read from", "Tooltip: input file"));
		m_buttonChooseInputFile->setToolTip(tr("Select input folder", "Tooltip: input folder selector"));
	}
}


void MainWindow::setOutputType(const QString & type)
{
	if ( type == QString ( "DivX" ) )
	{
		m_outputType = MainWindow::DivX;
	}
	else if(type == QString ( "H264" ))
	{
		m_outputType = MainWindow::H264;
	}
}


void MainWindow::closeEvent ( QCloseEvent * event )
{
	if ( m_running && !QMessageBox::question ( this, tr ( "Close confirmation", "Close confirmation title" ), tr ( "Do you realy want to abort the convertion?", "Close confirmation" ), QMessageBox::Yes | QMessageBox::No ) == QMessageBox::No )
	{
		event->ignore();
		return;
	}
	m_userInterrupt = true;
	m_converterProcess.close();
	event->accept();
}


void MainWindow::launchInputFileDialog()
{
	m_editInputFile->setText ( QFileDialog::getOpenFileName ( this, tr ( "Choose input file", "Input file selection dialog" ) ) );
}


void MainWindow::launchOutputFileDialog()
{
	m_editOutputFile->setText ( QFileDialog::getSaveFileName ( this, tr ( "Choose output file", "Output file selection dialog"), QString(), tr("Video files (*.avi *.mpg)", "Output file types" ) ) );
}


void MainWindow::setTargetBitrateEnabled(bool enabled)
{
	m_checkTargetSize->setChecked(!enabled);
	m_editTargetBitrate->setEnabled(enabled);
	
	if(enabled)
		m_targetBitrate	=	m_editTargetBitrate->value();
	else
		m_targetBitrate	= 0;
}


void MainWindow::setTargetSizeEnabled(bool enabled)
{
	m_checkTargetBitrate->setChecked(!enabled);
	m_editTargetSize->setEnabled(enabled);
	
	if(enabled)
		m_targetSize	=	m_editTargetSize->value();
	else
		m_targetSize	= 0;
}


void MainWindow::setTargetResolutionEnabled(bool enabled)
{
	m_editTargetResolutionWidth->setEnabled(enabled);
	m_editTargetResolutionHeight->setEnabled(enabled);
	
	if(enabled)
		m_targetResolution	=	QSize(m_editTargetResolutionWidth->value(), m_editTargetResolutionHeight->value());
	else
		m_targetResolution	= QSize(0, 0);
}


void MainWindow::setTargetFramerateEnabled(bool enabled)
{
	m_editTargetFramerate->setEnabled(enabled);
	
	if(enabled)
		m_targetFramerate	=	m_editTargetFramerate->value();
	else
		m_targetFramerate	= 0;
}


bool MainWindow::normalizeAudio() const
{
	return m_normalizeAudio;
}


void MainWindow::setNormalizeAudio ( bool theValue )
{
	m_normalizeAudio = theValue;
}


QRect MainWindow::detectCropArea()
{
	QProcess mplayer;
	
	int wait = 10;
	
	QString filename;
	if(m_inputType == MainWindow::Dvd)
		filename = QString("-dvd-device \"%1\" dvd://%2").arg(m_inputFilename).arg(m_inputChapter);
	else
		filename = QString("\"%1\"").arg(m_inputFilename);
	
	mplayer.start ( QString ( "mplayer -vf cropdetect=:2 %1 -ss 300 -endpos %2 -vo null -ao null" ).arg ( filename ).arg(wait) );
	
	QTime time;
	time.start();
	
	while(!mplayer.waitForFinished(500))
	{
		QApplication::processEvents();
		m_progress->setValue((int)(time.elapsed()/(wait*1000.0f)*100));
	}
	m_progress->setValue(0);
	
	QRect ret;

	if ( mplayer.exitCode() == 0 )
	{
		QStringList tmp = QString::fromLocal8Bit ( mplayer.readAllStandardOutput().data() ).replace ( "\r\n","\n" ).split ( "\n" );
			
		QString x,y,height,width,aspectW,aspectH;
		
		for ( int i = 0; i < tmp.count(); i++ )
		{
			if(regCaps(".*VO\\:\\s*\\[[a-zA-Z]+\\]\\s*\\S*\\s*\\=\\>\\s*(\\d+)x(\\d+).*", tmp[i], &aspectW, &aspectH))
			{
				m_aspectRatioWidth = aspectW.toFloat();
				m_aspectRatioHeight = aspectH.toFloat();
				
// 				printf("Aspect: %f x %f\n", m_aspectRatioWidth, m_aspectRatioHeight);
			}
// 			else
// 				printf("No match: %s\n", qPrintable(tmp[i]));
		}

		for ( int i = tmp.count()-1; i > 0; i-- )
		{
			if ( regCaps ( ".*\\(\\-vf crop\\=([0-9]+)\\:([0-9]+)\\:([0-9]+)\\:([0-9]+)\\).*",tmp[i],&width,&height,&x,&y ) )
			{
				ret = QRect(x.toInt(), y.toInt(), width.toInt(), height.toInt());
				return ret;
			}
		}
	}
	return ret;
}


void MainWindow::setStatus(const QString & msg)
{
	m_labelStatus->setText(msg);
}


void MainWindow::setTargetResolutionWidth(const int & value)
{
	m_targetResolution.setWidth(value);
}


void MainWindow::setTargetResolutionHeight(const int & value)
{
	m_targetResolution.setHeight(value);
}


void MainWindow::launchInputFolderDialog()
{
	QFileDialog dlg(this, tr ( "Choose VIDEO_TS folder", "Input folder selection dialog" ));
	
	dlg.setFileMode(QFileDialog::DirectoryOnly);
	dlg.exec();
	QStringList files = dlg.selectedFiles();
	
	if(files.count())
		m_editInputFile->setText(files[0]);
}
