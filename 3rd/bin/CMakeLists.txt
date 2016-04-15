IF (BUILD_WIN64)
	SET(Dlls 
		 x64/D3DX9_43.dll
		 x64/OpenAL32.dll
		 x64/AntTweakBar64.dll
		 x64/lua51.dll
		 x64/lua51d.dll
		 x64/wrap_oal.dll
	)
ELSE (BUILD_WIN64)
	SET(Dlls 
		 x86/D3DX9_43.dll
		 x86/OpenAL32.dll
		 x86/AntTweakBar.dll
		 x86/lua51.dll
		 x86/lua51d.dll
		 x86/wrap_oal.dll
	)
ENDIF (BUILD_WIN64)

INSTALL(FILES ${Dlls} DESTINATION ${DEPLOY_DIR}/Debug CONFIGURATIONS Debug)
INSTALL(FILES ${Dlls} DESTINATION ${DEPLOY_DIR}/Release CONFIGURATIONS Release)
INSTALL(FILES ${Dlls} DESTINATION ${DEPLOY_DIR}/MinSizeRel CONFIGURATIONS MinSizeRel)
INSTALL(FILES ${Dlls} DESTINATION ${DEPLOY_DIR}/RelWithDebInfo CONFIGURATIONS RelWithDebInfo)
