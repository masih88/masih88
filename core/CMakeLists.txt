add_library(elc_core STATIC
    src/types.cpp
    src/registry.cpp
    src/formulas/basics.cpp
    src/formulas/cables.cpp
    src/formulas/motors.cpp
    src/formulas/automation.cpp
    src/formulas/electronics.cpp
    src/formulas/units.cpp
    src/formulas/cleanroom.cpp
    src/formulas/pv.cpp
    src/formulas/air.cpp
    src/formulas/datacenter.cpp
    src/catalog/basics.cpp
    src/catalog/hvac.cpp
    src/catalog/cleanroom.cpp
    src/catalog/datacenter.cpp
    src/catalog/pv.cpp
    src/catalog/cables.cpp
    src/catalog/motors.cpp
    src/catalog/industry.cpp
    src/catalog/automation.cpp
    src/catalog/electronics.cpp
    src/catalog/converters.cpp
    src/tables/tables.cpp
    src/tables/vdi_tables.cpp
    src/lists/lists.cpp
    src/lists/loadlist.cpp
    src/lists/gapoints.cpp
)
add_library(elc::core ALIAS elc_core)

target_include_directories(elc_core
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(elc_core PUBLIC cxx_std_20)
target_link_libraries(elc_core PRIVATE elc_warnings)
target_compile_definitions(elc_core PRIVATE ELC_VERSION="${PROJECT_VERSION}")
set_target_properties(elc_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
