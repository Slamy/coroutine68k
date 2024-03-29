cmake_minimum_required (VERSION 3.12.0)

project (coroutine68k C CXX ASM)

option(BUILD_FOR_AMIGADOS "Build for AmigaDOS" false)
option(MEASURE_TIME "Use CIA to measure time" false)

include_directories(srcExample)
include_directories(srcExample/util)
include_directories(srcCoroutine)
include_directories(srcBenhoyt)
include_directories(${CMAKE_BINARY_DIR})

SET (SOURCES
	srcExample/main.cpp
	srcCoroutine/Coroutine68k.cpp
	srcExample/sample_coroutine68k.cpp
	srcExample/sample_cogenerator.cpp
	srcExample/sample_coparser.cpp
	srcExample/sample_stackless.cpp
	context.o
)

#create listing files
set(CMAKE_C_COMPILE_OBJECT "${CMAKE_C_COMPILE_OBJECT} -Wa,-adhln=<OBJECT>.lst")
set(CMAKE_CXX_COMPILE_OBJECT "${CMAKE_CXX_COMPILE_OBJECT} -Wa,-adhln=<OBJECT>.lst")


find_path( TOOLCHAIN_AMIGAOS_DIR m68k-amigaos-gcc PATHS
	/opt/m68k-amigaos/bin/
)

find_path( TOOLCHAIN_ELF_DIR m68k-elf-gcc PATHS
	/opt/m68k-elf.slamy2/bin/
)

set(NDKINCLUDE ${TOOLCHAIN_AMIGAOS_DIR}/../m68k-amigaos/ndk-include/)

include_directories(${NDKINCLUDE})


if (BUILD_FOR_AMIGADOS)
	set(TARGET_TRIPLET m68k-amigaos)
	SET(VASM_OUTPUT_TYPE hunk)
	set(TOOLCHAIN_DIR ${TOOLCHAIN_AMIGAOS_DIR})
	
	MESSAGE( STATUS "Build for AmigaDOS")
	
	SET (OPTIMIZE_FLAGS "-O3 -fomit-frame-pointer ")
	#SET (OPTIMIZE_FLAGS "-g -fomit-frame-pointer ")

	SET(CMAKE_C_FLAGS " -noixemul -Wall ${OPTIMIZE_FLAGS}  ")
	SET(CMAKE_CXX_FLAGS " -noixemul -std=c++14 -Wall ${OPTIMIZE_FLAGS} -Iinclude -fno-exceptions ")

	SET(CMAKE_LINK_FLAGS " -noixemul -Wl,-Map=amibench.map ")

else()
	
	set(TARGET_TRIPLET m68k-elf)
	SET(VASM_OUTPUT_TYPE elf)
	set(TOOLCHAIN_DIR ${TOOLCHAIN_ELF_DIR})
	
	MESSAGE( STATUS "Build for NDOS")
	set (DISKIMAGE ${CMAKE_SOURCE_DIR}/main.adf)
	
	SET (OPTIMIZE_FLAGS "-O3 -Werror -ffunction-sections ")
	SET (OPTIMIZE_FLAGS "${OPTIMIZE_FLAGS} -fomit-frame-pointer -fno-exceptions")
	#SET (OPTIMIZE_FLAGS "-g -fomit-frame-pointer ")

	SET(CMAKE_C_FLAGS " -m68000 -Wall ${OPTIMIZE_FLAGS} ")
	SET(CMAKE_CXX_FLAGS " -m68000 -std=c++14 -Wall ${OPTIMIZE_FLAGS} -fno-exceptions")
	
	SET(CMAKE_LINK_FLAGS " -m68000 -Wl,-v,--script=${CMAKE_SOURCE_DIR}/trackloaded.ld,-Map=amibench.map,--gc-sections,-flto -lm")
	SET(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -nostdlib -lstdc++ -lc -lgcc")
	
	SET(CMAKE_C_LINK_FLAGS " ${CMAKE_LINK_FLAGS} ")
	SET(CMAKE_CXX_LINK_FLAGS " ${CMAKE_LINK_FLAGS} -fno-exceptions")

	#Correct Order
	set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> -o <TARGET>")
	set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_C_COMPILER> <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> -o <TARGET>")

	include_directories(${TOOLCHAIN_ELF_DIR}/../m68k-elf/include)
	
	SET (SOURCES
		${SOURCES}
		srcExample/util/syscalls.c
		srcExample/util/uart.c
	)

	add_subdirectory(dde5bootloader)
endif()

SET(CMAKE_ASM_COMPILER	"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-gcc")
SET(CMAKE_C_COMPILER	"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-gcc")
SET(CMAKE_CXX_COMPILER	"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-g++")
set(CMAKE_SIZE			"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-size")
set(CMAKE_STRIP			"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-strip")
set(CMAKE_OBJCOPY 		"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-objcopy")
set(CMAKE_OBJDUMP 		"${TOOLCHAIN_DIR}/${TARGET_TRIPLET}-objdump")

add_executable(amibench ${SOURCES})

add_custom_command(
	OUTPUT context.o
	COMMAND ${TOOLCHAIN_AMIGAOS_DIR}/vasmm68k_mot -F${VASM_OUTPUT_TYPE} -o context.o -L context.lst ${CMAKE_CURRENT_SOURCE_DIR}/srcCoroutine/context.asm
	DEPENDS srcCoroutine/context.asm
)
	
	
if (BUILD_FOR_AMIGADOS)
	add_custom_command(TARGET amibench POST_BUILD
		COMMAND cp ${CMAKE_BINARY_DIR}/amibench ${CMAKE_SOURCE_DIR}/harddrive/m
	)
else()

	set_target_properties(amibench PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/trackloaded.ld)

	add_custom_command(
	OUTPUT amibench.bin ${DISKIMAGE}
		COMMAND ${CMAKE_OBJCOPY}
			-S -O binary amibench amibench.bin
		
		COMMAND python ${CMAKE_SOURCE_DIR}/tools/make-adf.py ${DISKIMAGE} dde5bootloader/DDE5-BootLoader.sum
		COMMAND dd conv=notrunc if=amibench.bin of=${DISKIMAGE} seek=2
		DEPENDS amibench dde5boot
	)
	
	# ----- Diskimage und Emulator ------
	add_custom_target(
		diskimage ALL
		DEPENDS ${DISKIMAGE}
	)

endif()

add_custom_target(sectionAnalyse
	COMMAND ${TOOLCHAIN_ELF_DIR}/${TARGET_TRIPLET}-nm -S amibench --size-sort
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	DEPENDS amibench
)

configure_file(srcExample/config.h.in config.h)

add_custom_target(applyCodeStyle
  COMMAND find -iname *.h -o -iname *.cpp -o -iname *.c | xargs clang-format -i
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(doxygen
  COMMAND doxygen
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

