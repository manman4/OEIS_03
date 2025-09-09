\\ a(n) = Sum_{k=0..n} binomial(6*n+1,6*k).
b(n) = sum(k=0, n, binomial(6*n+1, 6*k));
for(n=0, 16, print1(b(n),", "));

\\ a(n) = 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3).
a(n) = if(n<3, b(n), 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3));
for(n=0, 20, print1(a(n)-b(n),", "));
