
// A simple VTK volume rendering example
//
// Derek Magee, University of Leeds, UK

#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkStructuredPoints.h"
#include "vtkDataReader.h"

#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include <stdio.h>

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>

#include "vtkDataArray.h"
#include "vtkErrorCode.h"
#include "vtkFieldData.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"

#include <vtkUnsignedCharArray.h>
#include <vtkCellData.h>

#include <iostream>
using namespace std;


//
// First define source class + methods
//

//
// vtkExampleDataSource class
//
// Based on vtk souurce vtkStructuredPointsReader with new Execute() method

#include "vtkDataReader.h"

class vtkStructuredPoints;

class VTK_IO_EXPORT vtkExampleDataSource : public vtkDataReader
{
public:
    static vtkExampleDataSource *New();
    vtkTypeRevisionMacro(vtkExampleDataSource,vtkDataReader);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    // Set/Get the output of this reader.
    void SetOutput(vtkStructuredPoints *output);
    vtkStructuredPoints *GetOutput(int idx)
    {return (vtkStructuredPoints *) this->vtkSource::GetOutput(idx); };
    vtkStructuredPoints *GetOutput();

protected:
    vtkExampleDataSource();
    ~vtkExampleDataSource();

    void Execute();

    // Default method performs Update to get information.  Not all the old
    // structured points sources compute information
    void ExecuteInformation();

private:
    vtkExampleDataSource(const vtkExampleDataSource&);  // Not implemented.
    void operator=(const vtkExampleDataSource&);  // Not implemented.
};

vtkCxxRevisionMacro(vtkExampleDataSource, "$Revision: 1.00 $");
vtkStandardNewMacro(vtkExampleDataSource);

vtkExampleDataSource::vtkExampleDataSource()
{
    this->SetOutput(vtkStructuredPoints::New());
    // Releasing data for pipeline parallism.
    // Filters will know it is empty. 
    this->Outputs[0]->ReleaseData();
    this->Outputs[0]->Delete();
}

vtkExampleDataSource::~vtkExampleDataSource()
{
}

//----------------------------------------------------------------------------
void vtkExampleDataSource::SetOutput(vtkStructuredPoints *output)
{
    this->vtkSource::SetNthOutput(0, output);
}

//----------------------------------------------------------------------------
vtkStructuredPoints *vtkExampleDataSource::GetOutput()
{
    if (this->NumberOfOutputs < 1)
    {
        return NULL;
    }

    return (vtkStructuredPoints *)(this->Outputs[0]);
}


//----------------------------------------------------------------------------
// Default method performs Update to get information.  Not all the old
// structured points sources compute information
void vtkExampleDataSource::ExecuteInformation()
{
    this->SetErrorCode( vtkErrorCode::NoError );
    vtkStructuredPoints *output = this->GetOutput();
    vtkDataArray *scalars;
    int saveRequestFlag;

    // Now here is a problem.
    // Update Extent needs to be set incase the RequestExactExtent flag is on.
    // Bypass to superclasses update.  
    // Well that solution no longer works because the superclass now handles
    // the RequestForExactExtent flag.  Lets try disabling the flag temporarily.
    saveRequestFlag = output->GetRequestExactExtent();
    output->SetRequestExactExtent(0);
    output->vtkDataObject::UpdateData();
    output->SetRequestExactExtent(saveRequestFlag);

    scalars = output->GetPointData()->GetScalars();
    if (scalars)
    {
        output->SetScalarType(scalars->GetDataType());
        output->SetNumberOfScalarComponents(scalars->GetNumberOfComponents());
    }

    output->SetWholeExtent(output->GetExtent());
}

