### Требования к функционалу таблицы

Посмотрим на интерфейс таблицы `SheetInterface`.
В этом задании вам будет нужно реализовать только часть из этих
методов. Но для того, чтобы определиться со структурой данных, важно
учесть все требования к функционалу.

### Методы, обращающиеся к ячейке по индексу:

- `SetCell(Position, std::string)` задаёт содержимое ячейки по индексу `Position`. Если ячейка пуста, надо её создать. Нужно задать ячейке текст методом `Cell::Set(std::string)`;
- `Cell* GetCell(Position pos)` константный и неконстантный геттеры, которые возвращают указатель на ячейку, расположенную по индексу `pos`. Если ячейка пуста, возвращают `nullptr`;
- `void ClearCell(Position pos)` очищает ячейку по индексу. Последующий вызов `GetCell()` для этой ячейки вернёт `nullptr`. При этом может измениться размер минимальной печатной области. Если в
  примере выше удалить ячейку В5, минимальная печатная область поменяется и будет выглядеть так:

### Методы, применимые к таблице целиком:

`Size GetPrintableSize()` определяет размер минимальной печатной области. Специально для него в файле common.h и определена структура `Size`. Она содержит количество строк и столбцов в минимальной печатной области.

Печать таблицы выводит в стандартный поток вывода `std::ostream&` минимальную прямоугольную печатную область. Ячейки из одной строки должны быть разделены табуляцией `\t`, в конце строки добавьте символ перевода строки `\n`.

- `void PrintText(std::ostream&)` выводит текстовые представления ячеек:
    - для текстовых ячеек это текст, который пользователь задал в методе `Set()`, то есть не очищенный от ведущих апострофов `'`;
    - для формульных это формула, очищенная от лишних скобок, как `Formula::GetExpression()`, но с ведущим знаком “=”.

  Обратите внимание, что в формуле 1+2*3 пропали лишние скобки, а в текстовых ячейках в третьей строке сохранились апострофы.

### Задание

Реализуйте структуру данных «Электронная таблица». Интерфейс таблицы `SheetInterface` вы найдёте в файле common.h.

common.h содержит интерфейсы классов `FormulaInterface`, `CellInterface`, объявление структур `Size` и `Position`, описание исключений и ошибок. Интерфейс `SheetInterface` не содержит методов удаления и вставки. Реализовывать их не нужно.

Для размера минимальной печатной области в файле common.h определена структура `Size`. В файле  structures.cpp из решения первой задачи реализуйте `operator=` для этой структуры.

### Ограничения

Вносите изменения только в файлы sheet.h, sheet.cpp и structures.cpp. В файле structures.cpp реализуйте `Size::operator=()`.

### Как будет тестироваться ваш код

Протестируем
только файлы sheet.h и sheet.cpp. Мы не будем тестировать остальные
файлы. Примеры тестов вы найдёте в файле main.cpp.

Мы
будем проводить нагрузочные тесты: создавать/удалять большое количество
ячеек много раз и проверять время работы и использование памяти.

Проверим, что минимальная видимая область находится корректно.

Также будем проверять все функции интерфейса `SheetInterface`.

Скопируйте содержимое файлов `main.cpp` и `common.h` ниже и вставьте в своё решение.

+

Разберёмся с ограничениями на производительность. Метод `GetCell()`
должен отрабатывать за константное время, в то время как ограничения на
время работы для вставки и удаления столбцов/строк самые либеральные.
Значит, операции произвольного доступа будут вызываться чаще, чем
операции вставки.

Наверно, имеет смысл использовать простую структуру данных вроде вектора векторов.