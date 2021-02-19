/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4SDManager.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4String.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

#include "TOFRICH/DetectorConstruction.hh"
#include "FCT/DetectorConstruction.hh"

namespace G4me {

/*****************************************************************/

DetectorConstruction::DetectorConstruction()
  : mDetectorDirectory(nullptr)
  , mDetectorEnableCmd(nullptr)
  , mTOFRICH(nullptr)
  , mFCT(nullptr)
  , mPipeDirectory(nullptr)
  , mPipeRadiusCmd(nullptr)
  , mPipeThicknessCmd(nullptr)
  , mPipeLengthCmd(nullptr)
  , mPipeRadius(1.6 * cm)
  , mPipeLength(100. * cm)
  , mPipeThickness(500 * um)
  , mTrackerDirectory(nullptr)
  , mTrackerAddLayerCmd(nullptr)
{

  mDetectorDirectory = new G4UIdirectory("/detector/");

  mDetectorEnableCmd = new G4UIcmdWithAString("/detector/enable", this);
  mDetectorEnableCmd->SetGuidance("Enable detector module");
  mDetectorEnableCmd->SetParameterName("select", false);
  mDetectorEnableCmd->SetCandidates("TOFRICH FCT");
  mDetectorEnableCmd->AvailableForStates(G4State_PreInit);
  
  /** beam pipe **/
  
  mPipeDirectory = new G4UIdirectory("/detector/pipe/");

  mPipeRadiusCmd = new G4UIcmdWithADoubleAndUnit("/detector/pipe/radius", this);
  mPipeRadiusCmd->SetGuidance("Radius of the beam pipe.");
  mPipeRadiusCmd->SetParameterName("radius", false);
  mPipeRadiusCmd->SetUnitCategory("Length");
  mPipeRadiusCmd->AvailableForStates(G4State_PreInit);
  
  mPipeLengthCmd = new G4UIcmdWithADoubleAndUnit("/detector/pipe/length", this);
  mPipeLengthCmd->SetGuidance("Radius of the beam pipe.");
  mPipeLengthCmd->SetParameterName("length", false);
  mPipeLengthCmd->SetUnitCategory("Length");
  mPipeLengthCmd->AvailableForStates(G4State_PreInit);
  
  mPipeThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/pipe/thickness", this);
  mPipeThicknessCmd->SetGuidance("Thickness of the beam pipe.");
  mPipeThicknessCmd->SetParameterName("thickness", false);
  mPipeThicknessCmd->SetUnitCategory("Length");
  mPipeThicknessCmd->AvailableForStates(G4State_PreInit);

  /** tracker **/

  mTrackerDirectory = new G4UIdirectory("/detector/tracker/");
  
  mTrackerAddLayerCmd = new G4UIcommand("/detector/tracker/addLayer", this);
  mTrackerAddLayerCmd->SetGuidance("Add a silicon tracker cylindrical layer.");
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("radius", 'd', false));
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("length", 'd', false));
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("thickness", 'd', false));
  mTrackerAddLayerCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mTrackerAddLayerCmd->AvailableForStates(G4State_PreInit);
}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{
  
  if (command == mDetectorEnableCmd) {
    if (value.compare("TOFRICH") == 0) mTOFRICH = new TOFRICH::DetectorConstruction();
    if (value.compare("FCT") == 0) mFCT = new FCT::DetectorConstruction();
  }
  
  if (command == mPipeRadiusCmd)
    mPipeRadius = mPipeRadiusCmd->GetNewDoubleValue(value);
  if (command == mPipeThicknessCmd)
    mPipeThickness = mPipeThicknessCmd->GetNewDoubleValue(value);
  if (command == mPipeLengthCmd)
    mPipeLength = mPipeLengthCmd->GetNewDoubleValue(value);
  if (command == mTrackerAddLayerCmd) {
    G4String radius_value, radius_unit, length_value, length_unit, thickness_value, thickness_unit;
    std::istringstream iss(value);
    iss >> radius_value    >> radius_unit
	>> length_value    >> length_unit
	>> thickness_value >> thickness_unit;
    G4double radius = command->ConvertToDimensionedDouble(G4String(radius_value + ' ' + radius_unit));
    G4double length = command->ConvertToDimensionedDouble(G4String(length_value + ' ' + length_unit));
    G4double thickness = command->ConvertToDimensionedDouble(G4String(thickness_value + ' ' + thickness_unit));
    mTrackerLayer.push_back({ {"radius", radius}, {"length", length}, {"thickness", thickness} });
  }
}
  
/*****************************************************************/

