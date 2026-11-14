find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

qt_add_executable(astracare
    main.cpp
    MainWindow.h
    MainWindow.cpp
    widgets/VitalCard.h         widgets/VitalCard.cpp
    widgets/AlertBanner.h       widgets/AlertBanner.cpp
    widgets/BodyTwinWidget.h    widgets/BodyTwinWidget.cpp
    pages/DashboardPage.h       pages/DashboardPage.cpp
    pages/TrackPage.h           pages/TrackPage.cpp
    pages/GuidePage.h           pages/GuidePage.cpp
    pages/AlertsPage.h          pages/AlertsPage.cpp
    pages/ReportsPage.h         pages/ReportsPage.cpp
    pages/EmergencyPage.h       pages/EmergencyPage.cpp
)

target_include_directories(astracare PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(astracare PRIVATE
    Qt6::Widgets
    astracare::core
    astracare::storage
)
