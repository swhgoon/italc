CMAKE_MINIMUM_REQUIRED(VERSION 2.4.5)

PROJECT(italc)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
SET(CMAKE_BUILD_TYPE relwithdebinfo)

IF(COMMAND CMAKE_POLICY)
	CMAKE_POLICY(SET CMP0005 NEW)
	CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

INCLUDE(AddFileDependencies)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(FindPkgConfig)
INCLUDE(Qt4Hacks)

SET(VERSION_MAJOR "2")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")
SET(VERSION_SUFFIX "")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
IF(VERSION_SUFFIX)
	SET (VERSION "${VERSION}-${VERSION_SUFFIX}")
ENDIF(VERSION_SUFFIX)
ADD_DEFINITIONS(-D'ITALC_VERSION="${VERSION}"')


INCLUDE(DetectMachine)

# check for pthreads and TLS support
IF(WIN32)
	SET(ITALC_HAVE_LIBPTHREAD true)
ELSE(WIN32)
    FIND_PACKAGE(Threads)
	IF(CMAKE_USE_PTHREADS_INIT)
		SET(ITALC_HAVE_LIBPTHREAD true)
	ENDIF(CMAKE_USE_PTHREADS_INIT)
ENDIF(WIN32)

CHECK_C_SOURCE_COMPILES("static __thread int p = 0; int main() {}" ITALC_HAVE_TLS)