void vtkExampleDataSource::Execute()
/**
* Method modified/replaced by DRM
*/
{
    this->SetErrorCode( vtkErrorCode::NoError );
    int dim[3] = {100,100,100};
    float ar[3] = {1,1,1} ;
    float origin[3] = {50,50,50} ;

    vtkStructuredPoints *output = this->GetOutput();

    // ImageSource superclass does not do this.
    output->ReleaseData();

    // Set data dimensions, spacing and origin
    output->SetDimensions(dim); 
    output->SetSpacing(ar);
    output->SetOrigin(origin); 

    vtkDataSet *ds = output ;

    vtkDataSetAttributes *a = ds->GetPointData() ;

    // Data may be vtkUnsignedCharArray or vtkUnsignedShortArray
    vtkUnsignedCharArray *scalars = vtkUnsignedCharArray::New();

    // Create some data (a sphere with a 10 unit thick plane through)
    unsigned int cnt=0 ;
    float dx, dy, dz, r ;
    for(int cnt0=0 ; cnt0<100 ; cnt0++){
        for(int cnt1=0 ; cnt1<100 ; cnt1++){
            for(int cnt2=0 ; cnt2<100 ; cnt2++, cnt++){
                dx = (float)cnt0 - 50 ;
                dy = (float)cnt1 - 50 ;
                dz = (float)cnt2 - 50 ;
                r = sqrt(dx*dx + dy*dy+dz*dz) ;
                if(r < 30 || fabs(dx)<5) scalars->InsertValue(cnt, 128) ;
                else       scalars->InsertValue(cnt, 0) ;
            }
        }
    }

    a->SetScalars(scalars) ;
}

void vtkExampleDataSource::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}


int main( int argc, char *argv[] )
{
    int shade=0, compos=0, interp=0;

    //
    // Create the Renderer and assign actors to it. A renderer is like a
    // viewport. It is part or all of a window on the screen and it is
    // responsible for drawing the actors it has.  We also set the background
    // color here
    //
    vtkRenderer *ren1= vtkRenderer::New();

    //
    // Create the render window which will show up on the screen
    // We put our renderer into the render window using AddRenderer. We also
    // set the size to be 300 pixels by 300
    //
    vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer( ren1 );
    renWin->SetSize( 300, 300 );

    // Make interactive
    vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

    // Create some synthetic data
    vtkExampleDataSource *reader = vtkExampleDataSource::New();

#if 1
    // Standard transfer functions
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(0,   0.000);
    opacityTransferFunction->AddPoint(128, 0.02);
    opacityTransferFunction->AddPoint(255, 0.2);

    vtkColorTransferFunction *colorTransferFunction = 
        vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(      0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(    128.0, 1.0, 1.0, 1.0);
    colorTransferFunction->AddRGBPoint(    255.0, 1.0, 1.0, 1.0);
#else
    // Some colourful transfer functions
    vtkPiecewiseFunction *opacityTransferFunction = vtkPiecewiseFunction::New();
    opacityTransferFunction->AddPoint(0,   0.000);
    opacityTransferFunction->AddPoint(64, 0.0);
    opacityTransferFunction->AddPoint(255, 0.2);

    vtkColorTransferFunction *colorTransferFunction = 
        vtkColorTransferFunction::New();
    colorTransferFunction->AddRGBPoint(      0.0, 0.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(      64.0, 1.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(      128.0, 0.0, 0.0, 1.0);
    colorTransferFunction->AddRGBPoint(    192, 0, 1.0, 0);
    colorTransferFunction->AddRGBPoint(    255.0, 0, 0.2, 0);
#endif


    vtkVolumeProperty *volumeProperty = vtkVolumeProperty::New();
    volumeProperty->SetColor(colorTransferFunction);
    volumeProperty->SetScalarOpacity(opacityTransferFunction);
    volumeProperty->SetInterpolationType(interp);
    volumeProperty->SetShade(shade);

    vtkVolumeRayCastCompositeFunction *compositeFunction = 
        vtkVolumeRayCastCompositeFunction::New();
    compositeFunction->SetCompositeMethod(compos);


    vtkVolumeRayCastMapper *volumeMapper = vtkVolumeRayCastMapper::New();
    volumeMapper->SetVolumeRayCastFunction(compositeFunction);
    volumeMapper->SetInput(reader->GetOutput());

    vtkVolume *volume = vtkVolume::New();
    volume->SetMapper(volumeMapper);
    volume->SetProperty(volumeProperty);

    ren1->AddVolume(volume);
    renWin->Render();

    // Start interactive control
    iren->Start();

    cerr << "Bye now" << endl ;

    //
    // Free up any objects we created
    //

    ren1->Delete();

    return 0;
}


