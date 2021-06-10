##############################
# SegmentedFusion
##############################
set( PROJ_NAME SegmentedFusion )
project( ${PROJ_NAME} )

# internal
file(GLOB PROJ_SOURCE *.*)

# shared

# target
add_executable(
	${PROJ_NAME}
    ${PROJ_SOURCE}
    ${PROJ_SHARED_DATADIR_INCLUDE}
)

target_link_libraries(
	${PROJ_NAME}
    ${OpenCV_LIBS}
	${OpenCL_LIBRARIES}
	${OPENGL_LIBRARY}
)