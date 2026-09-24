apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = 4^n*apr(n, 5/2, 3/2);

for(n=0, 30, print1(a(n), ", "));

\\ a(n) = 4^n * binomial(5*n/2 + 1/2,n)/(n+1).
b(n) = 4^n * binomial(5*n/2 + 1/2,n) / (n + 1);
for(n=0, 30, print1(a(n)-b(n), ", "));
