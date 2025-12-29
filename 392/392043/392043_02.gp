\\ a(n) = Sum_{k=0..n} (k+1) * binomial(k,3*(n-k)).
b(n) = sum(k=0, n, (k+1) * binomial(k,3*(n-k)));
for(n=0, 33, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 13*a(n-4) + 5*a(n-6) - 2*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 6*a(n-1) - 15*a(n-2) + 20*a(n-3) - 13*a(n-4) + 5*a(n-6) - 2*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

