# Search Engine

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.21+-brightgreen.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Профессиональная поисковая система на C++ с поддержкой инвертированного индекса и ранжированием результатов по TF-IDF.

## Описание проекта

**Search Engine** - это высокопроизводительная поисковая система, которая демонстрирует принципы работы современных поисковых движков.

**Основные возможности:**
- Индексирование текстовых документов с использованием инвертированного индекса
- Поиск по запросам с ранжированием результатов по релевантности (TF-IDF)
- Многопоточная обработка документов для ускорения индексации
- Работа с конфигурационными файлами в формате JSON
- Полностью покрыта модульными тестами

## Стек технологий

- **Язык программирования:** C++17
- **Система сборки:** CMake
- **Библиотеки:**
    - `nlohmann/json` - для работы с JSON
    - `Google Test` - для модульного тестирования
- **Многопоточность:** `std::thread`, `std::mutex`
- **Инструменты разработки:** CLion, Visual Studio Code

## Архитектура проекта

- CMakeLists.txt
- main.cpp
- README.md, .gitignore, LICENSE
- config.json, requests.json
- ConverterJSON/ (h/cpp)
- InvertedIndex/ (h/cpp)
- SearchServer/ (h/cpp)
- tests/ (test_main.cpp)
- resources/ (txt files)


## Быстрый старт

### Предварительные требования

- **Компилятор C++** с поддержкой C++17
- **CMake** версии 3.21 или выше
- **Git**

### Установка и запуск

1. **Клонируйте репозиторий**
```bash
git clone https://github.com/your-username/search_engine.git
cd search_engine
```

2. **Соберите проект**
```bash
# Создайте директорию для сборки
mkdir build && cd build

# Настройте проект с помощью CMake
cmake ..

# Соберите проект
cmake --build . --config Release
```

3. **Настройте конфигурацию**

Перед запуском убедитесь, что в корне проекта есть необходимые JSON файлы:

**`config.json`** - настройки поискового движка:
```json
{
    "config": {
        "name": "SearchEngine",
        "version": "1.0.0",
        "max_responses": 5
    },
    "files": [
        "resources/file001.txt",
        "resources/file002.txt",
        "resources/file003.txt"
    ]
}
```

**`requests.json`** - поисковые запросы:
```json
{
    "requests": [
        "первый поисковый запрос",
        "второй запрос для тестирования", 
        "третий пример"
    ]
}
```

4. **Запустите приложение**
```bash
# Из директории build
./search_engine

# Или если вы используете Windows
./search_engine.exe
```

5. **Запустите тесты** (опционально)
```bash
./search_engine --test
```
## Итоговая структура файлов:

- README.md
- CMakeLists.txt
- main.cpp
- .gitignore
- LICENSE
- config.json
- requests.json
- ConverterJSON/
- InvertedIndex/
- SearchServer/
- tests/
- resources/