\\ a(n) = Sum_{k=0..n} binomial(5*n+4,5*k+1).
b(n) = sum(k=0, n, binomial(5*n+4, 5*k+1));
for(n=0, 16, print1(b(n),", "));

\\ a(n) = 21*a(n-1) + 353*a(n-2) - 32*a(n-3).
a(n) = if(n<3, b(n), 21*a(n-1) + 353*a(n-2) - 32*a(n-3));
for(n=0, 20, print1(a(n)-b(n),", "));
