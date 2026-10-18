# HW1 — Лексер Funny

**Регулярные выражения → НКА → ДКА → минимизация**

Проект реализует лексический анализатор для языка Funny. Лексер строится на основе регулярных выражений, которые преобразуются в недетерминированный конечный автомат (НКА), затем в детерминированный конечный автомат (ДКА) с последующей минимизацией.

## Структура проекта

```text
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
```

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

После успешной сборки будут созданы исполняемые файлы `dz1` и `lexer_tests`.

## Запуск программы

**Windows (Visual Studio):**

```powershell
.\build\Debug\dz1.exe
```

**Linux / macOS:**

```bash
./build/dz1
```

## Запуск тестов

В проекте предусмотрен набор тестов `lexer_tests`, проверяющий работу лексического анализатора.

**Windows (Visual Studio):**

```bash
ctest --test-dir build -C Debug --output-on-failure
```

**Linux / macOS или генераторы с одной конфигурацией:**

```bash
ctest --test-dir build --output-on-failure
```
