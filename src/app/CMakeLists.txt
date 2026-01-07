set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(personal-calendar
    main.cpp
    CalendarApp.cpp
    models/EventsModel.cpp
    resources.qrc
)

target_link_libraries(personal-calendar
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    personalcalendar-core
    personalcalendar-local
)

target_include_directories(personal-calendar PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

install(TARGETS personal-calendar RUNTIME DESTINATION bin)
