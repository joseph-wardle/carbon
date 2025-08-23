set(PROJECT_NAME "carbon_core")

add_library(${PROJECT_NAME} STATIC)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

target_sources(carbon_core
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
            types.cppm
            string_view.cppm
)