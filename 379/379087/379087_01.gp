M=21;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*n+k-1,k) * binomial(3*n+k,n-3*k).
a(n) = sum(k=0, n\3, binomial(3*n+k-1, k) * binomial(3*n+k, n-3*k));
for(n=0, M, print1(a(n),", "))

\\ a(n) == 0 (mod 3) for n>0.
for(n=1, 100, print1(a(n)%3, ", "))