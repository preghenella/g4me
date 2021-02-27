/// @author: Benedikt Volkel
/// @email: benedikt.volkel@cern.ch

#ifndef _Common_DetectorConstruction_h_
#define _Common_DetectorConstruction_h_

#include "G4String.hh"


namespace G4me {
namespace geometry {


struct Cylinder
{
  double x, y, z;
  double rMin, rMax;
  double length;
  G4String material;
  bool addSD;
};

struct Box
{
  double x, y, z;
  double dX, dY, dZ;
  G4String material;
  bool addSD;
};


} /** namespace common **/
} /** namespace G4me **/

#endif /** _Common_DetectorConstruction_h_ **/
