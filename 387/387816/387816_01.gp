\\ a(n) = Sum_{k=0..n} binomial(7*n+1,7*k).
b(n) = sum(k=0, n, binomial(7*n+1, 7*k));
for(n=0, 14, print1(b(n),", "));

\\ a(n) = 71*a(n-1) + 7585*a(n-2) - 36991*a(n-3) - 128*a(n-4).
a(n) = if(n<4, b(n), 71*a(n-1) + 7585*a(n-2) - 36991*a(n-3) - 128*a(n-4));
for(n=0, 20, print1(a(n)-b(n),", "));
