\\ a(n) = (1/7) * Sum_{k=0..n-1} binomial(7*n,7*k+2).
b(n) = sum(k=0, n-1, binomial(7*n, 7*k+2)) / 7;
for(n=0, 15, print1(b(n),", "));

\\ a(n) = (1/7) * Sum_{k=0..n-1} binomial(7*n,7*k+5).
c(n) = sum(k=0, n-1, binomial(7*n, 7*k+5)) / 7;
for(n=0, 20, print1(b(n)-c(n),", "));

\\ a(n) = 71*a(n-1) + 7585*a(n-2) - 36991*a(n-3) - 128*a(n-4) for n > 3.
a(n) = if(n<4, b(n), 71*a(n-1) + 7585*a(n-2) - 36991*a(n-3) - 128*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));
