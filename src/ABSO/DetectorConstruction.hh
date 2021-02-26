/// @author: Benedikt Volkel
/// @email: benedikt.volkel@cern.ch

#ifndef _ABSO_DetectorConstruction_h_
#define _ABSO_DetectorConstruction_h_

#include "G4UImessenger.hh"
#include <vector>

class G4VSensitiveDetector;
class G4LogicalVolume;

class G4String;

class G4UIcommand;
class G4UIdirectory;

namespace G4me {
namespace ABSO {


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

class DetectorConstruction : public G4UImessenger
{

public:

  DetectorConstruction();
  ~DetectorConstruction() override;

  void Construct(G4LogicalVolume *world);

  const std::vector<std::pair<std::string, G4VSensitiveDetector *>> &GetSensitiveDetectors() const { return mSensitiveDetectors; };

protected:

  void SetNewValue(G4UIcommand *command, G4String value);

  void RegisterSensitiveDetector(std::string lv, G4VSensitiveDetector *sd) { mSensitiveDetectors.push_back({lv, sd}); };
  std::vector<std::pair<std::string, G4VSensitiveDetector *>> mSensitiveDetectors;

  G4UIdirectory *mDetectorDirectory;
  G4UIcommand *mAddAbsoCylinderCmd;
  G4UIcommand *mAddAbsoBoxCmd;

  std::vector<Cylinder> mCylinders;
  std::vector<Box> mBoxes;

};

} /** namespace ABSO **/
} /** namespace G4me **/

#endif /** _ABSO_DetectorConstruction_h_ **/
