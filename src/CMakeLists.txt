set(SOURCES
  DetectorConstruction.cc
  SensitiveDetector.cc
  PrimaryGeneratorAction.cc
  GeneratorPythia8.cc
  StackingAction.cc
  SteppingAction.cc
  EventAction.cc
  RunAction.cc
  RootIO.cc
  )

set(HEADERS
  DetectorConstruction.hh
  SensitiveDetector.hh
  PrimaryGeneratorAction.hh
  GeneratorPythia8.hh
  StackingAction.hh
  SteppingAction.hh
  EventAction.hh
  RunAction.hh
  RootIO.hh
  )

add_executable(${PROJECT_NAME} main.cc ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} ${Geant4_LIBRARIES} ${Pythia_LIBRARIES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
