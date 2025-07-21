M=19;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = apr(n, 7, 4);
for(n=0, M, print1(a(n),", "));

\\ a(n) = A386380(6*n+3).
a386380(n) = apr(n\6, 7, n%6+1);
for(n=0, 30, print1(a(n)-a386380(6*n+3), ", "));
