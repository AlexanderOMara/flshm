SET(
	shared
	dump.c
	hex2bin.c
	sleep.c
	strinv.c
)
FOREACH(share ${shared})
	GET_FILENAME_COMPONENT(name ${share} NAME_WE)

	ADD_LIBRARY(${name} ${share})

	TARGET_INCLUDE_DIRECTORIES(${name} PRIVATE "../../src" ".")
ENDFOREACH()
