file(
    GLOB 
    EXAMPLE_FOLDERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)

foreach(EXAMPLE_FOLDER ${EXAMPLE_FOLDERS})
    if(IS_DIRECTORY ${EXAMPLE_FOLDER})
        get_filename_component(EXAMPLE_FOLDER_NAME ${EXAMPLE_FOLDER} NAME)
        message(STATUS "Adding example: ${EXAMPLE_FOLDER_NAME}")

        set(EXAMPLE_SRC_FILES "")
        set(EXAMPLE_HEADER_FILES "")

        file(
            GLOB 
            EXAMPLE_SRC_FILES
            "${EXAMPLE_FOLDER}/*.c"
        )

        file(
            GLOB 
            EXAMPLE_HEADER_FILES
            "${EXAMPLE_FOLDER}/*.h"
        )

        add_executable(${EXAMPLE_FOLDER_NAME} 
            ${EXAMPLE_SRC_FILES} 
            ${EXAMPLE_HEADER_FILES}
        )

        target_link_libraries(${EXAMPLE_FOLDER_NAME} PRIVATE SimUI)
    endif()
endforeach()