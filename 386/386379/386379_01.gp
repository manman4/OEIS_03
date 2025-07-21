M=43;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = apr(n\5, 6, n%5+1);
for(n=0, M, print1(a(n),", "));

\\ For k=0..4, a(5*n+k) = (k+1) * binomial(6*n+k+1,n)/(6*n+k+1).
b(n) = (n%5+1) * binomial(6*(n\5)+n%5+1, n\5)/(6*(n\5)+n%5+1);
for(n=0, M, print1(a(n)-b(n),", "));