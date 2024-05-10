set(TARGET "app")

set(SOURCES
    main.cpp
    model/mainmodel.h
    model/mainmodel.cpp
    datamodel/abstractdatamodel.h
    datamodel/datamodel.h
    datamodel/dataprovider.h
    datamodel/datamodel.cpp
    datamodel/dataprovider.cpp
    taskrunner/taskrunner.h
    taskrunner/compressortask.h
    taskrunner/taskrunner.cpp
    taskrunner/compressortask.cpp)

include_directories(model datamodel ../compress)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Concurrent)

qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(app ${SOURCES})

qt_add_qml_module(
  app
  URI
  compress
  VERSION
  1.0
  QML_FILES
  Main.qml)

add_subdirectory(assets)

set_target_properties(
  app
  PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

target_link_libraries(app PRIVATE Qt6::Quick Qt6::Concurrent compress
                                  assetsplugin)

install(
  TARGETS app
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
