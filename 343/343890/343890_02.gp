M=8;

row(n) = Vecrev(n!*pollaguerre(n, n+1));
for(n=0, M, print(row(n)));