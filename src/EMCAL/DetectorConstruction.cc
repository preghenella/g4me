/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "EMCAL/DetectorConstruction.hh"
#include "DetectorInfo.hh"
#include "SensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
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
namespace EMCAL {

/*****************************************************************/

DetectorConstruction::DetectorConstruction()
  : mDetectorDirectory(nullptr)
  , mAddEMCalCmd(nullptr)
  , mCylinder({0., 0., 0., 0., 0., 0., "", false})
{

  mDetectorDirectory = new G4UIdirectory("/detector/EMCAL/");

  mAddEMCalCmd = new G4UIcommand("/detector/EMCAL/addEMCal", this);
  mAddEMCalCmd->SetGuidance("Add EMCal cylinder.");
  mAddEMCalCmd->SetParameter(new G4UIparameter("rmin", 'd', false));
  mAddEMCalCmd->SetParameter(new G4UIparameter("rmax", 'd', false));
  mAddEMCalCmd->SetParameter(new G4UIparameter("radius_unit", 's', false));
  mAddEMCalCmd->SetParameter(new G4UIparameter("length", 'd', false));
  mAddEMCalCmd->SetParameter(new G4UIparameter("length_unit", 's', false));
  mAddEMCalCmd->SetParameter(new G4UIparameter("material", 's', false));
  // position (optional)
  auto x = new G4UIparameter("x", 'd', false);
  x->SetDefaultValue(0.);
  x->SetOmittable(true);
  auto y = new G4UIparameter("y", 'd', false);
  y->SetDefaultValue(0.);
  y->SetOmittable(true);
  auto z = new G4UIparameter("z", 'd', false);
  z->SetDefaultValue(0.);
  z->SetOmittable(true);
  mAddEMCalCmd->SetParameter(x);
  mAddEMCalCmd->SetParameter(y);
  mAddEMCalCmd->SetParameter(z);
  auto posUnit = new G4UIparameter("pos_unit", 'd', false);
  posUnit->SetDefaultValue("cm");
  posUnit->SetOmittable(true);
  mAddEMCalCmd->SetParameter(posUnit);
  mAddEMCalCmd->AvailableForStates(G4State_PreInit);

}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mAddEMCalCmd) {
    G4String rMin, rMax, rUnit, length, lengthUnit, material, x, y, z, posUnit;
    std::istringstream iss(value);
    iss >> rMin >> rMax >> rUnit >> length >> lengthUnit >> material >> x >> y >> z >> posUnit;

    mCylinder.x = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(x + " " + posUnit));
    mCylinder.y = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(y + " " + posUnit));
    mCylinder.z = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(z + " " + posUnit));
    mCylinder.rMin = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(rMin + " " + rUnit));
    mCylinder.rMax = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(rMax + " " + rUnit));
    mCylinder.length = mAddEMCalCmd->ConvertToDimensionedDouble(G4String(length + " " + lengthUnit));
    mCylinder.material = material;
    mCylinder.addSD = true;
  }
}

/*****************************************************************/

void
DetectorConstruction::Construct(G4LogicalVolume *world) {

  G4cout << "--- constructing electromagnetoc calorimeter " << G4endl;

  /** materials **/
  G4NistManager *nist = G4NistManager::Instance();

  G4cout << " --- constructing EMCAL with\n"
         << "          rmin = " << mCylinder.rMin / cm << " cm\n"
         << "          rmax = " << mCylinder.rMax / cm << " cm\n"
         << "          length = " << mCylinder.length / cm << " cm\n"
         << "          material = " << mCylinder.material << "\n"
         << "at position ("
         << mCylinder.x << "," << mCylinder.y << "," << mCylinder.z << ")"
         << G4endl;

  auto mat = nist->FindOrBuildMaterial(mCylinder.material);

  auto emcal = new G4Tubs("emcal",
				  mCylinder.rMin, mCylinder.rMax, mCylinder.length, 0., 2. * M_PI);
  auto emcal_lv = new G4LogicalVolume(emcal, mat, "emcal_lv");
  auto emcal_pv = new G4PVPlacement(nullptr,
				  G4ThreeVector(mCylinder.x, mCylinder.y, mCylinder.z),
				  emcal_lv,
				  "emcal_pv",
				  world,
				  false,
				  detID::EMCAL,
				  false);

  auto emcalSD = new SensitiveDetector("emcal_sd");
  G4SDManager::GetSDMpointer()->AddNewDetector(emcalSD);
  RegisterSensitiveDetector("emcal_lv", emcalSD);

}

/*****************************************************************/

} /** namespace EMCAL **/
} /** namespace G4me **/
