\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(k,3*(n-2*k)).
b(n) = sum(k=0, n\2, (k+1) * binomial(k, 3*(n-2*k)));
for(n=0, 46, print1(b(n), ", "));

\\ a(n) = 6*a(n-2) - 15*a(n-4) + 20*a(n-6) + 2*a(n-7) - 15*a(n-8) - 6*a(n-9) + 6*a(n-10) + 6*a(n-11) - a(n-12) - 2*a(n-13) - a(n-14).
a(n) = if(n<14, b(n), 6*a(n-2) - 15*a(n-4) + 20*a(n-6) + 2*a(n-7) - 15*a(n-8) - 6*a(n-9) + 6*a(n-10) + 6*a(n-11) - a(n-12) - 2*a(n-13) - a(n-14));
for(n=0, 25, print1(a(n)-b(n), ", "));

