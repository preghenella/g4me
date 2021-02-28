/// @author: Benedikt Volkel
/// @email: benedikt.volkel@cern.ch

#ifndef _EMCAL_DetectorConstruction_h_
#define _EMCAL_DetectorConstruction_h_

#include "G4UImessenger.hh"
#include <vector>
#include "geometry/Common.hh"

class G4VSensitiveDetector;
class G4LogicalVolume;

class G4String;

class G4UIcommand;
class G4UIdirectory;

namespace G4me {
namespace EMCAL {

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
  G4UIcommand *mAddEMCalCmd;

  geometry::Cylinder mCylinder;

};

} /** namespace EMCAL **/
} /** namespace G4me **/

#endif /** _EMCAL_DetectorConstruction_h_ **/
