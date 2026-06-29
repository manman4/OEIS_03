a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=4, m=2, c=0, r=0, s=-1 of the following family. 
for(n=0, 30, print1(a(4,2,0,0,-1,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));

for(n=0, 50, print1(a(4,2,0,0,-1,n)-b(4,2,0,0,-1,n),", "));

\\ a(n) = Sum_{k=0..n} binomial(4*n+2*k-3,k-1).
c(n) = sum(k=0, n, binomial(4*n+2*k-3,k-1));
for(n=0, 50, print1(a(4,2,0,0,-1,n)-c(n),", "));

\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(6*n+k-1,n-k-1).
d(n) = sum(k=0, n, (-1)^k * binomial(6*n+k-1,n-k-1));
\\ d(n) = sum(k=0, n, (-1)^k * C(6*n+k-1,n-k-1));
for(n=0, 50, print1(a(4,2,0,0,-1,n)-d(n),", "));

\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * binomial(6*n-k-2,n-2*k-1).
e(n) = sum(k=0, n\2, (-1)^k * binomial(6*n-k-2,n-2*k-1));
for(n=0, 50, print1(a(4,2,0,0,-1,n)-e(n),", "));