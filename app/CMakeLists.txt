qt_standard_project_setup()

qt_add_executable(ElektroRechner WIN32 MACOSX_BUNDLE
    main.cpp
    CalcBridge.h
    CalcBridge.cpp
    UiStrings.h
    UiStrings.cpp
)

qt_add_qml_module(ElektroRechner
    URI ElektroRechnerUi
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        qml/Main.qml
        qml/HomePage.qml
        qml/ListPage.qml
        qml/CalculatorPage.qml
        qml/TablePage.qml
        qml/SettingsPage.qml
        qml/ItemRow.qml
        qml/Card.qml
        qml/InputForm.qml
        qml/ResultView.qml
        qml/ResultBar.qml
        qml/ProjectListPage.qml
        qml/RowEditorPage.qml
        qml/Status.js
        qml/ChartView.qml
        qml/CategoryToggles.qml
)

target_compile_definitions(ElektroRechner PRIVATE ELC_APP_VERSION="${PROJECT_VERSION}")
target_link_libraries(ElektroRechner PRIVATE elc::core Qt6::Quick Qt6::QuickControls2)

# Strict warnings only for our own sources – the QML compiler's generated code would trip -Wshadow.
set(ELC_APP_OWN_SOURCES main.cpp CalcBridge.cpp UiStrings.cpp)
if(MSVC)
    target_compile_options(ElektroRechner PRIVATE /utf-8)
    set_source_files_properties(${ELC_APP_OWN_SOURCES} PROPERTIES COMPILE_OPTIONS "/W4")
else()
    set_source_files_properties(${ELC_APP_OWN_SOURCES} PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra;-Wpedantic;-Wshadow")
endif()

set_target_properties(ElektroRechner PROPERTIES
    OUTPUT_NAME ElektroRechner
    # Android (Qt 6.5+ reads these; older versions ignore them)
    QT_ANDROID_VERSION_NAME ${PROJECT_VERSION}
    QT_ANDROID_VERSION_CODE 11
)

include(GNUInstallDirs)
install(TARGETS ElektroRechner
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
