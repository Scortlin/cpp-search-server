# cpp-search-server (учебный проект курса Яндекс Практикум C++)
Финальный проект: поисковый сервер

# Основные функции:

ранжирование результатов поиска по статистической мере TF-IDF;
обработка стоп-слов (не учитываются поисковой системой и не влияют на результаты поиска);
обработка минус-слов (документы, содержащие минус-слова, не будут включены в результаты поиска);
создание и обработка очереди запросов;
удаление дубликатов документов;
постраничное разделение результатов поиска;
возможность работы в многопоточном режиме;
# Принцип работы
Создание экземпляра класса SearchServer. В конструктор передаётся строка с стоп-словами, разделенными пробелами. Вместо строки можно передавать произвольный контейнер (с последовательным доступом к элементам с возможностью использования в for-range цикле)

С помощью метода AddDocument добавляются документы для поиска. В метод передаётся id документа, статус, рейтинг, и сам документ в формате строки.

Метод FindTopDocuments возвращает вектор документов, согласно соответствию переданным ключевым словам. Результаты отсортированы по статистической мере TF-IDF. Возможна дополнительная фильтрация документов по id, статусу и рейтингу. Метод реализован как в однопоточной так и в многопоточной версии.

Класс RequestQueue реализует очередь запросов к поисковому серверу с сохранением результатов поиска.

# Сборка и установка
Сборка с помощью любой IDE либо сборка из командной строки

# Системные требования
Компилятор С++ с поддержкой стандарта C++17 или новее
