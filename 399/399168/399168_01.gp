apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = 4^n*apr(n, 7/2, 1/2);

for(n=0, 30, print1(a(n), ", "));

\\ a(n) = 4^n * binomial(7*n/2 - 1/2,n)/(5*n+1).
b(n) = 4^n * binomial(7*n/2 - 1/2,n) / (5*n + 1);
for(n=0, 30, print1(a(n)-b(n), ", "));
