M=17;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = 9^n*apr(n, 2, 2/3);
for(n=0, M, print1(a(n),", "));

\\ a(n) = 9^n * binomial((6*n+2)/3,n)/(3*n+1).
b(n) = 9^n * binomial((6*n+2)/3, n)/(3*n+1);
for(n=0, M, print1(a(n)-b(n),", "));