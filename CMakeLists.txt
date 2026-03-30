cmake_minimum_required(VERSION 3.14)

# Название проекта
project(CustomCollections LANGUAGES CXX)

# Устанавливаем стандарт C++11 (необходим для noexcept, std::move и т.д.)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================================
# ПОДГОТОВКА ФРЕЙМВОРКА ДЛЯ ТЕСТОВ (Google Test)
# =========================================================
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# Предотвращаем конфликты рантайма на Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Включаем поддержку тестирования в CMake (чтобы работала команда ctest)
enable_testing()
# Подключаем модуль для автоматического поиска тестов (gtest_discover_tests)
include(GoogleTest)

# =========================================================
# ПОДКЛЮЧЕНИЕ ПОДПАПОК ПРОЕКТА
# =========================================================
# 1. Сначала собираем ядро библиотеки (оно нужно всем остальным)
add_subdirectory(core)

# 2. Собираем исполняемое приложение (наш UI в консоли)
add_subdirectory(app)

# 3. Затем собираем тесты (они тоже зависят от ядра)
add_subdirectory(tests)