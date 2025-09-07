\\ a(n) = (1/8) * Sum_{k=0..n-1} binomial(8*n,8*k+1).
b(n) = sum(k=0, n-1, binomial(8*n, 8*k+1)) / 8;
for(n=0, 14, print1(b(n),", "));

\\ a(n) = (1/8) * Sum_{k=0..n-1} binomial(8*n,8*k+7).
c(n) = sum(k=0, n-1, binomial(8*n, 8*k+7)) / 8;
for(n=0, 20, print1(b(n)-c(n),", "));

\\ a(n) = 120*a(n-1) + 34800*a(n-2) + 4096*a(n-3) for n > 3.
a(n) = if(n<4, b(n), 120*a(n-1) + 34800*a(n-2) + 4096*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));

print("以下はダメ")
\\ a(n) = 120*a(n-1) + 34800*a(n-2) + 4096*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 120*a(n-1) + 34800*a(n-2) + 4096*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
