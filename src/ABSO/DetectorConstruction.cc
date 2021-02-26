/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "ABSO/DetectorConstruction.hh"
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
namespace ABSO {

/*****************************************************************/

DetectorConstruction::DetectorConstruction()
  : mDetectorDirectory(nullptr)
  , mInnerRadiusCmd(nullptr)
  , mThicknessCmd(nullptr)
  , mMaterialCmd(nullptr)
  , mInnerRadius(2*m)
  , mThickness(167*cm)
  , mMaterial("G4_Pb")
{

  mDetectorDirectory = new G4UIdirectory("/detector/ABSO/");

  mAddAbsoCylinderCmd = new G4UIcommand("/detector/ABSO/addCylinder", this);
  mAddAbsoCylinderCmd->SetGuidance("Add absorber cylinder.");
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("rmin", 'd', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("rmax", 'd', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("radius_unit", 's', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("length", 'd', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("length_unit", 's', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("material", 's', false));
  mAddAbsoCylinderCmd->SetParameter(new G4UIparameter("add_sd", 's', false));

  mAddAbsoCylinderCmd->AvailableForStates(G4State_PreInit);
}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mAddAbsoCmd) {
    G4String rMin, rMax, rUnit, length, lengthUnit; material, addSD;
    std::istringstream iss(value);
    iss >> rmin >> rmax >> rUnit >> length >> lengthUnit >> material >> addSD;
    mCylinders.push_back(Cylinder());
    auto& newCylinder = mCylinders.back();

    newCylinder.rMin = mAddAbsoCmd->ConvertToDimensionedDouble(G4String(rMin + " " + rUnit));
    newCylinder.rMax = mAddAbsoCmd->ConvertToDimensionedDouble(G4String(rMax + " " + rUnit));
    newCylinder.length = mAddAbsoCmd->ConvertToDimensionedDouble(G4String(length + " " + lengthUnit));
    newCylinder.material = material;
    newCylinder.addSD = mAddAbsoCmd->ConvertToBool(addSD)
  }
}

/*****************************************************************/

void
DetectorConstruction::Construct(G4LogicalVolume *world) {

  G4cout << "--- constructing Forward Photon Spectrometer " << G4endl;

  /** materials **/
  G4NistManager *nist = G4NistManager::Instance();
  int icyl = 0;
  bool hasSD = false;

  for(auto& cyl : mCylinders) {
    G4cout << " --- constructing ABSO cylinder #" << icyl << " with\n"
	         << "          rmin = " << cyl.rMin / cm << " cm\n"
	         << "          rmax = " << cyl.rMax / cm << " cm\n"
           << "          length = " << cyl.length / cm << " cm\n"
           << "          material = " << cyl.material" << G4endl;

    auto mat = nist->FindOrBuildMaterial(cyl.material);

    std::string name("abso_");
    if(cyl.addSD) {
      name += "sens_";
      hasSD = true;
    }


    auto abso = new G4Tubs(name + std::to_string(icyl),
 				  cyl.rMin, cyl.rMax, cyl.length, 0., 2. * M_PI);
    auto abso_lv = new G4LogicalVolume(abso, mat, name + "lv_" + std::to_string(icyl));
    auto abso_pv = new G4PVPlacement(nullptr,
					  G4ThreeVector(),
					  abso_lv,
					  name + "pv_" + std::to_string(icyl),
					  world,
					  false,
					  icyl,
					  false);
    icyl++;
  }

  if(hasSD) {
    auto absoSensSD = new SensitiveDetector("abso_sens_sd");
    G4SDManager::GetSDMpointer()->AddNewDetector(absoSensSD);
    RegisterSensitiveDetector("abso_sens_lv", absoSensSD);
  }


}

/*****************************************************************/

} /** namespace ABSO **/
} /** namespace G4me **/
