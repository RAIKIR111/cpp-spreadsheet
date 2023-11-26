# Spreadsheet

Данный проект представляет собой прототип электронной таблицы (по типу Excel или Calc). Основной функционал: ввод данных ячеек с использованием внешней библиотеки ANTLR для парсинга формулы (поддерживаются строки, числа и формулы), вывод данных ячеек по отдельности, вывод таблицы целиком. Частные случаи, такие как деление на 0 и возникновение циклической зависимости между ячейками, учтены. Также используется технология кэширования данных для оптимизации.

## Требования
* C++17 и выше
* [Java SE Runtime Environment 8](https://www.oracle.com/java/technologies/javase-jre8-downloads.html)
* [ANTLR](https://www.antlr.org/)

## Порядок сборки
1. Установить [Java SE Runtime Environment 8](https://www.oracle.com/java/technologies/javase-jre8-downloads.html).
2. Установить [ANTLR](https://www.antlr.org/) (ANother Tool for Language Recognition), выполнив все пункты в меню Quick Start.
  *В случае установки на Windows может быть полезно данное [видео](https://youtu.be/p2gIBPz69DM).*
3. Проверить в файлах FindANTLR.cmake и CMakeLists.txt название файла antlr-X.X.X-complete.jar на корректность версии. Вместо "X.X.X" указать свою версию antlr.
4. Создайть папку с названием "antlr4_runtime" без кавычек и скачайть в неё [файлы](https://github.com/antlr/antlr4/tree/master/runtime/Cpp).
5. Запустить cmake build с CMakeLists.txt.
