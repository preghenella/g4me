/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _TOFRICH_DetectorConstruction_h_
#define _TOFRICH_DetectorConstruction_h_

#include "G4UImessenger.hh"
#include <map> 


class G4VSensitiveDetector;
class G4LogicalVolume;
class G4Material;
class G4OpticalSurface;

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithAnInteger;

namespace G4me {
namespace TOFRICH {
  
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
  
  G4Material *ConstructMaterialAerogel(std::string name, bool isConstN = false, double constN = 1.0);
  G4Material *ConstructMaterialVessel(bool isConstN = false, double constN = 1.0);
  G4Material *ConstructMaterialSensor(bool isConstN = false, double constN = 1.0);

  G4OpticalSurface *ConstructOpticalSurfaceAerogel(std::string name);
  G4OpticalSurface *ConstructOpticalSurfaceSensor(std::string name);

  std::vector<std::pair<std::string, G4VSensitiveDetector *>> mSensitiveDetectors;
  
  G4UIdirectory *mDetectorDirectory;
  G4UIcmdWithADoubleAndUnit *mInnerRadiusCmd;
  G4UIcmdWithADoubleAndUnit *mInnerLengthCmd;
  G4UIcmdWithADoubleAndUnit *mOuterRadiusCmd;
  G4UIcmdWithADoubleAndUnit *mOuterLengthCmd;
  G4UIcmdWithADoubleAndUnit *mAerogelThicknessCmd;
  G4UIcmdWithADoubleAndUnit *mSensorThicknessCmd;
  G4UIcmdWithADouble *mAerogelRIndexCmd;
  G4UIcmdWithADouble *mVesselRIndexCmd;
  G4UIcmdWithAnInteger *mAerogelLayersCmd;

  double mInnerRadius;
  double mInnerLength;
  double mOuterRadius;
  double mOuterLength;
  double mAerogelThickness;
  double mSensorThickness;
  double mAerogelRIndex;
  double mVesselRIndex;
  int mAerogelLayers;
  
};

} /** namespace TOFRICH **/
} /** namespace G4me **/
  
#endif /** _TOFRICH_DetectorConstruction_h_ **/
