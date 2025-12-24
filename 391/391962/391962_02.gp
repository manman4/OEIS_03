\\ a(n) = Sum_{k=0..n} (k+1) * binomial(k,2*(n-k)).
b(n) = sum(k=0, n, (k+1) * binomial(k,2*(n-k)));
for(n=0, 35, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 6*a(n-3) - 5*a(n-4) + 2*a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 4*a(n-1) - 6*a(n-2) + 6*a(n-3) - 5*a(n-4) + 2*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

