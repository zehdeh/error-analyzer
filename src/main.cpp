#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDesktopWidget>
#include <QVTKWidget.h>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkVolume.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkObjectFactory.h>
#include <vtkDataReader.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkStructuredPoints.h>

class MyDataSource : public vtkDataReader {
};

class MainWindow : public QMainWindow {
public:
	MainWindow() {
		QSize desktopSize = QDesktopWidget().availableGeometry(this).size();
		resize(desktopSize * 0.7);
		int x = (desktopSize.width() / 2) - (desktopSize.width()*0.7 / 2);
		int y = (desktopSize.height() / 2) - (desktopSize.height()*0.7 / 2);
		move(x,y);

		QVTKWidget* qvtk = new QVTKWidget(this);

		//vtkSmartPointer<vtkDataReader> grid = vtkSmartPointer<vtkDataReader>::New();

		//vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
		//compositeFunction->SetCompositeMethod(0);

		//vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
		//vtkSmartPointer<vtkVolumeRayCastMapper> volumeMapper = vtkSmartPointer<vtkVolumeRayCastMapper>::New();
		//volumeMapper->SetInputConnection(grid->GetOutputPort());
		//volumeMapper->SetVolumeRayCastFunction(compositeFunction);

		//vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
		//volume->SetProperty(volumeProperty);
		//volume->SetMapper(volumeMapper);

		//volumeMapper->SetBlendModeToMaximumIntensity();

		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

		//renderer->AddVolume(volume);
		renderer->ResetCamera();
		qvtk->GetRenderWindow()->AddRenderer(renderer);

		//qvtk->GetRenderWindow()->Render();

		this->setCentralWidget(qvtk);

		QDockWidget* dockWidget = new QDockWidget(tr("Dock Widget"), this);
		dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

		this->addDockWidget(Qt::LeftDockWidgetArea, dockWidget);
	}
};

int main(int argc, char** argv) {
	QApplication app(argc, argv);

	//QLabel *ql = new QLabel("<h2>Hello <font color='red'>World</font></h2>");
	//ql->show();
	MainWindow mainWindow;
	mainWindow.show();

	app.exec();
}
