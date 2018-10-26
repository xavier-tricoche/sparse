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
#define FLOWMAP3D 1

#ifdef FLOWMAPCROPPED
	// 3D
	#define INPUT_SIGNAL "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\flowmap.nrrd"
	#define INPUT_SIGNAL_JACOBIAN "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\flowmapJacobian.nrrd"
	
	// 2D images
	#define INPUT_SIGNAL_IMG_ORI "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\signal"
	#define INPUT_SIGNAL_IMG_LIN "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\output"

	// 2D output
	#define OUTPUT_SIGNAL "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\output.nrrd"
	#define ERROR_SIGNAL "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\error.nrrd"
	#define ERROR_MAG_SIGNAL "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\errormag.nrrd"
	#define ERROR_ORI_SIGNAL "E:\\Chaos2Rost\\Projects\\FTLE2DSamer\\errorori.nrrd"
#endif

#ifdef FLOWMAP3D
	// 3D
	#define INPUT_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Seven\\flowmap.nrrd"
	#define INPUT_SIGNAL_JACOBIAN "E:\\Chaos2Rost\\EV13Data\\Seven\\flowmapJacobian.nrrd"
	
	// 2D images
	#define INPUT_SIGNAL_IMG_ORI "E:\\Chaos2Rost\\EV13Data\\Seven\\signal"
	#define INPUT_SIGNAL_IMG_LIN "E:\\Chaos2Rost\\EV13Data\\Seven\\output"
	#define INPUT_SIGNAL_IMG_FTLE "E:\\Chaos2Rost\\EV13Data\\Seven\\ftle_SIB2"

	// 2D output
	#define OUTPUT_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Seven\\output.nrrd"
	#define ERROR_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Seven\\error.nrrd"
	#define ERROR_MAG_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Seven\\errormag.nrrd"
	#define ERROR_ORI_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Seven\\errorori.nrrd"
#endif


#ifdef LINEARSYS
	// 3D
	#define INPUT_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Simple\\flowmap_n.nrrd"
	#define INPUT_SIGNAL_JACOBIAN "E:\\Chaos2Rost\\EV13Data\\Simple\\flowmapJacobian_n.nrrd"
	
	// 2D images
	#define INPUT_SIGNAL_IMG_ORI "E:\\Chaos2Rost\\EV13Data\\Simple\\signal"
	#define INPUT_SIGNAL_IMG_LIN "E:\\Chaos2Rost\\EV13Data\\Simple\\output"

	// 2D output
	#define OUTPUT_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Simple\\output.nrrd"
	#define ERROR_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Simple\\error.nrrd"
	#define ERROR_MAG_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Simple\\errormag.nrrd"
	#define ERROR_ORI_SIGNAL "E:\\Chaos2Rost\\EV13Data\\Simple\\errorori.nrrd"
#endif

