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
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkStructuredPoints.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPNGReader.h>
#include <vtkImageLuminance.h>
#include <vtkProperty.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageCast.h>
#include <vtkImageGradient.h>
#include <vtkImageMagnitude.h>
#include <vtkImageNonMaximumSuppression.h>
#include <vtkImageConstantPad.h>
#include <vtkImageToStructuredPoints.h>
#include <vtkLinkEdgels.h>
#include <vtkThreshold.h>
#include <vtkGeometryFilter.h>
#include <vtkSubPixelPositionEdgels.h>
#include <vtkStripper.h>
#include <vtkPolyDataMapper.h>
#include <vtkCamera.h>
 
using namespace std;