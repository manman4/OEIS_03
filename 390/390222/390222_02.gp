\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+2,4*n-8*k+2).
b(n) = sum(k=0, n\2, binomial(k+2, 4*n-8*k+2));
for(n=0, 47, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+3,4*n-8*k+3).
a390040(n) = sum(k=0, n\2, binomial(k+3, 4*n-8*k+3));
\\ a(n) = A390040(n) - A390040(n-2).
for(n=0, 100, print1(a390040(n) - a390040(n-2) - b(n), ", "));