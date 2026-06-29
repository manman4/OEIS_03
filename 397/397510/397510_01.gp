a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=1, m=2, c=0, r=1, s=0 of the following family. 
for(n=0, 30, print1(a(1,2,0,1,0,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));

for(n=0, 50, print1(a(1,2,0,1,0,n)-b(1,2,0,1,0,n),", "));

\\ a(n) = Sum_{k=0..n} binomial(n+2*k-3,k).
c(n) = sum(k=0, n, binomial(n+2*k-3,k));
for(n=0, 50, print1(a(1,2,0,1,0,n)-c(n),", "));

\\ a(n) = Sum_{k=0..n} (-1)^k * binomial(3*n+k-1,n-k).
d(n) = sum(k=0, n, (-1)^k * binomial(3*n+k-1,n-k));
for(n=0, 50, print1(a(1,2,0,1,0,n)-d(n),", "));

\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * binomial(3*n-k-2,n-2*k).
e(n) = sum(k=0, n\2, (-1)^k * binomial(3*n-k-2,n-2*k));
for(n=0, 50, print1(a(1,2,0,1,0,n)-e(n),", "));