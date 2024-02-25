M=22;

\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * (n-3*k)! * binomial(n-2*k-1,k).
b(n) = sum(k=0, n\3, (-1)^k * (n-3*k)! * binomial(n-2*k-1,k));
for(n=0, M, print1(b(n), ", "))

\\ a(n) =  n*a(n-1) + a(n-3) + (n-6)*a(n-4) + 2*a(n-6) for n > 6.
a(n) = my(v=[1,1,2,6,23,116,702]); if(n>6, n*a(n-1) + a(n-3) + (n-6)*a(n-4) + 2*a(n-6), v[n+1]);
for(n=0, M, print1(a(n)-b(n), ", "))