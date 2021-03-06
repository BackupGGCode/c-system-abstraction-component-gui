
foreach(flag_var
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO
        )
   if(${flag_var} MATCHES "/MD")
      string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "/MD")
   if(${flag_var} MATCHES "-br")
      string(REGEX REPLACE "-br" "" ${flag_var} "${${flag_var}}")
   endif(${flag_var} MATCHES "-br")
endforeach(flag_var)

if( WATCOM )
string( REGEX REPLACE "library clbrdll.lib" "" CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}" )
string( REGEX REPLACE "library plbrdll.lib" "" CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES}" )
endif( WATCOM )

set( SOURCES_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../ )


include( ${SOURCES_ROOT}/CMakeSources.lst )

set(BASE_SOURCES
    ${NETWORK_SOURCES}
    ${TYPE_LIBRARY_SOURCES}
    ${UNSORTED_SOURCES}
    ${SYSTEM_LIBRARY_SOURCES}
    ${ExtraSources}
    ${FILESYSTEM_SOURCES}
)

set( PROJECT scan_ports.portable )
add_portable_program( ${PROJECT} ${FIRST_GCC_PROGRAM_SOURCE} 
../scanner 
${BASE_SOURCES}
${LAST_GCC_PROGRAM_SOURCE}
)
SET_PROPERTY(TARGET ${PROJECT} APPEND PROPERTY COMPILE_DEFINITIONS "__STATIC__;SACK_BAG_EXPORTS;__NO_OPTIONS__;__NO_DEFAULT_INTERFACES__;" )
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES FOLDER "Network Examples" )

if( NOT __ARM__ )

set( PROJECT scan_addresses.portable )
add_portable_program( ${PROJECT} ${FIRST_GCC_PROGRAM_SOURCE} 
../pinger 
${BASE_SOURCES}
${LAST_GCC_PROGRAM_SOURCE}
)
SET_PROPERTY(TARGET ${PROJECT} APPEND PROPERTY COMPILE_DEFINITIONS "__STATIC__;SACK_BAG_EXPORTS;__NO_OPTIONS__;__NO_DEFAULT_INTERFACES__;" )
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES FOLDER "Network Examples" )

endif( NOT __ARM__ )

set( PROJECT nameof.portable )
add_portable_program( ${PROJECT} ${FIRST_GCC_PROGRAM_SOURCE} 
../nameof 
${BASE_SOURCES}
${LAST_GCC_PROGRAM_SOURCE}
)
SET_PROPERTY(TARGET ${PROJECT} APPEND PROPERTY COMPILE_DEFINITIONS "SACK_BAG_EXPORTS;__NO_GUI__;__NO_OPTIONS__;__NO_DEFAULT_INTERFACES__;__STATIC__" )
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES FOLDER "Network Examples" )


set( PROJECT ipof.portable )
add_portable_program( ${PROJECT} ${FIRST_GCC_PROGRAM_SOURCE} 
../ipof 
${BASE_SOURCES}
${LAST_GCC_PROGRAM_SOURCE}
)
SET_PROPERTY(TARGET ${PROJECT} APPEND PROPERTY COMPILE_DEFINITIONS "SACK_BAG_EXPORTS;__NO_GUI__;__NO_OPTIONS__;__NO_DEFAULT_INTERFACES__;__STATIC__" )
SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES FOLDER "Network Examples" )

