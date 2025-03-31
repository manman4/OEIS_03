M=21;

\\ a(n) = Sum_{k=0..n} 9^(n-k) * (k+1) * binomial(n-2*k/3-1,n-k).
a(n) = sum(k=0, n, 9^(n-k)*(k+1)*binomial(n-2*k/3-1,n-k));
for(n=0, M, print1(a(n),", "));


