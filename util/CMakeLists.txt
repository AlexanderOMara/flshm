ADD_SUBDIRECTORY(shared)

SET(
	utils
	flshmchatbot.c
	flshmconnectionadd.c
	flshmconnectionlist.c
	flshmconnectionnamevalid.c
	flshmconnectionremove.c
	flshmdump.c
	flshmmessageclear.c
	flshmmessagegenerateticks.c
	flshmmessageread.c
	flshmmessagetick.c
	flshmmessagewrite.c
	flshmopenclose.c
)

FOREACH(util ${utils})
	GET_FILENAME_COMPONENT(util_name ${util} NAME_WE)

	ADD_EXECUTABLE(${util_name} ${util})

	TARGET_INCLUDE_DIRECTORIES(${util_name} PRIVATE "../src" "./shared")

	TARGET_LINK_LIBRARIES(${util_name} flshm hexdump)

	SET_TARGET_PROPERTIES(${util_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTBIN})
ENDFOREACH()
