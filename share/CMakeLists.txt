set(G4MACRO
  g4macro/init.mac
  g4macro/pythia8.mac
  )

set(PY8CONFIG
  py8config/pythia8_hi.cfg
  )

set(ANALYSIS
  analysis/io.C
  analysis/electron.C
  )

install(FILES ${G4MACRO} DESTINATION share/g4macro)
install(FILES ${PY8CONFIG} DESTINATION share/py8config)
install(FILES ${ANALYSIS} DESTINATION share/analysis)
