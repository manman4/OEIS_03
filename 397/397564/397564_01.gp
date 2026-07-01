a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=3, m=3, c=4, r=1, s=0 of the following family.
for(n=0, 30, print1(a(3,3,4,1,0,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));

for(n=0, 50, print1(a(3,3,4,1,0,n)-b(3,3,4,1,0,n),", "));

\\ a(n) = Sum_{k=0..n} binomial(3*n+3*k,k).
c(n) = sum(k=0, n, binomial(3*n+3*k,k));
for(n=0, 50, print1(a(3,3,4,1,0,n)-c(n),", "));