CHECK_INCLUDE_FILES(arpa/inet.h ITALC_HAVE_ARPA_INET_H)
CHECK_INCLUDE_FILES(unistd.h ITALC_HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(fcntl.h ITALC_HAVE_FCNTL_H)
CHECK_INCLUDE_FILES(limits.h ITALC_HAVE_LIMITS_H)
CHECK_INCLUDE_FILES(memory.h ITALC_HAVE_MEMORY_H)
CHECK_INCLUDE_FILES(netdb.h ITALC_HAVE_NETDB_H)
CHECK_INCLUDE_FILES(netinet/in.h ITALC_HAVE_NETINET_IN_H)
CHECK_INCLUDE_FILES(stdint.h ITALC_HAVE_STDINT_H)
CHECK_INCLUDE_FILES(stdlib.h ITALC_HAVE_STDLIB_H)
CHECK_INCLUDE_FILES(stdbool.h ITALC_HAVE_STDBOOL_H)
CHECK_INCLUDE_FILES(string.h ITALC_HAVE_STRING_H)
CHECK_INCLUDE_FILES(strings.h ITALC_HAVE_STRINGS_H)
CHECK_INCLUDE_FILES(sys/ioctl.h ITALC_HAVE_SYS_IOCTL_H)
CHECK_INCLUDE_FILES(sys/socket.h ITALC_HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILES(sys/time.h ITALC_HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILES(sys/timeb.h ITALC_HAVE_SYS_TIMEB_H)
CHECK_INCLUDE_FILES(sys/stat.h ITALC_HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILES(syslog.h ITALC_HAVE_SYSLOG_H)
CHECK_INCLUDE_FILES(unistd.h ITALC_HAVE_UNISTD_H)
CHECK_INCLUDE_FILES(pwd.h ITALC_HAVE_PWD_H)
CHECK_INCLUDE_FILES(sys/types.h ITALC_HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILES(utmpx.h ITALC_HAVE_UTMPX_H)
CHECK_INCLUDE_FILES(sys/wait.h ITALC_HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILES(time.h ITALC_HAVE_TIME_H)
CHECK_INCLUDE_FILES(errno.h ITALC_HAVE_ERRNO_H)
CHECK_INCLUDE_FILES(pthread.h ITALC_HAVE_PTHREAD_H)
CHECK_INCLUDE_FILES(sys/ipc.h ITALC_HAVE_SYS_IPC_H)
CHECK_INCLUDE_FILES(sys/shm.h ITALC_HAVE_SYS_SHM_H)
CHECK_INCLUDE_FILES(stdarg.h ITALC_HAVE_STDARG_H)
CHECK_INCLUDE_FILES(signal.h ITALC_HAVE_SIGNAL_H)
CHECK_INCLUDE_FILES(ctype.h ITALC_HAVE_CTYPE_H)
CHECK_INCLUDE_FILES(process.h ITALC_HAVE_PROCESS_H)
CHECK_INCLUDE_FILES(dlfcn.h ITALC_HAVE_DLFCN_H)
CHECK_INCLUDE_FILES(inttypes.h ITALC_HAVE_INTTYPES_H)
CHECK_INCLUDE_FILES(vfork.h ITALC_HAVE_VFORK_H)
CHECK_INCLUDE_FILES(linux/fb.h ITALC_HAVE_LINUX_FB_H)
CHECK_INCLUDE_FILES(linux/input.h ITALC_HAVE_LINUX_INPUT_H)
CHECK_INCLUDE_FILES(linux/uinput.h ITALC_HAVE_LINUX_UINPUT_H)
CHECK_INCLUDE_FILES(linux/videodev.h ITALC_HAVE_LINUX_VIDEODEV_H)
CHECK_INCLUDE_FILES(linux/videodev2.h ITALC_HAVE_LINUX_VIDEODEV2_H)

IF(NOT ITALC_BUILD_WIN32)
	FIND_PACKAGE(PAM REQUIRED)
ENDIF()

SET(CMAKE_REQUIRED_LIBRARIES "-lm")

SET(FUNCS dup2 floor ftime geteuid gethostbyname gethostname getpwnam getpwuid getspnam gettimeofday getuid grantpt inet_ntoa initgroups memcmp memcpy memmove memset mkfifo mmap pow putenv select seteuid setpgrp setsid setutxent socket strchr strdup strerror strpbrk strrchr strstr uname waitpid)
FOREACH(_func ${FUNCS})
	STRING(TOUPPER "${_func}" fuc)
	CHECK_FUNCTION_EXISTS(${_func} ITALC_HAVE_${fuc})
ENDFOREACH(_func ${FUNCS})

# TODO: ITALC_WORDS_BIGENDIAN

# check for Qt4

SET(QT_MIN_VERSION "4.6.0")
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
IF(WIN32)
	SET(QT_QM_RCC "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource>")
	FILE(GLOB QT_QM ${MINGW_PREFIX}/share/qt4/translations/qt_*.qm)
	FOREACH(f ${QT_QM})
		STRING(REGEX MATCH ".*qt_help.*" TMP "${f}")
		IF(NOT TMP)
			STRING(REPLACE "${MINGW_PREFIX}/share/qt4/translations/" "" fa "${f}")
			SET(QT_QM_RCC "${QT_QM_RCC}<file alias=\"${fa}\">${f}</file>")
		ENDIF()
	ENDFOREACH()
	SET(QT_QM_RCC "${QT_QM_RCC}</qresource></RCC>")
	FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lib/qt_qm.qrc "${QT_QM_RCC}")
ELSE()
	EXEC_PROGRAM(${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_TRANSLATIONS" OUTPUT_VARIABLE QT_TRANSLATIONS_DIR)
	IF(EXISTS "${QT_TRANSLATIONS_DIR}")
		MESSAGE("-- Found Qt translations in ${QT_TRANSLATIONS_DIR}")
		ADD_DEFINITIONS(-D'QT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}"')
	ENDIF(EXISTS "${QT_TRANSLATIONS_DIR}")
ENDIF()

IF(NOT WIN32)
	STRING(REPLACE "-DQT_DLL" "" QT_DEFINITIONS "${QT_DEFINITIONS}")
ENDIF(NOT WIN32)
INCLUDE(${QT_USE_FILE})


# find libraries
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(JPEG REQUIRED)
IF(ITALC_BUILD_WIN32)
	SET(OPENSSL_LIBRARIES -L${MINGW_PREFIX}/bin -leay32)
ELSE(ITALC_BUILD_WIN32)
	FIND_PACKAGE(OpenSSL REQUIRED)
ENDIF(ITALC_BUILD_WIN32)
FIND_PACKAGE(PNG)

# find X libraries
IF(NOT ITALC_BUILD_WIN32)
	FIND_PACKAGE(X11 REQUIRED)

	SET(CMAKE_REQUIRED_LIBRARIES ${X11_LIBRARIES} ${X11_XTest_LIB})
	SET(ITALC_HAVE_X11 TRUE)

	IF(X11_XShm_FOUND)
		SET(ITALC_HAVE_XSHM TRUE)
	ENDIF(X11_XShm_FOUND)
	IF(X11_XTest_FOUND)
		SET(ITALC_HAVE_XTEST TRUE)
	ENDIF(X11_XTest_FOUND)
	IF(X11_Xinerama_FOUND)
		SET(ITALC_HAVE_LIBXINERAMA TRUE)
	ENDIF(X11_Xinerama_FOUND)
	IF(X11_Xrandr_FOUND)
		SET(ITALC_HAVE_LIBXRANDR TRUE)
	ENDIF(X11_Xrandr_FOUND)
	IF(X11_Xfixes_FOUND)
		SET(ITALC_HAVE_LIBXFIXES TRUE)
	ENDIF(X11_Xfixes_FOUND)
	IF(X11_Xdamage_FOUND)
		SET(ITALC_HAVE_LIBXDAMAGE TRUE)
	ENDIF(X11_Xdamage_FOUND)

	CHECK_FUNCTION_EXISTS(XReadScreen ITALC_HAVE_SOLARIS_XREADSCREEN)
	CHECK_FUNCTION_EXISTS(FBPMForceLevel ITALC_HAVE_FBPM)
	CHECK_FUNCTION_EXISTS(DPMSForceLevel ITALC_HAVE_DPMS)
	CHECK_FUNCTION_EXISTS(XTestGrabControl ITALC_HAVE_XTESTGRABCONTROL)
	CHECK_FUNCTION_EXISTS(XRecordEnableContextAsync ITALC_HAVE_RECORD)
	CHECK_INCLUDE_FILES(X11/extensions/readdisplay.h ITALC_HAVE_IRIX_XREADDISPLAY)
	CHECK_INCLUDE_FILES(X11/XKBlib.h ITALC_HAVE_XKBLIB_H)
	IF(ITALC_HAVE_XKBLIB_H)
		CHECK_FUNCTION_EXISTS(XkbSelectEvents ITALC_HAVE_XKEYBOARD)
	ENDIF(ITALC_HAVE_XKBLIB_H)
	SET(ITALC_HAVE_LIBCRYPT FALSE)
ENDIF(NOT ITALC_BUILD_WIN32)
SET(ITALC_ALLOW24BPP TRUE)
SET(ITALC_BACKCHANNEL TRUE)

# find Java compiler for the JavaViewer
FIND_PACKAGE(Java)
IF(JAVA_COMPILE)
	SET(STATUS_JAVA "yes")
ELSE(JAVA_COMPILE)
	SET(STATUS_JAVA "no - please install a Java compiler")
ENDIF(JAVA_COMPILE)


SET(ITALCCONFIG ${CMAKE_BINARY_DIR}/italcconfig.h)
SET(RFBCONFIG ${CMAKE_BINARY_DIR}/rfb/rfbconfig.h)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/italcconfig.h.in ${ITALCCONFIG})
SET(IN_FILES iTALC.nsi italc.spec ima/data/italc ima/data/italc.desktop ica/ica.rc ima/italc.rc imc/imc.rc)
FOREACH(f ${IN_FILES})
	CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/${f}.in ${CMAKE_BINARY_DIR}/${f} @ONLY)
ENDFOREACH(f ${IN_FILES})

FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/rfb)
IF(${ITALCCONFIG} IS_NEWER_THAN ${RFBCONFIG})
	EXECUTE_PROCESS(COMMAND sed -e "s/ITALC_/LIBVNCSERVER_/g" INPUT_FILE ${ITALCCONFIG} OUTPUT_FILE ${RFBCONFIG})
ENDIF(${ITALCCONFIG} IS_NEWER_THAN ${RFBCONFIG})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-exceptions ${CXXFLAGS}")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${CFLAGS}")
IF(ITALC_BUILD_WIN32)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes -Wno-pragmas")
ELSE()
	SET(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} -lcrypto)
ENDIF(ITALC_BUILD_WIN32)

ADD_DEFINITIONS(-D'LIB_DIR="${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/"' -D'PLUGIN_DIR="${CMAKE_INSTALL_PREFIX}/${LIB_DIR}/italc/"')
ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBZ)
ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBJPEG)
IF(PNG_FOUND)
	ADD_DEFINITIONS(-DLIBVNCSERVER_HAVE_LIBPNG)
