/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#ifndef _FCT_DetectorConstruction_h_
#define _FCT_DetectorConstruction_h_

#include "G4UImessenger.hh"
#include <map> 

class G4VSensitiveDetector;
class G4LogicalVolume;

class G4String;

class G4UIcommand;
class G4UIdirectory;

namespace G4me {
namespace FCT {

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

  G4UIcommand *mAddConverterCmd;
  std::vector<std::map<std::string, double>> mConverter;

  G4UIcommand *mAddSensorCmd;
  std::vector<std::map<std::string, double>> mSensor;

};

} /** namespace FPS **/
} /** namespace G4me **/
  
#endif /** __FCTDetectorConstruction_h_ **/
