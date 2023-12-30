movement(From,To):-
	append(List,['_','w'|T],From),
	append(List,['w','_'|T],To).
movement(From,To):-
	append(List,['b','_'|T],From),
	append(List,['_','b'|T],To).
movement(From,To):-
	append(List,['_','b','w'|T],From),
	append(List,['w','b','_'|T],To).
movement(From,To):-
	append(List,['b','w','_'|T],From),
	append(List,['_','w','b'|T],To).

prolong([X|T],[Y,X|T]):-
	movement(X,Y),
	not(member(Y,[X|T])).

dfs([X|T],X,[X|T]).
dfs(P,X,R):-
	prolong(P,O),
	dfs(O,X,R).

dfs_search(From,To):-

	get_time(TimeStart),
	dfs([From],To,Sub),
	get_time(TimeEnd),

	Time is TimeEnd - TimeStart,

	print(Sub),nl, nl,
	length(Sub, Len),write('Count:'),writeln(Len),
	write('Time:'),writeln(Time),nl.

bfs([[X|T]|_],X,[X|T]).
bfs([P|QI],X,R):-
	findall(W,prolong(P,W),T),
	append(QI,T,QO),!,
	bfs(QO,X,R).
bfs([_|T],To,L):- bfs(T,To,L).

bfs_search(From,To):-

	get_time(TimeStart),
	bfs([[From]],To,Sub),
	get_time(TimeEnd),

	Time is TimeEnd - TimeStart,

	print(Sub),nl, nl,
	length(Sub, Len),write('Count:'),writeln(Len),
	write('Time:'),writeln(Time),nl.

iter([X|T],X,[X|T]).
iter(P,X,R,N):-
	prolong(P,W),
	N1 is N-1,
	iter(W,X,R,N1).

iter_search(From,To):-

	get_time(TimeStart),
	iter([From],To,Sub),
	get_time(TimeEnd),

	Time is TimeEnd - TimeStart,

	print(Sub),nl, nl,
	length(Sub, Len),write('Count:'),writeln(Len),
	write('Time:'),writeln(Time),nl.

print([_]):-!.
print([B|T]):-
	print(T),nl,write(B).

solve():-
	From = ['b', 'b', 'b', 'b', '_', 'w', 'w', 'w'],
	To = ['w', 'w', 'w', '_', 'b', 'b', 'b', 'b'],

	writeln('Iterative'),
	iter_search(From, To),

	writeln('DFS'),
	dfs_search(From, To),

	writeln('BFS'),
	bfs_search(From,To),
        !.