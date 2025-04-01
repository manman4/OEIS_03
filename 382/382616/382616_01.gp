M=26;

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(n+2*k-1,n-k).
b(n) = sum(k=0, n, (k+1)*binomial(n+2*k-1,n-k));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6) for n > 6.
\\ 以下はダメ
a(n) = if(n<6, b(n), 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n),", "));
\\ 以下はOK
a(n) = if(n<7, b(n), 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n),", "));


