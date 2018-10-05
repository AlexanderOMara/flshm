ADD_SUBDIRECTORY(shared)

SET(
	utils
	openclose.c
	message-write.c
	message-tick.c
	message-read.c
	message-clear.c
	generate-ticks.c
	dump.c
	connection-remove.c
	connection-name-valid.c
	connection-list.c
	connection-add.c
	chatbot.c
)

FOREACH(util ${utils})
	GET_FILENAME_COMPONENT(util_name ${util} NAME_WE)
	SET(name flshm-${util_name})

	ADD_EXECUTABLE(${name} ${util})

	TARGET_INCLUDE_DIRECTORIES(${name} PRIVATE "../src" "./shared")

	TARGET_LINK_LIBRARIES(${name} flshm hexdump sleep)

	SET_TARGET_PROPERTIES(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTBIN})
ENDFOREACH()
