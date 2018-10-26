/*************************************************************************
sparse: Efficient Computation of the Flow Map from Sparse Samples

Author: Samer S. Barakat

Copyright (c) 2010-2013, Purdue University

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**************************************************************************/
#include "VTKCannyEdgeDetector.h"

int VTKCannyEdges(string infile, double maxth, double minth)
{
	vtkSmartPointer<vtkPNGReader> imageIn =	vtkSmartPointer<vtkPNGReader>::New();
	imageIn->SetFileName(infile.c_str());
	imageIn->Update();
 
	vtkSmartPointer<vtkImageLuminance> il =	vtkSmartPointer<vtkImageLuminance>::New();
	il->SetInputConnection(imageIn->GetOutputPort());
 
	vtkSmartPointer<vtkImageCast> ic = vtkSmartPointer<vtkImageCast>::New();
	ic->SetOutputScalarTypeToFloat();
	ic->SetInputConnection(il->GetOutputPort());
 
	// Smooth the image
	vtkSmartPointer<vtkImageGaussianSmooth> gs = vtkSmartPointer<vtkImageGaussianSmooth>::New();
	gs->SetInputConnection(ic->GetOutputPort());
	gs->SetDimensionality(3);
	gs->SetRadiusFactors(1, 1, 1);
 
	// Gradient the image
	vtkSmartPointer<vtkImageGradient> imgGradient =	vtkSmartPointer<vtkImageGradient>::New();
	imgGradient->SetInputConnection(gs->GetOutputPort());
	imgGradient->SetDimensionality(3);
 
	vtkSmartPointer<vtkImageMagnitude> imgMagnitude = vtkSmartPointer<vtkImageMagnitude>::New();
	imgMagnitude->SetInputConnection(imgGradient->GetOutputPort());
 
	// Non maximum suppression
	vtkSmartPointer<vtkImageNonMaximumSuppression> nonMax =	vtkSmartPointer<vtkImageNonMaximumSuppression>::New();
	#if VTK_MAJOR_VERSION <= 5
	nonMax->SetMagnitudeInput(imgMagnitude->GetOutput());
	nonMax->SetVectorInput(imgGradient->GetOutput());
	#else
	imgMagnitude->Update();
	nonMax->SetMagnitudeInputData(imgMagnitude->GetOutput());
	imgGradient->Update();
	nonMax->SetVectorInputData(imgGradient->GetOutput());
	#endif
	nonMax->SetDimensionality(3);
 
	vtkSmartPointer<vtkImageConstantPad> pad = vtkSmartPointer<vtkImageConstantPad>::New();
	pad->SetInputConnection(imgGradient->GetOutputPort());
	pad->SetOutputNumberOfScalarComponents(3);
	pad->SetConstant(0);
 
	vtkSmartPointer<vtkImageToStructuredPoints> i2sp1 =	vtkSmartPointer<vtkImageToStructuredPoints>::New();
	i2sp1->SetInputConnection(nonMax->GetOutputPort());
	#if VTK_MAJOR_VERSION <= 5
	i2sp1->SetVectorInput(pad->GetOutput());
	#else
	pad->Update();
	i2sp1->SetVectorInputData(pad->GetOutput());
	#endif
 
	// Link edgles
	vtkSmartPointer<vtkLinkEdgels> imgLink = vtkSmartPointer<vtkLinkEdgels>::New();
	imgLink->SetInputConnection(i2sp1->GetOutputPort());
	imgLink->SetGradientThreshold(2);
 
	// Threshold links
	vtkSmartPointer<vtkThreshold> thresholdEdgels =	vtkSmartPointer<vtkThreshold>::New();
	thresholdEdgels->SetInputConnection(imgLink->GetOutputPort());
	thresholdEdgels->ThresholdByUpper(10);
	thresholdEdgels->AllScalarsOff();
 
	vtkSmartPointer<vtkGeometryFilter> gf =	vtkSmartPointer<vtkGeometryFilter>::New();
	gf->SetInputConnection(thresholdEdgels->GetOutputPort());
 
	vtkSmartPointer<vtkImageToStructuredPoints> i2sp = vtkSmartPointer<vtkImageToStructuredPoints>::New();
	i2sp->SetInputConnection(imgMagnitude->GetOutputPort());
	#if VTK_MAJOR_VERSION <= 5
	i2sp->SetVectorInput(pad->GetOutput());
	#else
	pad->Update();
	i2sp->SetVectorInputData(pad->GetOutput());
	#endif
 
	// Subpixel them
	vtkSmartPointer<vtkSubPixelPositionEdgels> spe = vtkSmartPointer<vtkSubPixelPositionEdgels>::New();
	spe->SetInputConnection(gf->GetOutputPort());
	#if VTK_MAJOR_VERSION <= 5
	spe->SetGradMaps(i2sp->GetStructuredPointsOutput());
	#else
	i2sp->Update();
	spe->SetGradMapsData(i2sp->GetStructuredPointsOutput());
	#endif
 
	vtkSmartPointer<vtkStripper> strip = vtkSmartPointer<vtkStripper>::New();
	strip->SetInputConnection(spe->GetOutputPort());
 
	vtkSmartPointer<vtkPolyDataMapper> dsm = vtkSmartPointer<vtkPolyDataMapper>::New();
	dsm->SetInputConnection(strip->GetOutputPort());
	dsm->ScalarVisibilityOff();
 
 
	return EXIT_SUCCESS;
}