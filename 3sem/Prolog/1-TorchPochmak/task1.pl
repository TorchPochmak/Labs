
% Горин Никита, М8О-213Б-22
% Номер по списку 5
% Задание 1

% Длина
mylen([], 0).
mylen([_|T], N) :- mylen(T, N1), N is N1+1.

% Проверка на принадлежность элемента X
mymember(X, [X|_]).
mymember(X, [_|T]) :-
  mymember(X, T).

% Конкатенация двух списков
myappend([], X, X).
myappend([H|T1], X, [H|T2]) :-
  myappend(T1, X, T2).

% Удаление элемента
myremove(X, [X|T], T).
myremove(X, [H|T1], [H|T2]) :-
  myremove(X, T1, T2).

% Перестановка элементов
mypermute([], []).
mypermute(X, [H|T1]) :-
  myremove(H, X, T),
  mypermute(T, T1).

% Подсписок
mysublist(S, R) :-
  myappend(X, _, S),
  myappend(_, R, X).

% Часть 1. Вариант 6 - Удаление N последних элементов
% Удаление(число элементов, входной список, выходной список)

% С использованием стандартных предикатов
myremove_n_stand(0, List, List).
myremove_n_stand(N, List, Result) :- N > 0, length(Deleted, N), append(Result, Deleted, List).

% Без использования стандартных предикатов
myremove_n_nostand(0, X, X).

myremove_n_nostand(1, [_], []).
myremove_n_nostand(1, [X|Xs], [X|Result]) :-
  myremove_n_nostand(1, Xs, Result).

myremove_n_nostand(N, Input, L) :- 
  N > 1,
  N1 is N - 1,
  myremove_n_nostand(1, Input, Res),
  myremove_n_nostand(N1, Res, L).

% Часть 2. Вариант 11 - Вычисление позиции максимального элемента в списке
% Поиск элемента(список, найденная позиция)

% С использованием стандартных предикатов
index_of_max_stand(L, Index):-
  max_list(L, El), 
  nth0(Index, L, El), !.

% Без использования стандартных предикатов
%-----------------------------------------
index_of_max_nostand(L,Index):-
  get_max(L,Max),
  index_of(L,0,Index,Max). 
%-----------------------------------------
compare_max(X,Y,Z) :- atomic(X), atomic(Y), (X >= Y, Z is X); (X < Y, Z is Y).
%-----------------------------------------
get_max([H|T], Max) :-
  get_max(T, H, Max).

get_max([], Max, Max).
get_max([H|T], MaxLast, Max):-
    compare_max(H, MaxLast, MaxTemp),
    get_max(T, MaxTemp, Max).
%-----------------------------------------
index_of([Max|_], I, I, Max).
index_of([_|T], CurInd, IndMax, Max):-
    NewInd is CurInd + 1,
    index_of(T, NewInd, IndMax, Max).
%-----------------------------------------

% Часть 3.
% Нахождение индекса максимального элемента и удаление всех элементов списка после него
delete_after_max(L, Result) :-
  index_of_max_stand(L, MaxIndex),
  mylen(L, AllLength),
  LenToDelete is AllLength - MaxIndex - 1,
  myremove_n_stand(LenToDelete, L, Result).
