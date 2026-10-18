HW1 — Лексер Funny: regex → НКА → ДКА → минимизация

Структура проекта

dz1/
├── CMakeLists.txt
├── dz1.cpp
├── include/
├── src/
│   ├── regex/
│   ├── nfa/
│   ├── dfa/
│   └── json/
└── tests/
    └── test.cpp

Сборка проекта

1. Клонирование репозитория

git clone https://github.com/ArtemSharlay/TVDSL.git
cd TVDSL

Если проект уже находится на компьютере, откройте терминал в корневой папке проекта.

2. Конфигурация CMake

Выполните команду:

cmake -S . -B build

CMake создаст папку build и подготовит файлы для сборки.

3. Компиляция

Windows (Visual Studio):

cmake --build build --config Debug

Linux / macOS или генераторы с одной конфигурацией:

cmake --build build
После сборки будут созданы два исполняемых файла: dz1 и lexer_tests.

Запуск программы

Windows (Visual Studio):

.\build\Debug\dz1.exe

Linux / macOS:

./build/dz1

Запуск тестов

В проекте предусмотрены тесты lexer_tests.

Windows (Visual Studio):

ctest --test-dir build -C Debug --output-on-failure

Linux / macOS или генераторы с одной конфигурацией:

ctest --test-dir build --output-on-failure