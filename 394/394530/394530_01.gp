M=16;

\\ A(n,k) = k^(n*(k-1)) - Sum_{j=1..k-1} k^(n*(k-1-j)) * j^n * A(n,j).
a(n, k) = k^(n*(k-1))-sum(j=1, k-1, k^(n*(k-1-j))*j^n*a(n, j));
for(n=0, 8, for(k=0, n, print1(a(k, n-k+1),", ")));

