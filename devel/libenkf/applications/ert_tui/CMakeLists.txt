include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${PLPLOT_HEADER} )

set( SITE_CONFIG_FILE /project/res/etc/ERT/site-config CACHE FILEPATH "Path to global ERT Configuration file")

set( src_list main.c enkf_tui_main.c  enkf_tui_fs.c  enkf_tui_ranking.c  enkf_tui_misc.c  enkf_tui_table.c  enkf_tui_plot.c enkf_tui_plot_rft.c enkf_tui_plot_util.c
              enkf_tui_run.c enkf_tui_util.c  enkf_tui_init.c  enkf_tui_export.c  enkf_tui_analysis.c enkf_tui_QC.c enkf_tui_help.c enkf_tui_simple.c ert_tui_jobs.c enkf_tui_workflow.c)

#exec_program( svnversion ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE SVN_VERSION)
#exec_program( date OUTPUT_VARIABLE COMPILE_TIME_STAMP)

#exec_program( svnversion ${PROJECT_SOURCE_DIR} OUTPUT_VARIABLE SVN_VERSION)
#exec_program( date OUTPUT_VARIABLE COMPILE_TIME_STAMP)
#file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h "#define SVN_VERSION  \"${SVN_VERSION}\"\n#define COMPILE_TIME_STAMP   \"${COMPILE_TIME_STAMP}\"\n" )


#add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/timestamp.cmake )
#add_custom_target( timestamp ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/build_timestep.h )
#set_source_files_properties( ${CMAKE_CURRENT_BINARY_DIR}/build_timestamp.h
#                              PROPERTIES GENERATED TRUE 
#                              HEADER_FILE_ONLY TRUE)


execute_process(COMMAND date "+%Y-%m-%d %H:%M:%S" OUTPUT_VARIABLE BUILD_TIME )
string(STRIP ${BUILD_TIME} BUILD_TIME)

# The original commit here used git -C; which might be an improvement
# but on git version 1.83 that switch is not recognized.
find_package(Git)
if(GIT_EXECUTABLE)
   execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                   OUTPUT_VARIABLE GIT_COMMIT)
   string(STRIP "${GIT_COMMIT}" GIT_COMMIT)
else()
   set(GIT_COMMIT)
endif()

set_source_files_properties( main.c PROPERTIES COMPILE_DEFINITIONS "COMPILE_TIME_STAMP=\"${BUILD_TIME}\";GIT_COMMIT=\"${GIT_COMMIT}\";SITE_CONFIG_FILE=\"${SITE_CONFIG_FILE}\"")

add_executable( ert ${src_list} )
target_link_libraries( ert enkf sched rms ecl config plot job_queue analysis ert_util )
if (USE_RUNPATH)
   add_runpath( ert )
endif()

if (ERT_INSTALL_PREFIX)
   set (destination ${CMAKE_INSTALL_PREFIX}/${ERT_INSTALL_PREFIX}/bin)
else()
   set (destination ${CMAKE_INSTALL_PREFIX}/bin)
endif()


if (INSTALL_ERT)
   install(TARGETS ert DESTINATION ${destination})
   if (INSTALL_GROUP)
      install(CODE "EXECUTE_PROCESS(COMMAND chgrp ${INSTALL_GROUP} ${destination}/ert)")
      install(CODE "EXECUTE_PROCESS(COMMAND chmod g+w ${destination}/ert)")
   endif()
endif()
