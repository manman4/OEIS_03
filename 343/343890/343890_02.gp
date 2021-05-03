M=8;

row(n) = Vecrev(n!*pollaguerre(n, n));
for(n=0, M, print(row(n)));