G4VPhysicalVolume *
DetectorConstruction::Construct() {

  /** materials **/
  G4NistManager *nist = G4NistManager::Instance();
  auto vacuum = nist->FindOrBuildMaterial("G4_Galactic");
  auto air = nist->FindOrBuildMaterial("G4_AIR");
  auto be = nist->FindOrBuildMaterial("G4_Be");
  auto si = nist->FindOrBuildMaterial("G4_Si");
  
  /** world **/
  auto world_s  = new G4Box("world_s", 1.5 * m, 1.5 * m, 3.0 * m);
  auto world_lv = new G4LogicalVolume(world_s, air, "world_lv");
  auto world_pv = new G4PVPlacement(0,                // no rotation
				    G4ThreeVector(),  // at (0,0,0)
				    world_lv,         // its logical volume
				    "world_pv",       // its name
				    0,                // its mother  volume
				    false,            // no boolean operations
				    0,                // copy number
				    false);           // checking overlaps    


  /** beam pipe **/

  G4cout << " --- constructing beam pipe " << G4endl
	 << "     radius    = " << mPipeRadius    / cm << " cm " << G4endl
	 << "     length    = " << mPipeLength    / cm << " cm " << G4endl
	 << "     thickness = " << mPipeThickness / um << " um " << G4endl;
  

#if 0
  auto vacuum_s = new G4Tubs("vacuum_s",
			     0.,
			     mPipeRadius - 0.5 * mPipeThickness,
			     mPipeLength,
			     0., 2. * M_PI);

  auto vacuum_lv = new G4LogicalVolume(vacuum_s, vacuum, "vacuum_lv");

  auto vacuum_pv = new G4PVPlacement(nullptr,
				     G4ThreeVector(0., 0., 0),
				     vacuum_lv,
				     "vacuum_pv",
				     world_lv,
				     false,
				     0,
				     false);  
#endif
  
  auto pipe_s = new G4Tubs("pipe_s",
			   mPipeRadius - 0.5 * mPipeThickness,
			   mPipeRadius + 0.5 * mPipeThickness,
			   mPipeLength,
			   0., 2. * M_PI);

  auto pipe_lv = new G4LogicalVolume(pipe_s, be, "pipe_lv");
  
  auto pipe_pv = new G4PVPlacement(nullptr,
				   G4ThreeVector(0., 0., 0),
				   pipe_lv,
				   "pipe_pv",
				   world_lv,
				   false,
				   0,
				   false);  

  /** silicon tracker **/

  G4cout << " --- constructing silicon tracker " << G4endl
	 << "     nlayers   = " << mTrackerLayer.size() << G4endl;
  G4cout << "     radius    = ";
  for (auto layer : mTrackerLayer) G4cout << layer["radius"] / cm << " ";
  G4cout << "cm " << G4endl;
  G4cout << "     length    = ";
  for (auto layer : mTrackerLayer) G4cout << layer["length"] / cm << " ";
  G4cout << "cm " << G4endl;
  G4cout << "     thickness = ";
  for (auto layer : mTrackerLayer) G4cout << layer["thickness"] / um << " ";
  G4cout << "um " << G4endl;
  
  int ilayer = 0;
  for (auto layer : mTrackerLayer) {
    
    auto layer_s = new G4Tubs("layer_s",
			      layer["radius"] - 0.5 * layer["thickness"],
			      layer["radius"] + 0.5 * layer["thickness"],
			      layer["length"],
			      0., 2. * M_PI);

    auto layer_lv = new G4LogicalVolume(layer_s, si, "layer_lv");
    
    auto layer_pv = new G4PVPlacement(nullptr,
				      G4ThreeVector(0., 0., 0),
				      layer_lv,
				      "layer_pv",
				      world_lv,
				      false,
				      ilayer,
				      false);  

    ++ilayer;
  }


  if (mTOFRICH)
    mTOFRICH->Construct(world_lv);

  if (mFCT)
    mFCT->Construct(world_lv);

  return world_pv;
}

/*****************************************************************/

void
DetectorConstruction::ConstructSDandField()
{
  if (G4LogicalVolumeStore::GetInstance()->GetVolume("layer_lv")) {
    auto tracker_sd = new SensitiveDetector("tracker_sd");
    G4SDManager::GetSDMpointer()->AddNewDetector(tracker_sd);
    SetSensitiveDetector("layer_lv", tracker_sd, true);
  }

  if (mTOFRICH)
    for (const auto &sd : mTOFRICH->GetSensitiveDetectors())
      SetSensitiveDetector(sd.first, sd.second, false);
  
  if (mFCT)
    for (const auto &sd : mFCT->GetSensitiveDetectors())
      SetSensitiveDetector(sd.first, sd.second, false);
  
  G4ThreeVector fieldValue = G4ThreeVector();
  auto MagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  MagFieldMessenger->SetVerboseLevel(1);
  
  // Register the field messenger for deleting
  G4AutoDelete::Register(MagFieldMessenger);
}

/*****************************************************************/

} /** namespace G4me **/
