# Project Name
Project Description

### Column Name
- [x] Добавить документы и открытие из файла и сохраниение в файл
    - [x] Создание нового документа
    - [x] Сохранение документа
    - [x] Открытие документа
    - [x] Проверка изменения файла при действиях (запрос при сохранении)
- [ ] Изменить хранение Vertex
    - [ ] Вместо `add_position/add_transition` сделать `add_vertex(VertexType)`
    - [ ] Вместо `insert_position/insert_transition` сделать `insert_vertex(Vertex)`
    - [ ] Вместо `remove_position/remove_transition` сделать `remove_vertex(VertexIndex)`
- [ ] Добавить в основное дерево проекта
    - [ ] Для `NetTreeItem/LinearBaseFragmentsItem/PrimitiveItem` добавить просмотр в виде:
        - [ ] Матрицы инцидентности
        - [ ] Матрицы инцидентности + `I -> P/T`
    - [ ] Для SynthesisProgramItem добавить:
        - [ ] Сопоставить запись в таблицы программ и элемент дерева с 
      помощью индекса `ProgramIndex`
        - [ ] Синтезированная программа неизменяема
        - [ ] Проверку на существование программы (дубли)
        - [ ] Просмотр уравнений синтеза
        - [ ] Просмотр графа до удаления эквивалентных позиций
- [ ] Создать окно которое закрывается при снятии фокуса
- [ ] Для всех `GraphicScene` добавить:
    - [ ] Возможность сохранения в файл (SVG)
    - [ ] Действия:
        - [ ] Fit in view
        - [ ] Fit selection

### Completed Column ✓
- [x] Completed task title  