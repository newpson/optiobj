add_executable(app_gui)
target_sources(app_gui
    PRIVATE
    main.cpp
    viewerwindow.cpp
    glwidget.cpp
    PUBLIC
    viewerwindow.h
    glwidget.h)
target_include_directories(app_gui
    PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(app_gui
    PUBLIC
    Qt::Widgets)

