add_executable(elcalc main.cpp)
target_link_libraries(elcalc PRIVATE elc::core elc_warnings)
