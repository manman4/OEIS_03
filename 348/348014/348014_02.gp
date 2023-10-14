M=6;

row(n) = Vecrev(prod(k=1, n, 1+k^k*x));
for(n=0, M, print1(row(n)); print);