% Горин Никита, М8О-213Б-22
% Номер по списку 5
% Задание 2

% Вариант представления данных - 2 (two.pl)
% Вариант задания - 2

% Вариант 2
%     - Напечатать средний балл для каждого предмета
%     - Для каждой группы, найти количество не сдавших студентов 
%     - Найти количество не сдавших студентов для каждого из предметов

:- ['two.pl'].

% Сумма элементов списка
sum_list([], 0).
sum_list([H|T], Result):-
    sum_list(T, Res1), Result is Res1 + H.

% Cредний балл для каждого предмета
% средняя_оценка(предмет, результат)
average_mark(Subject, Result):-
    findall(M, grade(_, _, Subject, M), AllMarks),
    sum_list(AllMarks, Sum),
    length(AllMarks, Len),
    Result is Sum / Len.

% Количество не сдавших студентов для группы
% не_сдавшие_студенты_группа(группа, результат)

failed_students_group(Group, Result):-
    findall(St, grade(Group, St, _, 2), L),
    length(L, Result).

% Количество не сдавших студентов для предмета
% не_сдавшие_студенты_предмет(предмет, результат)

failed_students_subject(Subject, Result):-
    findall(St, grade(_, St, Subject, 2), L),
    length(L, Result).
