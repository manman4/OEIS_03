M=9;

\\ A(n,k) = ((k+1)*n)!/(k*n)! * (1 + k*n * Sum_{j=1..n} 1/(k*n+j)).
a(n, k) = ((k+1)*n)!/(k*n)! * (1 + k*n * sum(j=1, n, 1/(k*n+j)));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")));


