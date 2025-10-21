set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ImGui)

add_library(rlImGui EXCLUDE_FROM_ALL
        rlImGui.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
)


target_include_directories(rlImGui PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${IMGUI_DIR})

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${IMGUI_DIR}
)


target_link_libraries(rlImGui PRIVATE raylib)