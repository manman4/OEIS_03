\\ a(n) = Sum_{k=0..floor(n/6)} (k+1) * binomial(n-4*k+3,n-6*k).
b(n) = sum(k=0, n\6, (k+1) * binomial(n-4*k+3, n-6*k));
for(n=0, 39, print1(b(n), ", "));

\\ a(n) = 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + 2*a(n-6) - 4*a(n-7) + 2*a(n-8) - a(n-12).
a(n) = if(n<12, b(n), 4*a(n-1) - 6*a(n-2) + 4*a(n-3) - a(n-4) + 2*a(n-6) - 4*a(n-7) + 2*a(n-8) - a(n-12));
for(n=0, 20, print1(a(n)-b(n), ", "));

