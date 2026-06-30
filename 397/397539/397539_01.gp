a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=2, m=4, c=0, r=1, s=0 of the following family.
for(n=0, 30, print1(a(2,4,0,1,0,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));

for(n=0, 50, print1(a(2,4,0,1,0,n)-b(2,4,0,1,0,n),", "));

\\ a(n) = Sum_{k=0..n} binomial(2*n+4*k-5,k).
c(n) = sum(k=0, n, binomial(2*n+4*k-5,k));
for(n=0, 50, print1(a(2,4,0,1,0,n)-c(n),", "));

