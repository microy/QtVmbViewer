#include "QtVmbViewer.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>

// Qt widget to display the images from an Allied Vision camera
class TestWidget : public QWidget {
	// Public members
	public :
		// Constructor
		TestWidget( QWidget *parent = 0 ): QWidget( parent ) {
			// The camera viewer
			vmb_camera_viewer = new QtVmbViewer();
			vmb_camera_viewer->setScaledContents( true );
			// Widget layout
			QVBoxLayout* layout = new QVBoxLayout( this );
			layout->addWidget( vmb_camera_viewer );
			layout->setSizeConstraint( QLayout::SetFixedSize );
			// Open the camera
			vmb_camera_viewer->Open();
			// Start acquisition
			vmb_camera_viewer->StartCapture();
		}
		// Destructor
		~TestWidget() {
			// Stop acquisition
			vmb_camera_viewer->StopCapture();
			// Close the camera
			vmb_camera_viewer->Close();
		}
		// Label to display the camera image
		QtVmbViewer* vmb_camera_viewer;
};


int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	TestWidget w;
	w.show();
	return a.exec();
}
