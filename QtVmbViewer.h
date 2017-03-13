#ifndef QTVMBVIEWER_H
#define QTVMBVIEWER_H

// Vimba SDK dependency
#include <VimbaC.h>
// Qt dependency
#include <QImage>
#include <QLabel>

// Qt Label to display the images from an Allied Vision camera
class QtVmbViewer : public QLabel {

	Q_OBJECT

	// Public members
	public :
		// Constructor
		QtVmbViewer( QWidget* parent = 0 );
		// Destructor
		~QtVmbViewer();

		// Open the camera
		void Open();
		// Close the camera
		void Close();
		// Start the acquisition
		void StartCapture();
		// Stop the acquisition
		void StopCapture();

		// Function called by Vimba to receive the frame
		static void VMB_CALL FrameDoneCallback( const VmbHandle_t camera_handle, VmbFrame_t* frame_pointer );

		// Handle to the camera
		VmbHandle_t handle;
		// Image parameters
		VmbInt64_t width;
		VmbInt64_t height;
		VmbInt64_t payloadsize;
		// Frame buffer size
		static const int frame_buffer_size = 10;
		// Frame buffer
		VmbFrame_t* frame_buffer;
		// Current camera image
		QImage* image;

	// Qt signals
	signals :
		// Signal to tell a new image is received
		void ImageReady();

	// Qt slots
	public slots :
		// Slot to get the image from the camera and update the widget
		void UpdateImage();
};

#endif // QTVMBVIEWER_H
