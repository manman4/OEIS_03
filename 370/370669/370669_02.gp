M=22;

\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (n-2*k)! * binomial(n-k-1,k).
b(n) = sum(k=0, n\2, (-1)^k * (n-2*k)! * binomial(n-k-1,k));
for(n=0, M, print1(b(n), ", "))

\\ a(n) = n*a(n-1) + (n-4)*a(n-3) + a(n-4) for n > 4.
a(n) = my(v=[1,1,2,5,20]); if(n>4, n*a(n-1) + (n-4)*a(n-3) + a(n-4), v[n+1]);
for(n=0, M, print1(a(n)-b(n), ", "))