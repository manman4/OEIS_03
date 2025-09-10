\\ a(n) = Sum_{k=0..n} binomial(5*n,6*k).
b(n) = sum(k=0, n, binomial(5*n, 6*k));
for(n=0, 17, print1(b(n),", "));

\\ a(n) = 6*a(n-1) + 615*a(n-2) + 7160*a(n-3) - 7155*a(n-4) + 7776*a(n-5) for n > 5.
a(n) = if(n<6, b(n), 6*a(n-1) + 615*a(n-2) + 7160*a(n-3) - 7155*a(n-4) + 7776*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));

print("以下はダメ")
\\ a(n) = 6*a(n-1) + 615*a(n-2) + 7160*a(n-3) - 7155*a(n-4) + 7776*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 6*a(n-1) + 615*a(n-2) + 7160*a(n-3) - 7155*a(n-4) + 7776*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));
