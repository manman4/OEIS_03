\\ a(n) = (1/5) * Sum_{k=0..n-1} binomial(5*n,5*k+2).
b(n) = sum(k=0, n-1, binomial(5*n, 5*k+2)) / 5;
for(n=0, 17, print1(b(n),", "));

\\ a(n) = (1/5) * Sum_{k=0..n-1} binomial(5*n,5*k+3).
c(n) = sum(k=0, n-1, binomial(5*n, 5*k+3)) / 5;
for(n=0, 20, print1(b(n)-c(n),", "));

\\ a(n) = 21*a(n-1) + 353*a(n-2) - 32*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 21*a(n-1) + 353*a(n-2) - 32*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
