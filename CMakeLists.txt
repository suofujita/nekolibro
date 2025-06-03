cmake_minimum_required(VERSION 3.16)

project(NekoLibro VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SMTPCLIENT_SRC "SmtpClient-for-Qt/src/*.cpp")
file(GLOB SMTPCLIENT_HDR "SmtpClient-for-Qt/src/*.h")


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

set(PROJECT_SOURCES
        main.cpp
        nekolibro.cpp
        nekolibro.h
        nekolibro.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NekoLibro
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        saleswindow.h saleswindow.cpp saleswindow.ui
        employeeswindow.h employeeswindow.cpp employeeswindow.ui
        categorieswindow.h categorieswindow.cpp categorieswindow.ui

        login.h login.cpp login.ui
        galeries.qrc
        flashscreen.h flashscreen.cpp flashscreen.ui
        database.h database.cpp
        createaccount.h createaccount.cpp createaccount.ui
        imexport.h imexport.cpp imexport.ui
        resetpassword.h resetpassword.cpp resetpassword.ui
        viewinvoicesdetails.h viewinvoicesdetails.cpp viewinvoicesdetails.ui
        reports.h reports.cpp reports.ui
        settings.h settings.cpp settings.ui
        accountswindow.h accountswindow.cpp accountswindow.ui

        ${SMTPCLIENT_SRC}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET NekoLibro APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(NekoLibro SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(NekoLibro
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Thêm thư mục chứa header của thư viện SmtpClient-for-Qt vào đường dẫn bao gồm


target_include_directories(NekoLibro PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SmtpClient-for-Qt/src)
target_link_libraries(NekoLibro PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(NekoLibro PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(NekoLibro PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(NekoLibro PRIVATE Qt${QT_VERSION_MAJOR}::Charts)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.NekoLibro)
endif()
set_target_properties(NekoLibro PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS NekoLibro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NekoLibro)
endif()
