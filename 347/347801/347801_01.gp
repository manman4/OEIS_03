M=90;

a(n) = sum(i=0, n, sum(j=0, n, (i^2+j^2==n)*(i*j)^2));
for(n=0, M, print1(a(n), ", "));