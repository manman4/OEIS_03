\\ a(n) = (1/4) * Sum_{k=0..n-1} binomial(6*n,6*k+3).
b(n) = sum(k=0, n-1, binomial(6*n, 6*k+3)) / 4;
for(n=0, 16, print1(b(n),", "));

\\ a(n) = 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3) for n > 3.
a(n) = if(n<4, b(n), 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));

print("以下はダメ")
\\ a(n) = 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 38*a(n-1) + 1691*a(n-2) - 1728*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
