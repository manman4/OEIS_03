M=11;

\\ A(n,k) = (1/n) * (k^n - (k-1)^n + Sum_{d<n and d|n} (-1)^(n/d) * d * A(d,k)).
a(n, k)= (1/n) * (k^n - (k-1)^n + sum(d=1, n-1, (n%d==0) * (-1)^(n/d) * d * a(d, k))); 
for(n=1, M, for(k=1, n, print1(a(k, n-k+1), ", "))); 
