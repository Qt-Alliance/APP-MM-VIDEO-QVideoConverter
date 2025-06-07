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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

/**
@brief The MainWindow class provides the main window for the application.

	@author Oliver Becker <der.ole.becker@gmail.com>
*/
class MainWindow : public QWidget
{
		Q_OBJECT

	private:
		enum InputType
		{
			File	= 0x00,
			Dvd		= 0x01
		};

		enum OutputType
		{
			DivX	= 0x00,
			H264	= 0x01
		};

	public:
		/**
		 *   @brief Constructs an main window.
		 * @param parent Parent widget.
		 */
		MainWindow ( QWidget *parent = 0 );

		/**
		 *   @brief Destroys the main window.
		 */
		~MainWindow();

		QString inputFilename() const;

		QString outputFilename() const;

		void setInputType ( const InputType& theValue );


		InputType inputType() const;
		
		void setInputChapter ( const int& theValue );
	

		int inputChapter() const;
		
		void setOutputType ( const OutputType& theValue );

		OutputType outputType() const;

		bool getPictureArea() const;

		int targetSize() const;

		int targetFramerate() const;

		QSize targetResolution() const;

		int targetBitrate() const;
		
		bool normalizeAudio() const;

	signals:
		/**
		 *   @brief The convertion finished successfully.
		 */
		void finished();

		/**
		 *   @brief The convertion failed.
		 */
		void failed ( const QString& msg );

	public slots:
		void showErrorMessage ( const QString& msg ) const;

		void showSuccessMessage() const;

		/**
		 *   @brief Starts the convertion.
		 * This function is non-blocking. It will return immediately.
		 * If the convertion is finished, either finished() or failed() signals will be emitted.
		 */
		void start();

		/**
		 *   @brief Stops the convertion.
		 * This function is non-blocking.
		 */
		void stop();
		
	protected:
		virtual void closeEvent ( QCloseEvent * event );

	private:
		/**
		 *   @brief Initializes all values.
		 */
		void init();

		/**
		 *   @brief Initializes all gui elements.
		 */
		void initGui();
		
		/**
		 *   @brief Detects the crop area.
		 * @return Returns the area to crop.
		 */
		QRect	detectCropArea();

	private slots:
		void processFinished ( int returnCode, QProcess::ExitStatus exitStatus );

		void showDebugOutput ( const QString& string );

		void parseProcessOutput();
		
		void setInputType(const QString& type);
		
		void setInputFilename ( const QString& theValue );
		
		void setOutputFilename ( const QString& theValue );
		
		void setOutputType(const QString& type);
		
		void launchInputFileDialog();
		
		void launchInputFolderDialog();
		
		void launchOutputFileDialog();
		
		void setGetPictureArea ( bool theValue );
		
		void setTargetBitrateEnabled(bool enabled);
		
		void setTargetBitrate ( const int& theValue );
		
		void setTargetSizeEnabled(bool enabled);
		
		void setTargetSize ( const int& theValue );
		
		void setTargetFramerateEnabled(bool enabled);
		
		void setTargetFramerate ( const int& theValue );
		
		void setTargetResolutionEnabled(bool enabled);
		
		void setTargetResolution ( const QSize& theValue );
		
		void setTargetResolutionWidth(const int& value);
		
		void setTargetResolutionHeight(const int& value);
		
		void setNormalizeAudio ( bool theValue );
		
		void setStatus(const QString& msg);

	private:
		/* Options */

		/// Input filename
		QString			m_inputFilename;
		/// Input (DVD) chapter
		int			m_inputChapter;
		/// Output filename
		QString			m_outputFilename;

		/// Input type
		InputType		m_inputType;
		/// Output type
		OutputType	m_outputType;

		/// Get picture area?
		bool				m_getPictureArea;
		/// Normalize audio?
		bool				m_normalizeAudio;
		/// User stopped process?
		bool				m_userInterrupt;
		/// Convertion running?
		bool				m_running;
		
