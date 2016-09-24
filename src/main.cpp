#include <stdlib.h>

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
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkObjectFactory.h>
#include <vtkImageImport.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkImageGaussianSmooth.h>


class MainWindow : public QMainWindow {
public:
	MainWindow() {
		QSize desktopSize = QDesktopWidget().availableGeometry(this).size();
		resize(desktopSize * 0.7);
		int x = (desktopSize.width() / 2) - (desktopSize.width()*0.7 / 2);
		int y = (desktopSize.height() / 2) - (desktopSize.height()*0.7 / 2);
		move(x,y);

		QVTKWidget* qvtk = new QVTKWidget(this);

		vtkSmartPointer<vtkVolumeRayCastCompositeFunction> compositeFunction = vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
		//compositeFunction->SetCompositeMethod(0);

		unsigned int dimensionSize = 10;
		unsigned int dataSize = dimensionSize*dimensionSize*dimensionSize;
		unsigned char data[dataSize];
		for(unsigned int i = 0; i < dataSize; i++) {
			data[i] = rand() % 255;
		}

		vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();
		imageImport->SetWholeExtent(0, dimensionSize - 1, 0, dimensionSize - 1, 0, dimensionSize - 1);
		imageImport->SetDataExtentToWholeExtent();
		imageImport->SetImportVoidPointer(data);
		imageImport->CopyImportVoidPointer(data, dataSize*sizeof(unsigned char));
		imageImport->SetDataScalarTypeToUnsignedChar();
		imageImport->SetNumberOfScalarComponents(1);

		vtkSmartPointer<vtkPiecewiseFunction> alphaChannelFunc = vtkSmartPointer<vtkPiecewiseFunction>::New();
		alphaChannelFunc->AddPoint(0, 0);
		alphaChannelFunc->AddPoint(25, 0.03);
		alphaChannelFunc->AddPoint(50, 0.05);
		alphaChannelFunc->AddPoint(100, 0.1);
		alphaChannelFunc->AddPoint(130, 0.15);
		alphaChannelFunc->AddPoint(150, 0.155);
		alphaChannelFunc->AddPoint(255, 0.2);

		vtkSmartPointer<vtkColorTransferFunction> colorFunc = vtkSmartPointer<vtkColorTransferFunction>::New();
		colorFunc->AddRGBPoint(0, 0.0, 0.9, 0.0);
		colorFunc->AddRGBPoint(75, 1.0, 1.0, 0.0);
		colorFunc->AddRGBPoint(150, 1.0, 0.0, 0.0);
		colorFunc->AddRGBPoint(200, 1.0, 0.0, 1.0);
		colorFunc->AddRGBPoint(255, 0.0, 0.0, 1.0);
		//colorFunc->AddRGBPoint(100, 0.0, 1.0, 0.0);

		vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
		volumeProperty->SetColor(colorFunc);
		//volumeProperty->SetGradientOpacity(alphaChannelFunc);
		volumeProperty->SetScalarOpacity(alphaChannelFunc);
		volumeProperty->ShadeOn();
		//volumeProperty->SetInterpolationTypeToLinear();

		vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
		//volumeMapper->SetVolumeRayCastFunction(compositeFunction);
		volumeMapper->SetInputConnection(imageImport->GetOutputPort());

		vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
		volume->SetProperty(volumeProperty);
		volume->SetMapper(volumeMapper);

		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();

		qvtk->GetRenderWindow()->AddRenderer(renderer);
		renderer->AddVolume(volume);
		renderer->SetBackground(0,0,0);
		renderer->ResetCamera();

		qvtk->GetRenderWindow()->Render();

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
