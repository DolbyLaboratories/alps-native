find_package(Doxygen REQUIRED)
if (NOT DOXYGEN_FOUND)
    message(SEND_ERROR "Doxygen needs to be installed to generate the documentation; disable it with '-DBUILD_DOCUMENTATION=OFF")
    return()
endif (NOT DOXYGEN_FOUND)

doxygen_add_docs(doxygen
    ${PROJECT_SOURCE_DIR}/cli
    ${PROJECT_SOURCE_DIR}/src
    ALL)