		// Bug message 1 printed?
		bool				m_bugMsg1Printed;

		/// Target size
		int					m_targetSize;
		/// Target framerate
		int					m_targetFramerate;
		/// Target resolution
		QSize				m_targetResolution;
		/// Target bitrate
		int					m_targetBitrate;
		
		/// Aspect ratio
		float				m_aspectRatioWidth;
		float				m_aspectRatioHeight;


		/* Variables */

		QProcess								m_converterProcess;
		QHash<QString, QString>	m_processParams;


		/* GUI elements */

		/// Main layout
		QGridLayout						* m_mainLayout;
		
		/// Tab widget: Files | Options
		QTabWidget						*	m_tabMain;
		
		/// Layout: Files		
		QGridLayout						*	m_layoutFiles;
		/// Group: Input
		QGroupBox							*	m_groupInput;
		/// Group-layout: Input
		QGridLayout						*	m_groupLayoutInput;
		/// Input file: label
		QLabel								*	m_labelInputFile;
		/// Input file: edit
		QLineEdit							*	m_editInputFile;
		/// File dialog launcher
		QPushButton						* m_buttonChooseInputFile;
		
		/// Input type: label
		QLabel								*	m_labelInputType;
		/// Input type: edit
		QComboBox							*	m_editInputType;
		
		/// Input chapter: label
		QLabel								*	m_labelInputChapter;
		/// Input chapter: edit
		QSpinBox							*	m_editInputChapter;
		
		/// Group: Output
		QGroupBox							*	m_groupOutput;
		/// Group-layout: Output
		QGridLayout						*	m_groupLayoutOutput;
		/// Output file: label
		QLabel								*	m_labelOutputFile;
		/// Output file: edit
		QLineEdit							*	m_editOutputFile;
		/// File dialog launcher
		QPushButton						* m_buttonChooseOutputFile;
		/// Output type: label
		QLabel								*	m_labelOutputType;
		/// Output type: edit
		QComboBox							*	m_editOutputType;
		/// Output language: label
		QLabel								* m_labelOutputLanguage;
		/// Output language: combo
		QComboBox							* m_editOutputLanguage;
		
		
		/// Layout: options
		QGridLayout						*	m_layoutOptions;
		
		/// Option: Detect real video area
		QCheckBox							* m_checkDetectArea;
		/// Option: Normalize audio
		QCheckBox							* m_checkNormalizeAudio;
		/// Option: Bitrate
		QCheckBox							*	m_checkTargetBitrate;
		/// Value: Bitrate
		QSpinBox							*	m_editTargetBitrate;
		/// Option: Size
		QCheckBox							*	m_checkTargetSize;
		/// Value: Size
		QSpinBox							*	m_editTargetSize;
		/// Option: Framerate
		QCheckBox							*	m_checkTargetFramerate;
		/// Value: Framerate
		QSpinBox							*	m_editTargetFramerate;
		/// Option: Resolution
		QCheckBox							*	m_checkTargetResolution;
		/// Value: Resolution
		QSpinBox							*	m_editTargetResolutionHeight;
		QSpinBox							*	m_editTargetResolutionWidth;
		
		
		/// Label: status
		QLabel								*	m_labelStatus;
		/// Progressbar
		QProgressBar					* m_progress;
		
		/// Progress: FPS (label)
		QLabel								* m_progressFpsLabel;
		/// Progress: FPS (value)
		QLabel								* m_progressFpsValue;
		
		/// Progress: Trem (label)
		QLabel								* m_progressTremLabel;
		/// Progress: Trem (value)
		QLabel								* m_progressTremValue;
		
		/// Start/Stop button
		QPushButton						*	m_buttonStartStop;
#ifdef DEBUG
		/// Debug output
		QTextEdit							*	m_debugOutput;
#endif
		
		/// List of supported output languages
		QHash<QString, QString>	m_languages;
};

#endif
