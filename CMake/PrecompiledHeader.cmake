MACRO(ADD_PRECOMPILED_HEADER
	Target PrecompiledHeader PrecompiledSource SourcesVar)
  SET(_sources ${${SourcesVar}})
  IF(MSVC)
	ADD_MSVC_PRECOMPILED_HEADER(${PrecompiledHeader}
	  ${PrecompiledSource} _sources)
  ENDIF(MSVC)
  IF(CMAKE_COMPILER_IS_GNUCXX)
	ADD_GCC_PRECOMPILED_HEADER(${Target} ${PrecompiledHeader} _sources)
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDMACRO(ADD_PRECOMPILED_HEADER)

MACRO(ADD_MSVC_PRECOMPILED_HEADER
  PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC_IDE)
	GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
	SET(PrecompiledBinary
	  "$(IntDir)/${PrecompiledBasename}.pch")
	SET(Sources ${${SourcesVar}})
	SET_SOURCE_FILES_PROPERTIES(
	  ${PrecompiledSource}
	  PROPERTIES COMPILE_FLAGS
	  "/Yc\"${NAME_WE}\" /Fp\"${PrecompiledBinary}\""
	  OBJECT_OUTPUTS "${PrecompiledBinary}")
	SET_SOURCE_FILES_PROPERTIES(${Sources}
	  PROPERTIES COMPILE_FLAGS
	  "/Yu\"${PrecompiledBinary}\" /FI\"${PrecompiledBinary}\" /Fp\"${PrecompiledBinary}\""
	  OBJECT_DEPENDS "${PrecompiledBinary}")
	SET_SOURCE_FILES_PROPERTIES(
	  ${PrecompiledSource}
	  PROPERTIES COMPILE_FLAGS
	  "/Yc\"${NAME_WE}\" /Fp\"${PrecompiledBinary}\""
	  OBJECT_OUTPUTS "${PrecompiledBinary}")
	LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC_IDE)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)
IF(CMAKE_COMPILER_IS_GNUCXX)
  EXEC_PROGRAM(
	${CMAKE_CXX_COMPILER}
	ARGS					--version
	OUTPUT_VARIABLE _compiler_output)
  STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"
	gcc_compiler_version ${_compiler_output})
  IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	SET(PCHSupport_FOUND TRUE)
  ELSE(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
	IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
	  SET(PCHSupport_FOUND TRUE)
	ENDIF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
  ENDIF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MACRO(ADD_GCC_PRECOMPILED_HEADER _targetName _input _sources)
  GET_FILENAME_COMPONENT(_name ${_input} NAME)
  GET_FILENAME_COMPONENT(_path ${_input} PATH)
  SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
  IF(NOT CMAKE_BUILD_TYPE)
	SET(_output "${_outdir}/default.c++")
	SET(_compile_FLAGS ${CMAKE_CXX_FLAGS})
  ELSE(NOT CMAKE_BUILD_TYPE)
	SET(_output "${_outdir}/${CMAKE_BUILD_TYPE}.c++")
	STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
	SET(_compile_FLAGS ${${_flags_var_name}})
  ENDIF(NOT CMAKE_BUILD_TYPE)
  ADD_CUSTOM_COMMAND(
	OUTPUT ${_outdir}
	COMMAND ${CMAKE_COMMAND} -E make_directory ${_outdir} 
	)
  GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
  SET(_CMAKE_CURRENT_BINARY_DIR_included_before_path FALSE)
  FOREACH(item ${_directory_flags})
	LIST(APPEND _compile_FLAGS "-I${item}")
  ENDFOREACH(item)
  GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
  #LIST(APPEND _compile_FLAGS "-fPIC")
  LIST(APPEND _compile_FLAGS ${_directory_flags})
  SEPARATE_ARGUMENTS(_compile_FLAGS)
  ADD_CUSTOM_COMMAND(
	OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${_name}
	COMMAND ${CMAKE_COMMAND} -E copy  ${_input} ${CMAKE_CURRENT_BINARY_DIR}/${_name}
	)
  MESSAGE(${_compile_FLAGS})
  ADD_CUSTOM_COMMAND(
	OUTPUT ${_output}
	COMMAND ${CMAKE_CXX_COMPILER}
	${_compile_FLAGS}
	-x c++-header
	-o ${_output}
	${_input}
	DEPENDS ${_input} ${_outdir} ${CMAKE_CURRENT_BINARY_DIR}/${_name}
	)
  ADD_CUSTOM_TARGET(${_targetName}_gch
	DEPENDS ${_output}
	)
  ADD_DEPENDENCIES(${_targetName} ${_targetName}_gch )
  SET(_sourcesVar ${${_sources}})
  FOREACH(source ${_sourcesVar})
	SET_SOURCE_FILES_PROPERTIES(${source}
	  PROPERTIES
	  COMPILE_FLAGS "-include ${_name} -Winvalid-pch"
	  OBJECT_DEPENDS "${_output}")
  ENDFOREACH(source)
ENDMACRO(ADD_GCC_PRECOMPILED_HEADER)

