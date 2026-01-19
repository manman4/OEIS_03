\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+1,3*n-6*k+1).
b(n) = sum(k=0, n\2, binomial(k+1, 3*n-6*k+1));
for(n=0, 49, print1(b(n), ", "));

\\ a(n) = 3*a(n-2) - 3*a(n-4) + a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 3*a(n-2) - 3*a(n-4) + a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

