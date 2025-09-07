\\ a(n) = (1/9) * Sum_{k=0..n-1} binomial(9*n,9*k+1).
b(n) = sum(k=0, n-1, binomial(9*n, 9*k+1)) / 9;
for(n=0, 13, print1(b(n),", "));

\\ a(n) = (1/9) * Sum_{k=0..n-1} binomial(9*n,9*k+8).
c(n) = sum(k=0, n-1, binomial(9*n, 9*k+8)) / 9;
for(n=0, 20, print1(b(n)-c(n),", "));

\\ a(n) = 265*a(n-1) + 139823*a(n-2) - 6826204*a(n-3) - 6965249*a(n-4) + 512*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 265*a(n-1) + 139823*a(n-2) - 6826204*a(n-3) - 6965249*a(n-4) + 512*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));
