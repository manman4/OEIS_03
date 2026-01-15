\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+3,4*n-8*k+3).
b(n) = sum(k=0, n\2, binomial(k+3, 4*n-8*k+3));
for(n=0, 44, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

