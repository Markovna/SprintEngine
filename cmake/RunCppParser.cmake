message(STATUS "Generating files from ${INPUT} with ${GEN} to ${OUTPUT}")
message(STATUS "Includes: ${INCLUDES}")
message(STATUS "Implicit includes: ${IMPLICIT_INCLUDES}")
message(STATUS "Defines: ${DEFINES}")

execute_process(
	COMMAND "${GEN}"
		--sources ${INPUT}
		--includes ${INCLUDES}
		--implicit-includes ${IMPLICIT_INCLUDES}
		--defines ${DEFINES}
		--output ${OUTPUT}
	RESULT_VARIABLE result)
if (result)
	message(FATAL_ERROR "Error running ${CMD}")
endif()
