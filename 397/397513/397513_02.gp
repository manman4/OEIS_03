a(l,m,c,r,s,n) = sum(k=0, n, (r*binomial(l*n+m*k-m-1+c,k) - s*binomial(l*n+m*k-m-1+c,k-1)));
\\ This is the special case l=5, m=2, c=0, r=1, s=0 of the following family. 
for(n=0, 30, print1(a(5,2,0,1,0,n),", "));

C(n, k) = if(k>=0, binomial(n, k));
b(l,m,c,r,s,n) = sum(k=0, n, (r*C(l*n+m*k-m-1+c,k) - s*C(l*n+m*k-m-1+c,k-1)));

for(n=0, 50, print1(a(5,2,0,1,0,n)-b(5,2,0,1,0,n),", "));