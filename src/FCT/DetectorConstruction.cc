/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "FCT/DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4SDManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4String.hh"

#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"

namespace G4me {
namespace FCT {
  
/*****************************************************************/

DetectorConstruction::DetectorConstruction()
  : mDetectorDirectory(nullptr)
  , mAddConverterCmd(nullptr)
{

  mDetectorDirectory = new G4UIdirectory("/detector/FCT/");

  mAddConverterCmd = new G4UIcommand("/detector/FCT/addConverter", this);
  mAddConverterCmd->SetGuidance("Add a lead converter slab.");
  mAddConverterCmd->SetParameter(new G4UIparameter("x", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("y", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("z", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("rmin", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("rmax", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("thickness", 'd', false));
  mAddConverterCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mAddConverterCmd->AvailableForStates(G4State_PreInit);

  mAddSensorCmd = new G4UIcommand("/detector/FCT/addSensor", this);
  mAddSensorCmd->SetGuidance("Add a silicon sensor layer.");
  mAddSensorCmd->SetParameter(new G4UIparameter("x", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("y", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("z", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("rmin", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("rmax", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("thickness", 'd', false));
  mAddSensorCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mAddSensorCmd->AvailableForStates(G4State_PreInit);
}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mAddConverterCmd) {
    G4String x_val, y_val, z_val, rmin_val, rmax_val, thickness_val, unit;
    std::istringstream iss(value);
    iss >> x_val >> y_val >> z_val >> rmin_val >> rmax_val >> thickness_val >> unit;
    G4double x = command->ConvertToDimensionedDouble(G4String(x_val + ' ' + unit));
    G4double y = command->ConvertToDimensionedDouble(G4String(y_val + ' ' + unit));
    G4double z = command->ConvertToDimensionedDouble(G4String(z_val + ' ' + unit));
    G4double rmin = command->ConvertToDimensionedDouble(G4String(rmin_val + ' ' + unit));
    G4double rmax = command->ConvertToDimensionedDouble(G4String(rmax_val + ' ' + unit));
    G4double thickness = command->ConvertToDimensionedDouble(G4String(thickness_val + ' ' + unit));
    mConverter.push_back({ {"x", x}, {"y", y}, {"z", z}, {"rmin", rmin}, {"rmax", rmax}, {"thickness", thickness} });
  }

  if (command == mAddSensorCmd) {
    G4String x_val, y_val, z_val, rmin_val, rmax_val, thickness_val, unit;
    std::istringstream iss(value);
    iss >> x_val >> y_val >> z_val >> rmin_val >> rmax_val >> thickness_val >> unit;
    G4double x = command->ConvertToDimensionedDouble(G4String(x_val + ' ' + unit));
    G4double y = command->ConvertToDimensionedDouble(G4String(y_val + ' ' + unit));
    G4double z = command->ConvertToDimensionedDouble(G4String(z_val + ' ' + unit));
    G4double rmin = command->ConvertToDimensionedDouble(G4String(rmin_val + ' ' + unit));
    G4double rmax = command->ConvertToDimensionedDouble(G4String(rmax_val + ' ' + unit));
    G4double thickness = command->ConvertToDimensionedDouble(G4String(thickness_val + ' ' + unit));
    mSensor.push_back({ {"x", x}, {"y", y}, {"z", z}, {"rmin", rmin}, {"rmax", rmax}, {"thickness", thickness} });
  }
}
  
/*****************************************************************/

void
DetectorConstruction::Construct(G4LogicalVolume *world_lv) {
  
  G4cout << "--- constructing Forward Photon Spectrometer " << G4endl;
  
  /** materials **/
  G4NistManager *nist = G4NistManager::Instance();
  auto pb = nist->FindOrBuildMaterial("G4_Pb");
  auto si = nist->FindOrBuildMaterial("G4_Si");
  
  /** converter **/

  int iconverter = 0;
  for (auto converter : mConverter) {
    
    G4cout << " --- constructing lead converter #" << iconverter << G4endl
	   << "             x = " << converter["x"]         / cm << " cm " << G4endl
	   << "             y = " << converter["y"]         / cm << " cm " << G4endl
	   << "             z = " << converter["z"]         / cm << " cm " << G4endl
	   << "          rmin = " << converter["rmin"]      / cm << " cm " << G4endl
	   << "          rmax = " << converter["rmax"]      / cm << " cm " << G4endl
	   << "     thickness = " << converter["thickness"] / cm << " cm " << G4endl;

    auto converter_s = new G4Tubs(std::string("fct_converter_s_") + std::to_string(iconverter),
				  converter["rmin"], converter["rmax"], converter["thickness"],
				  0., 2. * M_PI);

    auto converter_lv = new G4LogicalVolume(converter_s, pb, std::string("fct_converter_lv_") + std::to_string(iconverter));
    
    auto converter_pv = new G4PVPlacement(nullptr,
					  G4ThreeVector(converter["x"], converter["y"], converter["z"]),
					  converter_lv,
					  std::string("fct_converter_pv_") + std::to_string(iconverter),
					  world_lv,
					  false,
					  iconverter,
					  false);  
    
    ++iconverter;
  }
  
  /** sensor **/

  int isensor = 0;
  for (auto sensor : mSensor) {
    
    G4cout << " --- constructing silicon sensor #" << isensor << G4endl
	   << "             x = " << sensor["x"]         / cm << " cm " << G4endl
	   << "             y = " << sensor["y"]         / cm << " cm " << G4endl
	   << "             z = " << sensor["z"]         / cm << " cm " << G4endl
	   << "          rmin = " << sensor["rmin"]      / cm << " cm " << G4endl
	   << "          rmax = " << sensor["rmax"]      / cm << " cm " << G4endl
	   << "     thickness = " << sensor["thickness"] / cm << " cm " << G4endl;
    
    auto sensor_s = new G4Tubs(std::string("fct_sensor_s_") + std::to_string(isensor),
			       sensor["rmin"], sensor["rmax"], sensor["thickness"],
			       0., 2. * M_PI);

    //    auto sensor_lv = new G4LogicalVolume(sensor_s, pb, std::string("sensor_lv_") + std::to_string(isensor));
    auto sensor_lv = new G4LogicalVolume(sensor_s, pb, "fct_sensor_lv");
    
    auto sensor_pv = new G4PVPlacement(nullptr,
					  G4ThreeVector(sensor["x"], sensor["y"], sensor["z"]),
					  sensor_lv,
					  std::string("fct_sensor_pv_") + std::to_string(isensor),
					  world_lv,
					  false,
					  isensor,
					  false);  
    
    ++isensor;
  }

  auto sensor_sd = new SensitiveDetector("fct_sensor_sd");
  G4SDManager::GetSDMpointer()->AddNewDetector(sensor_sd);
  RegisterSensitiveDetector("fct_sensor_lv", sensor_sd);
    
}

/*****************************************************************/

} /** namespace FPS **/
} /** namespace G4me **/
