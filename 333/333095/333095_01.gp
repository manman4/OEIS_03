a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=4, m=2, c=2, r=1, s=1 of the following family. 
\\ for(n=0, 30, print1(a(4,2,2,1,1,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));
for(n=0, 30, print1(b(4,2,2,1,1,n),", "));

\\ for(n=0, 50, print1(a(4,2,2,1,1,n)-b(4,2,2,1,1,n),", "));

