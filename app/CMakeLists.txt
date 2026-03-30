# Создаем исполняемый файл нашего приложения из исходника main.cpp
add_executable(my_app main.cpp)

# Подключаем к нему библиотеку bit_lib (которая автоматически подтянет и seq_lib)
target_link_libraries(my_app PUBLIC bit_lib)