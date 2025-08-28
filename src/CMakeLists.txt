set(PROJECT_NAME "carbon")

add_library(${PROJECT_NAME} STATIC)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

target_sources(carbon
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
            types.cppm
            string_view.cppm
            string.cppm
            random.cppm
            time.cppm
)