ENDIF(PNG_FOUND)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/lib/include ${ZLIB_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
LINK_DIRECTORIES(${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_BINARY_DIR}/lib)
LINK_LIBRARIES(${QT_LIBRARIES} ${ZLIB_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES} ${OPENSSL_LIBRARIES})

# make sub-directories
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(ica)
ADD_SUBDIRECTORY(ima)
ADD_SUBDIRECTORY(imc)

INSTALL()

#
# rules for building localizations
#
FILE(GLOB italc_LOCALES lib/resources/*.ts)
SET(ts_targets "")
SET(qm_targets "")
FILE(GLOB_RECURSE italc_SOURCES *.cpp *.h *.ui)
FOREACH(_ts_file ${italc_LOCALES})
	STRING(REPLACE "${CMAKE_SOURCE_DIR}/lib/resources/" "" _ts_target "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_file "${_ts_file}")
	STRING(REPLACE ".ts" ".qm" _qm_target "${_ts_target}")
	ADD_CUSTOM_TARGET(${_ts_target} COMMAND ${QT_LUPDATE_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/lib/include -locations none -no-obsolete ${italc_SOURCES} -ts ${_ts_file})
	ADD_CUSTOM_TARGET(${_qm_target} COMMAND ${QT_LRELEASE_EXECUTABLE} ${_ts_file} -qm ${_qm_file})
	LIST(APPEND ts_targets "${_ts_target}")
	LIST(APPEND qm_targets "${_qm_target}")
ENDFOREACH(_ts_file ${italc_LOCALES})

ADD_CUSTOM_TARGET(update-locales)
FOREACH(_item ${ts_targets})
	ADD_DEPENDENCIES(update-locales ${_item})
ENDFOREACH(_item ${ts_targets})

ADD_CUSTOM_TARGET(finalize-locales)
FOREACH(_item ${qm_targets})
	ADD_DEPENDENCIES(finalize-locales ${_item})
ENDFOREACH(_item ${qm_targets})


#
# add tarball-target
#
SET(TMP "italc-${VERSION}")
ADD_CUSTOM_TARGET(dist
			COMMAND make clean
			COMMAND rm -rf ${TMP}
			COMMAND mkdir -p ${TMP}
			COMMAND cp AUTHORS build_mingw* ChangeLog CMakeLists.txt COPYING INSTALL italcconfig.h.in italc.spec.in iTALC.nsi.in README* TODO ${TMP}
			COMMAND cp -r cmake contrib doc lib ica ima imc nsis ${TMP}
			COMMAND rm -rf `find ${TMP} -type d -name ".svn"`  `find ${TMP} -name cmake_install.cmake` `find ${TMP} -name Makefile` `find ${TMP} -type d -name CMakeFiles` ${TMP}/CMakeCache.txt `find ${TMP} -name "*.depends"` ${TMP}/ima/data/italc ${TMP}/ima/data/italc.desktop
			COMMAND tar cjf italc-${VERSION}.tar.bz2 ${TMP}
			COMMAND rm -rf ${TMP})

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "italcconfig.h")

ADD_CUSTOM_TARGET(win-nsi
			COMMAND make
			COMMAND rm -rf ${TMP}
			COMMAND mkdir -p ${TMP}
			COMMAND cp lib/ItalcCore.dll ica/ica.exe ica/win32/vnchooks.dll ica/win32/authSSP.dll ima/italc.exe imc/imc.exe ${TMP}
			COMMAND ${STRIP} ${TMP}/*
			COMMAND cp -r ${CMAKE_SOURCE_DIR}/doc ${TMP}
			COMMAND cp -r ${CMAKE_SOURCE_DIR}/contrib ${TMP}
			COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${TMP}
			COMMAND cp ${CMAKE_SOURCE_DIR}/COPYING ${TMP}/LICENSE.TXT
			COMMAND cp ${CMAKE_SOURCE_DIR}/README ${TMP}/README.TXT
			COMMAND todos ${TMP}/*.txt ${TMP}/*.TXT
			COMMAND makensis iTALC.nsi
			#COMMAND rm -rf ${TMP}
		)


#
# display configuration information
#

MESSAGE("\n"
"iTALC build summary\n"
"--------------------\n"
"* Install prefix              : ${CMAKE_INSTALL_PREFIX}\n"
"* Build type                  : ${CMAKE_BUILD_TYPE}\n"
"* Platform                    : ${CMAKE_SYSTEM_PROCESSOR} (${Machine})\n"
"* Builtin JavaViewer support  : ${STATUS_JAVA}\n"
"* Compile flags               : ${CMAKE_C_FLAGS} (CXX: ${CMAKE_CXX_FLAGS})\n"
)

MESSAGE(
"\n"
"-----------------------------------------------------------------\n"
"IMPORTANT:\n"
"After installing missing packages, remove CMakeCache.txt before\n"
"running cmake again!\n"
"-----------------------------------------------------------------\n"
"\n\n")


INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "iTALC - Intelligent Teaching And Learning with Computers")
SET(CPACK_PACKAGE_VENDOR "ITALC Developers")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}-${VERSION_SUFFIX}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "iTALC ${VERSION}")
IF(WIN32)
SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/data\\\\nsis_branding.bmp")
SET(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/ima/data\\\\italc.ico")
SET(CPACK_NSIS_INSTALLED_ICON_NAME "italc.exe")
SET(CPACK_NSIS_DISPLAY_NAME "iTALC ${VERSION}")
SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\italc.sourceforge.net")
SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\italc.sourceforge.net")
SET(CPACK_NSIS_CONTACT "italc-devel@lists.sourceforge.net")
SET(CPACK_PACKAGE_EXECUTABLES "italc.exe;iTALC ${VERSION}")
SET(CPACK_NSIS_MENU_LINKS "italc.exe;iTALC ${VERSION}")
ELSE(WIN32)
SET(CPACK_STRIP_FILES "bin/italc;bin/ica;${LIB_DIR}/*.so;${PLUGIN_DIR}/*.so")
SET(CPACK_PACKAGE_EXECUTABLES "italc" "iTALC binary")
ENDIF(WIN32)

SET(MACOSX_BUNDLE_ICON_FILE "${CMAKE_SOURCE_DIR}/data/themes/default/icon.png")
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "iTALC")
SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_NAME "iTALC")
SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}")
SET(MACOSX_BUNDLE_BUNDLE_VERSION "${VERSION}")
SET(MACOSX_BUNDLE_COPYRIGHT "Tobias Doerffel, 2008")

INCLUDE(CPack)

