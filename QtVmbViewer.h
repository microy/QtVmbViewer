#ifndef QTVMBVIEWER_H
#define QTVMBVIEWER_H

// Qt dependencies
#include <QWidget>
#include <QLabel>
#include <QSlider>
// Local dependency
#include "VmbCamera.h"

// Qt widget to display the images from an Allied Vision camera through the Vimba SDK
class QtVmbViewer : public QWidget {
	// Macro to use Qt signals and slots
	Q_OBJECT
	// Public members
	public :
		// Constructor
		QtVmbViewer( QWidget *parent = 0 );
		// Destructor
		~QtVmbViewer();
	// Qt slots
	public slots :
		// Slot to get the image from the camera and update the widget
		void UpdateImage();
		// Slot to update the camera exposure
		void SetExposure();
	// Private members
	private :
		// Label to display the camera image
		QLabel* label;
		// Slider to set the camera exposure time
		QSlider* slider;
		// Allied Vision camera
		VmbCamera* camera;
};

#endif // QTVMBVIEWER_H
