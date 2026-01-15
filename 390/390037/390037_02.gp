\\ a(n) = Sum_{k=0..floor(n/3)} binomial(k+2,3*n-9*k+2).
b(n) = sum(k=0, n\3, binomial(k+2, 3*n-9*k+2));
for(n=0, 55, print1(b(n), ", "));

\\ a(n) = 3*a(n-3) - 3*a(n-6) + a(n-9) + a(n-10).
a(n) = if(n<10, b(n), 3*a(n-3) - 3*a(n-6) + a(n-9) + a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

