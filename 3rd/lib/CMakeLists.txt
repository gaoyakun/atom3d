SET(Libs 
  lua51.lib
  dxsdk/x86/d3dx9.lib
  dxsdk/x86/dxguid.lib
  dxsdk/x86/dxerr.lib
)

INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/Debug CONFIGURATIONS Debug)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/Release CONFIGURATIONS Release)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/MinSizeRel CONFIGURATIONS MinSizeRel)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/RelWithDebInfo CONFIGURATIONS RelWithDebInfo)
