set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_CXX_FLAGS -Wno-c++98-compat) #Don't care
set(CMAKE_CXX_FLAGS -Wno-microsoft-extra-qualification) #I *want* my namespaced functions to be qualified, dangit

set(H_PROJECT_NAME
        Hyengine
)
set(H_PROJECT_VERSION
        0.0.1
)
set(H_PROJECT_DESCRIPTION
        "Hyeve's App Engine"
)

set(TRACY_ENABLE OFF)
set(TRACY_ON_DEMAND ON)
set(TRACY_ONLY_LOCALHOST ON)
set(TRACY_NO_CODE_TRANSFER ON)
set(TRACY_DELAYED_INIT ON)
