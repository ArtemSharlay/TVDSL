# HW2 — Парсер Funny

**LL(1)-парсер (рекурсивный спуск) на его основе**

## Структура проекта

```text
dz2/
├── CMakeLists.txt
├── dz2.cpp
├── include/
│   ├── ast/
│   ├── parser/
│   └── dfa/
├── resources/
│   └── lexer.dfa
├── src/
│   ├── parser/
│   └── dfa/
│  
└── test/
    └── parserTest.cpp
```

Ошибки
Режим паники: в блоке пропуск до ; или }, на верхнем уровне — до следующего определения. Парсер собирает все ошибки за один проход и не зацикливается.

## Сборка проекта

### 1. Клонирование репозитория

```bash
git clone https://github.com/ArtemSharlay/TVDSL.git
cd TVDSL
```

Если проект уже находится на компьютере, откройте терминал в корневой папке проекта.

### 2. Конфигурация CMake

```bash
cmake -S . -B build
```

CMake создаст папку `build` и подготовит файлы для сборки проекта.

### 3. Компиляция

**Windows (генератор Visual Studio):**

```bash
cmake --build build --config Debug
```

**Linux / macOS или генераторы с одной конфигурацией:**

```bash
cmake --build build
```

После успешной сборки будут созданы исполняемые файлы `dz2` и `parser_tests`.

## Запуск программы

**Windows (Visual Studio):**

```powershell
.\build\Debug\dz2.exe
```

**Linux / macOS:**

```bash
./build/dz2
```

## Запуск тестов

В проекте предусмотрен набор тестов `parser_tests`, проверяющий работу парсера.

**Windows (Visual Studio):**

```bash
ctest --test-dir build -C Debug --output-on-failure
```

**Linux / macOS или генераторы с одной конфигурацией:**

```bash
ctest --test-dir build --output-on-failure
```
