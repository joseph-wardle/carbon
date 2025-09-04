add_library(carbon STATIC)
target_compile_features(carbon INTERFACE cxx_std_23)

target_sources(carbon
    PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES FILES
            core.cppm
            math.cppm
            option.cppm
            random.cppm
            result.cppm
            string.cppm
            string_view.cppm
            time.cppm
            types.cppm
)