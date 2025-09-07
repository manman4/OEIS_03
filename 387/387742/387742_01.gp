\\ a(n) = (1/2) * Sum_{k=0..n-1} binomial(4*n,4*k+2).
b(n) = sum(k=0, n-1, binomial(4*n, 4*k+2))/2;
for(n=0, 19, print1(b(n),", "));

\\ a(n) = 12*a(n-1) + 64*a(n-2) for n > 2.
a(n) = if(n<3, b(n), 12*a(n-1) + 64*a(n-2));
for(n=0, 25, print1(a(n)-b(n),", "));

print("以下はダメ")
\\ a(n) = 12*a(n-1) + 64*a(n-2) for n > 1.
a(n) = if(n<2, b(n), 12*a(n-1) + 64*a(n-2));
for(n=0, 25, print1(a(n)-b(n),", "));
