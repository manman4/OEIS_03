\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*k,2*n-6*k).
b(n) = sum(k=0, n\3, binomial(3*k, 2*n-6*k));
for(n=0, 39, print1(b(n), ", "));

\\ a(n) = 2*a(n-3) + 6*a(n-4) - a(n-6) + 3*a(n-7) - 3*a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 2*a(n-3) + 6*a(n-4) - a(n-6) + 3*a(n-7) - 3*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

