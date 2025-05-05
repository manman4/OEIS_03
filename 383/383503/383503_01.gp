M=33;

\\ a(n) = Sum_{k=0..floor(n/3)} (2*k+1) * binomial(2*k,k) * binomial(n-2*k+1,k+1).
a(n) = sum(k=0, n/3, (2*k+1) * binomial(2*k, k) * binomial(n-2*k+1, k+1));
for(n=0, M, print1(a(n), ", "));
