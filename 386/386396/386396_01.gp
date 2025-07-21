M=49;

apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
a(n) = apr(n\7, 8, n%7+1);
for(n=0, M, print1(a(n),", "));

\\ For k=0..6, a(7*n+k) = (k+1) * binomial(8*n+k+1,n)/(8*n+k+1).
b(n) = (n%7+1) * binomial(8*(n\7)+n%7+1, n\7)/(8*(n\7)+n%7+1);
for(n=0, M, print1(a(n)-b(n),", "));