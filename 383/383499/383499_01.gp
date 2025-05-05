M=29;

\\ a(n) = Sum_{k=0..floor(n/2)} (2*k+1) * binomial(2*k,k) * binomial(n-k+1,k+1).
a(n) = sum(k=0, n/2, (2*k+1) * binomial(2*k, k) * binomial(n-k+1, k+1));
for(n=0, M, print1(a(n), ", "));
