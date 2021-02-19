### @author: Roberto Preghenella
### @email: preghenella@bo.infn.it

set(SOURCES
  TOFRICH/DetectorConstruction.cc
  FCT/DetectorConstruction.cc
  DetectorConstruction.cc
  SensitiveDetector.cc
  PrimaryGeneratorAction.cc
  Pythia8.cc
  GeneratorPythia8.cc
  GeneratorHepMC.cc
  StackingAction.cc
  SteppingAction.cc
  EventAction.cc
  RunAction.cc
  ExternalDecayerPhysics.cc
  ExternalDecayer.cc
  RootIO.cc
  PrimaryParticleInformation.cc
  )

set(HEADERS
  TOFRICH/DetectorConstruction.hh
  FCT/DetectorConstruction.hh
  DetectorConstruction.hh
  SensitiveDetector.hh
  PrimaryGeneratorAction.hh
  Pythia8.hh
  GeneratorPythia8.hh
  GeneratorHepMC.hh
  StackingAction.hh
  SteppingAction.hh
  EventAction.hh
  RunAction.hh
  ExternalDecayerPhysics.hh
  ExternalDecayer.hh
  RootIO.hh
  PrimaryParticleInformation.hh
  )

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_executable(${PROJECT_NAME} main.cc ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${Pythia_LIBRARIES} ${HepMC3_LIBRARIES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
