# Создаем исполняемый файл для тестов, перечисляя все исходники
add_executable(run_tests
        test_sequence.cpp
        test_array_sequence.cpp
        test_list_sequence.cpp
        test_bit_sequence.cpp
        test_dynamic_array.cpp
        test_linked_list.cpp
)

# Подключаем к тестам нашу библиотеку (bit_lib дает и seq_lib)
# и GTest с готовой функцией main()
target_link_libraries(run_tests
        PRIVATE
        bit_lib
        GTest::gtest_main
)

# Автоматически находим все макросы TEST() и регистрируем их в CTest
gtest_discover_tests(run_tests)