\\ a(n) = Sum_{k=0..n} binomial(2*n,5*k).
b(n) = sum(k=0, n, binomial(2*n, 5*k));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 5*a(n-1) - 10*a(n-2) + 20*a(n-3) + 15*a(n-4) + 4*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 5*a(n-1) - 10*a(n-2) + 20*a(n-3) + 15*a(n-4) + 4*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));

\\ a(2*n) = A387848(n).
for(n=0, 26, print1(b(2*n),", "));
