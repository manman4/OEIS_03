\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(k,2*(n-2*k)).
b(n) = sum(k=0, n\2, (k+1) * binomial(k, 2*(n-2*k)));
for(n=0, 45, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) - 6*a(n-4) + 2*a(n-5) + 4*a(n-6) - 4*a(n-7) - a(n-8) + 2*a(n-9) - a(n-10).
a(n) = if(n<10, b(n), 4*a(n-2) - 6*a(n-4) + 2*a(n-5) + 4*a(n-6) - 4*a(n-7) - a(n-8) + 2*a(n-9) - a(n-10));
for(n=0, 25, print1(a(n)-b(n), ", "));

