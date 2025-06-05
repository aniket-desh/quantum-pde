cmake_minimum_required(VERSION 3.18)
project(qps LANGUAGES CXX)

# ─────────────────────────────────────────────────────────────
# 1.  Eigen ─ choose ONE of the two paths
# ─────────────────────────────────────────────────────────────
# (A) System install (apt, brew, vcpkg, …)
find_package(Eigen3 3.4 REQUIRED CONFIG)     # looks for Eigen3::Eigen target
# (B) Header-only download at configure time
# include(FetchContent)
# FetchContent_Declare(
#   eigen
#   GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
#   GIT_TAG        3.4.0       # pin for reproducibility
# )
# FetchContent_MakeAvailable(eigen)

# ─────────────────────────────────────────────────────────────
# 2.  BLAS/LAPACK (OpenBLAS)
# ─────────────────────────────────────────────────────────────
set(OpenBLAS_INCLUDE_DIR /opt/homebrew/opt/openblas/include)
set(OpenBLAS_LIBRARY /opt/homebrew/opt/openblas/lib/libopenblas.dylib)

if(NOT EXISTS ${OpenBLAS_LIBRARY})
    message(FATAL_ERROR "OpenBLAS library not found at ${OpenBLAS_LIBRARY}")
endif()

add_compile_definitions(EIGEN_USE_BLAS)    # tells Eigen to route to BLAS/LAPACK

# ─────────────────────────────────────────────────────────────
# 3.  Build your static library and demo executable
# ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE QPS_SRC CONFIGURE_DEPENDS src/*.cpp src/*.cc)
add_library(qps STATIC ${QPS_SRC})
target_include_directories(qps PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(qps
    PUBLIC
        Eigen3::Eigen
        ${OpenBLAS_LIBRARY}   # fast SVD/GEMM
)

target_compile_features(qps PUBLIC cxx_std_17)  # Tensor module needs ≥C++14
target_compile_options(qps PRIVATE -O3 -march=native -ffast-math)

# Demo executable
add_executable(demo src/main.cc)
target_link_libraries(demo PRIVATE qps)

enable_testing()
add_subdirectory(tests)                      # GoogleTest or Catch2