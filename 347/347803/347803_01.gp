M=58;

a(n) = sum(i=0, n, sum(j=0, n, sum(k=0, n, sum(m=0, n, (i^2+j^2+k^2+m^2==n)*(i*j*k*m)^2))));
for(n=0, M, print1(a(n), ", "));