M=26;
\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n-3*k+1,n-3*k).
a(n) = sum(k=0, n\3, binomial(2*n-3*k+1, n-3*k));
for(n=0, M, print1(a(n),", "));