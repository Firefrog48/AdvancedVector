### Описание проекта на GitHub

---

**Название проекта:** Улучшенный вектор (Enhanced Vector)

**Описание:**

Проект "Улучшенный вектор" представляет собой учебный проект, разработанный в рамках программы Яндекс.Практикум. Этот проект направлен на создание аналога стандартного контейнера `std::vector` с улучшенной функциональностью и оптимизированной производительностью.

**Основные особенности и функционал:**

1. **Шаблонный класс `RawMemory`:**
   - Хранит буфер, вмещающий заданное количество элементов.
   - Предоставляет доступ к элементам по индексу.

2. **Конструкторы:**
   - **Конструктор по умолчанию:** Инициализирует вектор нулевого размера и вместимости. Не выбрасывает исключений. Алгоритмическая сложность: O(1).
   - **Конструктор заданного размера:** Создаёт вектор с элементами, инициализированными значением по умолчанию для типа `T`. Устойчив к исключениям. Алгоритмическая сложность: O(размер вектора).
   - **Копирующий конструктор:** Создаёт копию элементов исходного вектора. Выделяет память без запаса. Устойчив к исключениям. Алгоритмическая сложность: O(размер исходного вектора).
   - **Перемещающий конструктор:** Выполняется за O(1) и не выбрасывает исключений.

3. **Деструктор:**
   - Разрушает элементы вектора и освобождает занимаемую ими память. Алгоритмическая сложность: O(размер вектора).

4. **Вспомогательные методы:**
   - `Size` для получения количества элементов.
   - `Capacity` для получения вместимости вектора.
   - Статические методы `Allocate` и `Deallocate` для выделения и освобождения сырой памяти.

5. **Операторы и методы:**
   - **Оператор `[]`:** Доступ к элементу по индексу (две версии — константная и неконстантная).
   - **Метод `Reserve`:** Задаёт ёмкость вектора, повышая эффективность при известном количестве элементов.
   - **Операторы присваивания:** Копирующее и перемещающее присваивание.
   - **Метод `Swap`:** Обмен содержимым с другим вектором.
   - **Методы `Resize`, `PushBack`, `PopBack`:** Изменение размера, добавление и удаление элементов.
   - **Методы `EmplaceBack` и `Insert`:** Вставка элементов с гарантиями безопасности исключений.
   - **Метод `Emplace`:** Передаёт параметры конструктору элемента с использованием perfect forwarding.
   - **Метод `Erase`:** Удаление элемента по переданному итератору.

**Сборка и установка:**
  - Возможна сборка с помощью любой IDE либо из командной строки.

**Системные требования:**
  - Компилятор C++ с поддержкой стандарта C++17 или новее.

