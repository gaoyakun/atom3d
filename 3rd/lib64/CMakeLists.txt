SET(Libs 
  lua51.lib
  lua51d.lib
  dxsdk/x64/d3dx9.lib
  dxsdk/x64/dxguid.lib
  dxsdk/x64/dxerr.lib
)

INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/Debug CONFIGURATIONS Debug)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/Release CONFIGURATIONS Release)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/MinSizeRel CONFIGURATIONS MinSizeRel)
INSTALL(FILES ${Libs} DESTINATION ${LIBRARY_DIR}/RelWithDebInfo CONFIGURATIONS RelWithDebInfo)
