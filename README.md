# Spreadsheet

Данный проект представляет собой прототип электронной таблицы (по типу Excel или Calc). Основной функционал: ввод данных ячеек с использованием внешней библиотеки ANTLR для парсинга формулы (поддерживаются строки, числа и формулы), вывод данных ячеек по отдельности, вывод таблицы целиком. Частные случаи, такие как деление на 0 и возникновение циклической зависимости между ячейками, учтены. Также используется технология кэширования данных для оптимизации.

## Требования
* C++17 и выше
* [Java SE Runtime Environment 8](https://www.oracle.com/java/technologies/javase-jre8-downloads.html)
* [ANTLR](https://www.antlr.org/)

## Порядок сборки
1. Установить [Java SE Runtime Environment 8](https://www.oracle.com/java/technologies/javase-jre8-downloads.html).
2. Установить [ANTLR](https://www.antlr.org/), выполнив все пункты в меню Quick Start.
3. В файлах FindANTLR.cmake и CMakeLists.txt указать корректное название файла antlr-X.X.X-complete.jar (вместро X.X.X необходимо указать соответствующую версию).
4. Создать папку с названием "antlr4_runtime" и скачать в неё [файлы](https://github.com/antlr/antlr4/tree/master/runtime/Cpp).
5. Запустить cmake build с CMakeLists.txt.
