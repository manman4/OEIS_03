M=47;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = apr(n\6, 7, n%6+1);
for(n=0, M, print1(a(n),", "));

\\ For k=0..5, a(6*n+k) = (k+1) * binomial(7*n+k+1,n)/(7*n+k+1).
b(n) = (n%6+1) * binomial(7*(n\6)+n%6+1, n\6)/(7*(n\6)+n%6+1);
for(n=0, M, print1(a(n)-b(n),", "));