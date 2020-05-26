/// @author: Roberto Preghenella
/// @email: preghenella@bo.infn.it

#include "TOFRICH/DetectorConstruction.hh"
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
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAnInteger.hh"

namespace G4me {
namespace TOFRICH {
  
/*****************************************************************/

DetectorConstruction::DetectorConstruction() 
  : mSensitiveDetectors()
  , mDetectorDirectory(nullptr)
  , mInnerRadiusCmd(nullptr)
  , mInnerLengthCmd(nullptr)
  , mOuterRadiusCmd(nullptr)
  , mOuterLengthCmd(nullptr)
  , mAerogelThicknessCmd(nullptr)
  , mSensorThicknessCmd(nullptr)
  , mAerogelRIndexCmd(nullptr)
  , mVesselRIndexCmd(nullptr)
  , mAerogelLayersCmd(nullptr)
  , mInnerRadius(105. * cm)
  , mInnerLength(105. * cm)
  , mOuterRadius(135. * cm)
  , mOuterLength(135. * cm)
  , mAerogelThickness(5. * cm)
  , mSensorThickness(500. * um)
  , mAerogelRIndex(1.02)
  , mVesselRIndex(1.00)
  , mAerogelLayers(5)
{

  mDetectorDirectory = new G4UIdirectory("/detector/TOFRICH/");

  mInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/inner_radius", this);
  mInnerRadiusCmd->SetGuidance("Inner radius of the detector.");
  mInnerRadiusCmd->SetParameterName("inner radius", false);
  mInnerRadiusCmd->SetUnitCategory("Length");
  mInnerRadiusCmd->AvailableForStates(G4State_PreInit);
  
  mInnerLengthCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/inner_length", this);
  mInnerLengthCmd->SetGuidance("Inner length of the detector.");
  mInnerLengthCmd->SetParameterName("inner length", false);
  mInnerLengthCmd->SetUnitCategory("Length");
  mInnerLengthCmd->AvailableForStates(G4State_PreInit);
  
  mOuterRadiusCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/outer_radius", this);
  mOuterRadiusCmd->SetGuidance("Outer radius of the detector.");
  mOuterRadiusCmd->SetParameterName("outer radius", false);
  mOuterRadiusCmd->SetUnitCategory("Length");
  mOuterRadiusCmd->AvailableForStates(G4State_PreInit);
  
  mOuterLengthCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/outer_length", this);
  mOuterLengthCmd->SetGuidance("Outer length of the detector.");
  mOuterLengthCmd->SetParameterName("outer length", false);
  mOuterLengthCmd->SetUnitCategory("Length");
  mOuterLengthCmd->AvailableForStates(G4State_PreInit);
  
  mAerogelThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/aerogel_thickness", this);
  mAerogelThicknessCmd->SetGuidance("Thickness of the aerogel.");
  mAerogelThicknessCmd->SetParameterName("aerogel thickness", false);
  mAerogelThicknessCmd->SetUnitCategory("Length");
  mAerogelThicknessCmd->AvailableForStates(G4State_PreInit);
  
  mSensorThicknessCmd = new G4UIcmdWithADoubleAndUnit("/detector/TOFRICH/sensor_thickness", this);
  mSensorThicknessCmd->SetGuidance("Thickness of the sensor layer.");
  mSensorThicknessCmd->SetParameterName("aerogel thickness", false);
  mSensorThicknessCmd->SetUnitCategory("Length");
  mSensorThicknessCmd->AvailableForStates(G4State_PreInit);
  
  mAerogelRIndexCmd = new G4UIcmdWithADouble("/detector/TOFRICH/aerogel_rindex", this);
  mAerogelRIndexCmd->SetGuidance("Aerogel refractive index.");
  mAerogelRIndexCmd->SetParameterName("aerogel rindex", false);
  mAerogelRIndexCmd->AvailableForStates(G4State_PreInit);
  
  mVesselRIndexCmd = new G4UIcmdWithADouble("/detector/TOFRICH/vessel_rindex", this);
  mVesselRIndexCmd->SetGuidance("Vessel refractive index.");
  mVesselRIndexCmd->SetParameterName("vessel rindex", false);
  mVesselRIndexCmd->AvailableForStates(G4State_PreInit);
  
  mAerogelLayersCmd = new G4UIcmdWithAnInteger("/detector/TOFRICH/aerogel_layers", this);
  mAerogelLayersCmd->SetGuidance("Number of focusing aerogel layers.");
  mAerogelLayersCmd->SetParameterName("aerogel layers", false);
  mAerogelLayersCmd->AvailableForStates(G4State_PreInit);

  mAerogelAddLayerCmd = new G4UIcommand("/detector/TOFRICH/addAerogelLayer", this);
  mAerogelAddLayerCmd->SetGuidance("Add a layer of aerogel.");
  mAerogelAddLayerCmd->SetParameter(new G4UIparameter("radius", 'd', false));
  mAerogelAddLayerCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mAerogelAddLayerCmd->SetParameter(new G4UIparameter("thickness", 'd', false));
  mAerogelAddLayerCmd->SetParameter(new G4UIparameter("unit", 's', false));
  mAerogelAddLayerCmd->SetParameter(new G4UIparameter("rindex", 'd', false));
  mAerogelAddLayerCmd->AvailableForStates(G4State_PreInit);
}

/*****************************************************************/

DetectorConstruction::~DetectorConstruction()
{
}

/*****************************************************************/

void
DetectorConstruction::SetNewValue(G4UIcommand *command, G4String value)
{

  if (command == mInnerRadiusCmd)
    mInnerRadius = mInnerRadiusCmd->GetNewDoubleValue(value);
  if (command == mInnerLengthCmd)
    mInnerLength = mInnerLengthCmd->GetNewDoubleValue(value);
  if (command == mOuterRadiusCmd)
    mOuterRadius = mOuterRadiusCmd->GetNewDoubleValue(value);
  if (command == mOuterLengthCmd)
    mOuterLength = mOuterLengthCmd->GetNewDoubleValue(value);
  if (command == mAerogelThicknessCmd)
    mAerogelThickness = mAerogelThicknessCmd->GetNewDoubleValue(value);
  if (command == mSensorThicknessCmd)
    mSensorThickness = mSensorThicknessCmd->GetNewDoubleValue(value);
  if (command == mAerogelRIndexCmd)
    mAerogelRIndex = mAerogelRIndexCmd->GetNewDoubleValue(value);
  if (command == mVesselRIndexCmd)
    mVesselRIndex = mVesselRIndexCmd->GetNewDoubleValue(value);
  if (command == mAerogelLayersCmd)
    mAerogelLayers = mAerogelLayersCmd->GetNewIntValue(value);

  if (command == mAerogelAddLayerCmd) {
    G4String radius_value, radius_unit, thickness_value, thickness_unit, rindex_value;
    std::istringstream iss(value);
    iss >> radius_value    >> radius_unit
	>> thickness_value >> thickness_unit
	>> rindex_value;
    G4double radius = command->ConvertToDimensionedDouble(G4String(radius_value + ' ' + radius_unit));
    G4double thickness = command->ConvertToDimensionedDouble(G4String(thickness_value + ' ' + thickness_unit));
    G4double rindex = command->ConvertToDouble(rindex_value);
    mAerogelLayer.push_back({ {"radius", radius}, {"thickness", thickness}, {"rindex", rindex} });
  }

}
  
/*****************************************************************/

void
DetectorConstruction::Construct(G4LogicalVolume *world_lv)
{

  G4cout << " --- constructing TOF-RICH " << G4endl
	 << "     inner radius      = " << mInnerRadius      / cm << " cm " << G4endl
	 << "     inner length      = " << mInnerLength      / cm << " cm " << G4endl
	 << "     outer radius      = " << mOuterRadius      / cm << " cm " << G4endl
	 << "     outer length      = " << mOuterLength      / cm << " cm " << G4endl
	 << "     aerogel thickness = " << mAerogelThickness / cm << " cm " << G4endl
	 << "     sensor thickness  = " << mSensorThickness  / um << " um " << G4endl
	 << "     aerogel rindex    = " << mAerogelRIndex                   << G4endl
	 << "     vessel rindex     = " << mVesselRIndex                    << G4endl
	 << "     aerogel layers    = " << mAerogelLayers                   << G4endl;

  /** vessel **/
  
  auto vessel_m = ConstructMaterialVessel(true, mVesselRIndex);
  
  auto vessel_s = new G4Tubs("vessel_s",
			     mInnerRadius,
			     mOuterRadius,
			     mOuterLength, // should be a different shape
			     0., 2. * M_PI);
  
  auto vessel_lv = new G4LogicalVolume(vessel_s, vessel_m, "vessel_lv");
  
  auto vessel_pv = new G4PVPlacement(nullptr,
				     G4ThreeVector(0., 0., 0),
				     vessel_lv,
				     "vessel_pv",
				     world_lv,
				     false,
				     0,
				     false);  
  
  /** aerogel **/

  G4cout << " --- constructing aerogel " << G4endl
	 << "     nlayers   = " << mAerogelLayer.size() << G4endl;
  G4cout << "     radius    = ";
  for (auto layer : mAerogelLayer) G4cout << layer["radius"] / cm << " ";
  G4cout << "cm " << G4endl;
  G4cout << "     thickness = ";
  for (auto layer : mAerogelLayer) G4cout << layer["thickness"] / cm << " ";
  G4cout << "cm " << G4endl;
  G4cout << "     rindex    = ";
  for (auto layer : mAerogelLayer) G4cout << layer["rindex"];
  G4cout << G4endl;
  
  int ilayer = 0;
  for (auto layer : mAerogelLayer) {
    
    /** calculate refractive index for focusing features **/
    auto thickness = layer["thickness"];
    auto length = mInnerLength;
    auto radius_in = layer["radius"];
    auto radius_out = radius_in + thickness;
    auto rindex = layer["rindex"];
      
    G4cout << " --- constructing TOF-RICH focusing aerogel layer " << ilayer << G4endl
	   << "     aerogel radius_in  = " << radius_in  / cm << " cm " << G4endl
	   << "     aerogel radius_out = " << radius_out / cm << " cm " << G4endl
	   << "     aerogel length     = " << length     / cm << " cm " << G4endl
	   << "     aerogel rindex     = " << rindex                    << G4endl;

    auto aerogel_m = ConstructMaterialAerogel(std::string("aerogel_m_") + std::to_string(ilayer), true, rindex);
    auto aerogel_os = ConstructOpticalSurfaceAerogel(std::string("aerogel_os_") + std::to_string(ilayer));
    
    auto aerogel_s = new G4Tubs(std::string("aerogel_s_") + std::to_string(ilayer),
				radius_in,
				radius_out,
				length,
				0., 2. * M_PI);
    
    auto aerogel_lv = new G4LogicalVolume(aerogel_s, aerogel_m, std::string("aerogel_lv_") + std::to_string(ilayer));
    
    auto aerogel_pv = new G4PVPlacement(nullptr,
					G4ThreeVector(0., 0., 0),
					aerogel_lv,
					std::string("aerogel_pv_") + std::to_string(ilayer),
					vessel_lv,
					false,
					0,
					false);  
    
    // from aerogel to vessel
    auto aerogel_ls = new G4LogicalBorderSurface(std::string("aerogel_ls_") + std::to_string(ilayer),
						 vessel_pv,  // volume in
						 aerogel_pv, // volume out
						 aerogel_os);

    ++ilayer;
  }

  
#if 0
  auto gel_distance = mOuterRadius - mInnerRadius;
  auto gel_distance2 = gel_distance * gel_distance;
  for (int ilayer = 0; ilayer < mAerogelLayers; ++ilayer) {

    /** calculate refractive index for focusing features **/
    auto thickness = mAerogelThickness / mAerogelLayers;
    auto length = mInnerLength;
    auto radius_in = mInnerRadius + ilayer * thickness;
    auto radius_out = radius_in + thickness;
    auto distance = mOuterRadius - radius_in;
    auto distance2 = distance * distance;
    auto rindex = sqrt(1. / (1. - (gel_distance2 / distance2) * (1. - 1. / (mAerogelRIndex * mAerogelRIndex))));
      
    G4cout << " --- constructing TOF-RICH focusing aerogel layer " << ilayer << G4endl
	   << "     aerogel radius_in  = " << radius_in  / cm << " cm " << G4endl
	   << "     aerogel radius_out = " << radius_out / cm << " cm " << G4endl
	   << "     aerogel length     = " << length     / cm << " cm " << G4endl
	   << "     aerogel rindex     = " << rindex                    << G4endl;

    auto aerogel_m = ConstructMaterialAerogel(std::string("aerogel_m_") + std::to_string(ilayer), true, rindex);
    auto aerogel_os = ConstructOpticalSurfaceAerogel(std::string("aerogel_os_") + std::to_string(ilayer));
    
    auto aerogel_s = new G4Tubs(std::string("aerogel_s_") + std::to_string(ilayer),
				radius_in,
				radius_out,
				length,
				0., 2. * M_PI);
    
    auto aerogel_lv = new G4LogicalVolume(aerogel_s, aerogel_m, std::string("aerogel_lv_") + std::to_string(ilayer));
    
    auto aerogel_pv = new G4PVPlacement(nullptr,
					G4ThreeVector(0., 0., 0),
					aerogel_lv,
					std::string("aerogel_pv_") + std::to_string(ilayer),
					vessel_lv,
					false,
					0,
					false);  
    
    // from aerogel to vessel
    auto aerogel_ls = new G4LogicalBorderSurface(std::string("aerogel_ls_") + std::to_string(ilayer),
						 vessel_pv,  // volume in
						 aerogel_pv, // volume out
						 aerogel_os);

  }
#endif
  
  /** sensor **/
  
  auto sensor_m = ConstructMaterialSensor(true, 1.00);
  auto sensor_os = ConstructOpticalSurfaceSensor("sensor_os");

  auto sensor_s = new G4Tubs("sensor_s",
			     mOuterRadius - 0.5 * mSensorThickness,
			     mOuterRadius + 0.5 * mSensorThickness,
			     mOuterLength,
			     0., 2. * M_PI);
  
  auto sensor_lv = new G4LogicalVolume(sensor_s, sensor_m, "sensor_lv");
  
  auto sensor_pv = new G4PVPlacement(nullptr,
					G4ThreeVector(0., 0., 0),
					sensor_lv,
					"sensor_pv",
					vessel_lv,
					false,
					0,
					false);  

  auto sensor_ls = new G4LogicalBorderSurface("sensor_ls",
					      sensor_pv,
					      vessel_pv,
					      sensor_os);
  
  auto sensor_sd = new SensitiveDetector("sensor_sd");
  G4SDManager::GetSDMpointer()->AddNewDetector(sensor_sd);
  RegisterSensitiveDetector("sensor_lv", sensor_sd);
  
}

/*****************************************************************/

G4Material *
DetectorConstruction::ConstructMaterialAerogel(std::string name, bool isConstN, double constN)
{

  /** elements and material **/
  G4Element *C = new G4Element("Aerogel_Carbon", "C", 6, 12.011 * g/mole);
  G4Element *F = new G4Element("Aerogel_Fluorine", "F", 9, 18.998 * g/mole);
  G4Material *mat = new G4Material(name, 5.734 * mg/cm3, 2);
  mat->AddElement(C, 2);
  mat->AddElement(F, 6);

  /** material properties table **/
  G4double energy[10] = { 1.2915 * eV, 1.4089 * eV, 1.5498 * eV, 1.722 * eV, 1.9373 * eV,
			  2.214 * eV, 2.583 * eV, 3.0996 * eV, 3.8745 * eV, 5.166 * eV };

  double n[10] = { 1.028016033, 1.028055862, 1.0281084, 1.028179723, 1.028280043,
		   1.028427656, 1.028658131, 1.029048716, 1.029796312, 1.03155063 };

  G4double abs[10] = { 500. * cm, 500. * cm, 500. * cm, 500. * cm, 500. * cm,
		       500. * cm, 500. * cm, 500. * cm, 500. * cm, 500. * cm };
  
  G4double ray[10] = { 5.4 * cm, 5.4 * cm, 5.4 * cm, 5.4 * cm, 5.4 * cm,
		       5.4 * cm, 5.4 * cm, 5.4 * cm, 5.4 * cm, 5.4 * cm };
  
  if (isConstN) for (int i = 0; i < 10; ++i) n[i] = constN;
  
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable(); 
  mpt->AddProperty("RINDEX", energy, n, 10)->SetSpline(true);
  mpt->AddProperty("ABSLENGTH", energy, abs, 10)->SetSpline(true);
  mpt->AddProperty("RAYLEIGH", energy, ray, 10)->SetSpline(true);
  mat->SetMaterialPropertiesTable(mpt);
  
  return mat;
}

/*****************************************************************/

G4Material *
DetectorConstruction::ConstructMaterialVessel(bool isConstN, double constN)
{

  /** elements and material **/
  G4Element *Ar = new G4Element("Vessel_Argon", "Ar", 18, 39.948 * g/mole);
  G4Material *mat = new G4Material("Vessel", 1.7838 * kg/m3, 1);
  mat->AddElement(Ar, 1);

  /** material properties table **/
  G4double energy[10] = { 1.2915 * eV, 1.4089 * eV, 1.5498 * eV, 1.722 * eV, 1.9373 * eV,
			  2.214 * eV, 2.583 * eV, 3.0996 * eV, 3.8745 * eV, 5.166 * eV };
  
  G4double n[10] = { 1.00081514, 1.000815863, 1.000816816, 1.000818108, 1.00081992,
		     1.000822578, 1.000826707, 1.000833647, 1.000846733, 1.000876456 };
  
  G4double abs[10] = { 10. * m, 10. * m, 10. * m, 10. * m, 10. * m,
		       10. * m, 10. * m, 10. * m, 10. * m, 10. * m };
  
  if (isConstN) for (int i = 0; i < 10; ++i) n[i] = constN;
  
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable(); 
  mpt->AddProperty("RINDEX", energy, n, 10)->SetSpline(true);
  mpt->AddProperty("ABSLENGTH", energy, abs, 10)->SetSpline(true);
  mat->SetMaterialPropertiesTable(mpt);
  
  return mat;
}

/*****************************************************************/

G4Material *
DetectorConstruction::ConstructMaterialSensor(bool isConstN, double constN)
{

  /** elements and material **/
  G4Element *Si = new G4Element("Sensor_Silicon", "Si", 14, 28.085 * g/mole);
  G4Material *mat = new G4Material("Sensor", 2.3290 * g/cm3, 1);
  mat->AddElement(Si, 1);

  /** material properties table **/
  G4double energy[10] = { 1.2915 * eV, 1.4089 * eV, 1.5498 * eV, 1.722 * eV, 1.9373 * eV,
			  2.214 * eV, 2.583 * eV, 3.0996 * eV, 3.8745 * eV, 5.166 * eV };
  
  G4double n[10] = { 1.00081514, 1.000815863, 1.000816816, 1.000818108, 1.00081992,
		     1.000822578, 1.000826707, 1.000833647, 1.000846733, 1.000876456 };
  
  G4double abs[10] = { 10. * m, 10. * m, 10. * m, 10. * m, 10. * m,
		       10. * m, 10. * m, 10. * m, 10. * m, 10. * m };
  
  if (isConstN) for (int i = 0; i < 10; ++i) n[i] = constN;
  
  G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable(); 
  mpt->AddProperty("RINDEX", energy, n, 10)->SetSpline(true);
  mpt->AddProperty("ABSLENGTH", energy, abs, 10)->SetSpline(true);
  mat->SetMaterialPropertiesTable(mpt);
  
  return mat;
}

/*****************************************************************/

G4OpticalSurface *
DetectorConstruction::ConstructOpticalSurfaceAerogel(std::string name)
{
  G4double energy[2] = { 1.2915 * eV, 5.166 * eV };
  G4double reflectivity[2] = {0., 0.};
  G4double transmittance[2] = {1., 1.};
  auto mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  mpt->AddProperty("TRANSMITTANCE", energy, transmittance, 2);

  auto os = new G4OpticalSurface(name);
  os->SetType(dielectric_dielectric);
  os->SetFinish(polished);
  os->SetModel(glisur);
  os->SetMaterialPropertiesTable(mpt);

  return os;
}

/*****************************************************************/

G4OpticalSurface *
DetectorConstruction::ConstructOpticalSurfaceSensor(std::string name)
{
  G4double energy[2] = { 1.2915 * eV, 5.166 * eV };
  G4double reflectivity[2] = {0., 0.};
  G4double efficiency[2] = {1., 1.};
  auto mpt = new G4MaterialPropertiesTable();
  mpt->AddProperty("REFLECTIVITY", energy, reflectivity, 2);
  mpt->AddProperty("EFFICIENCY", energy, efficiency, 2);

  auto os = new G4OpticalSurface(name);
  os->SetType(dielectric_metal);
  os->SetFinish(polished);
  os->SetModel(glisur);
  os->SetMaterialPropertiesTable(mpt);

  return os;
}

/*****************************************************************/

} /** namespace TOFRICH **/
} /** namespace G4me **/
