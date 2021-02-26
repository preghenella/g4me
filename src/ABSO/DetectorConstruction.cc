/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "ABSO/DetectorConstruction.hh"
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
namespace ABSO {

/*****************************************************************/

DetectorConstruction::DetectorConstruction()
  : mDetectorDirectory(nullptr)
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
  // position (optional)
  auto x = new G4UIparameter("x", 'd', false);
  x->SetDefaultValue(0.);
  auto y = new G4UIparameter("y", 'd', false);
  y->SetDefaultValue(0.);
  auto z = new G4UIparameter("z", 'd', false);
  z->SetDefaultValue(0.);
  mAddAbsoCylinderCmd->SetParameter(x);
  mAddAbsoCylinderCmd->SetParameter(y);
  mAddAbsoCylinderCmd->SetParameter(z);
  auto posUnit = new G4UIparameter("pos_unit", 'd', false);
  posUnit->SetDefaultValue("cm");
  mAddAbsoCylinderCmd->SetParameter(posUnit);
  mAddAbsoCylinderCmd->AvailableForStates(G4State_PreInit);

  mAddAbsoBoxCmd = new G4UIcommand("/detector/ABSO/addCylinder", this);
  mAddAbsoBoxCmd->SetGuidance("Add absorber cylinder.");
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("dX", 'd', false));
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("dY", 'd', false));
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("dZ", 'd', false));
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("length_unit", 's', false));
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("material", 's', false));
  mAddAbsoBoxCmd->SetParameter(new G4UIparameter("add_sd", 's', false));
  // position (optional)
  auto xBox = new G4UIparameter("xBox", 'd', false);
  xBox->SetDefaultValue(0.);
  auto yBox = new G4UIparameter("yBox", 'd', false);
  yBox->SetDefaultValue(0.);
  auto zBox = new G4UIparameter("zBox", 'd', false);
  zBox->SetDefaultValue(0.);
  mAddAbsoBoxCmd->SetParameter(xBox);
  mAddAbsoBoxCmd->SetParameter(yBox);
  mAddAbsoBoxCmd->SetParameter(zBox);
  auto posUnitBox = new G4UIparameter("pos_unit", 'd', false);
  posUnitBox->SetDefaultValue("cm");
  mAddAbsoBoxCmd->SetParameter(posUnitBox);
  mAddAbsoBoxCmd->AvailableForStates(G4State_PreInit);
}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == mAddAbsoCylinderCmd) {
    G4String rMin, rMax, rUnit, length, lengthUnit, material, addSD, x, y, z, posUnit;
    std::istringstream iss(value);
    iss >> rMin >> rMax >> rUnit >> length >> lengthUnit >> material >> addSD >> x >> y >> z >> posUnit;
    mCylinders.push_back(Cylinder());
    auto& newCylinder = mCylinders.back();

    newCylinder.x = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(x + " " + posUnit));
    newCylinder.y = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(y + " " + posUnit));
    newCylinder.z = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(z + " " + posUnit));
    newCylinder.rMin = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(rMin + " " + rUnit));
    newCylinder.rMax = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(rMax + " " + rUnit));
    newCylinder.length = mAddAbsoCylinderCmd->ConvertToDimensionedDouble(G4String(length + " " + lengthUnit));
    newCylinder.material = material;
    newCylinder.addSD = mAddAbsoCylinderCmd->ConvertToBool(addSD);

  } else if (command == mAddAbsoBoxCmd) {
    G4String dX, dY, dZ, dUnit, material, addSD, x, y, z, posUnit;
    std::istringstream iss(value);
    iss >> dX >> dY >> dZ >> dUnit >> material >> addSD >> x >> y >> z >> posUnit;
    mBoxes.push_back(Box());
    auto& newBox = mBoxes.back();

    newBox.x = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(x + " " + posUnit));
    newBox.y = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(y + " " + posUnit));
    newBox.z = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(z + " " + posUnit));
    newBox.dX = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(dX + " " + dUnit));
    newBox.dY = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(dY + " " + dUnit));
    newBox.dZ = mAddAbsoBoxCmd->ConvertToDimensionedDouble(G4String(dZ + " " + dUnit));
    newBox.material = material;
    newBox.addSD = mAddAbsoBoxCmd->ConvertToBool(addSD);
  }
}

/*****************************************************************/

void
DetectorConstruction::Construct(G4LogicalVolume *world) {

  G4cout << "--- constructing Forward Photon Spectrometer " << G4endl;

  /** materials **/
  G4NistManager *nist = G4NistManager::Instance();
  int ivol = 0;
  bool hasSD = false;

  for(auto& cyl : mCylinders) {
    G4cout << " --- constructing ABSO #" << ivol << " (cylinder) with\n"
	         << "          rmin = " << cyl.rMin / cm << " cm\n"
	         << "          rmax = " << cyl.rMax / cm << " cm\n"
           << "          length = " << cyl.length / cm << " cm\n"
           << "          material = " << cyl.material << "\n"
           << "at position ("
           << cyl.x << "," << cyl.y << "," << cyl.z << ")"
           << G4endl;

    auto mat = nist->FindOrBuildMaterial(cyl.material);

    std::string name("abso_");
    if(cyl.addSD) {
      name += "sens_";
      hasSD = true;
    }


    auto abso = new G4Tubs(name + std::to_string(ivol),
 				  cyl.rMin, cyl.rMax, cyl.length, 0., 2. * M_PI);
    auto abso_lv = new G4LogicalVolume(abso, mat, name + "lv_" + std::to_string(ivol));
    auto abso_pv = new G4PVPlacement(nullptr,
					  G4ThreeVector(cyl.x, cyl.y, cyl.z),
					  abso_lv,
					  name + "pv_" + std::to_string(ivol),
					  world,
					  false,
					  ivol,
					  false);
    ivol++;
  }

  for(auto& box : mBoxes) {
    G4cout << " --- constructing ABSO #" << ivol << " (box) with\n"
	         << "          dX = " << box.dX / cm << " cm\n"
	         << "          dY = " << box.dY / cm << " cm\n"
           << "          dZ = " << box.dZ / cm << " cm\n"
           << "          material = " << box.material << "\n"
           << "at position ("
           << box.x / cm << "," << box.y / cm << "," << box.z / cm << ")"
           << G4endl;

    auto mat = nist->FindOrBuildMaterial(box.material);

    std::string name("abso_");
    if(box.addSD) {
      name += "sens_";
      hasSD = true;
    }

    auto abso = new G4Box(name + std::to_string(ivol),
 				  box.z, box.y, box.z);
    auto abso_lv = new G4LogicalVolume(abso, mat, name + "lv_" + std::to_string(ivol));
    auto abso_pv = new G4PVPlacement(nullptr,
					  G4ThreeVector(box.x, box.y, box.z),
					  abso_lv,
					  name + "pv_" + std::to_string(ivol),
					  world,
					  false,
					  ivol,
					  false);
    ivol++;
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
