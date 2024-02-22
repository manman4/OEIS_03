M=22;

a(n) = sum(i=0, n, sum(j=0, n\2, sum(k=0, n\3, sum(l=0, n\4, if(i+2*j+3*k+4*l<=n, (-1)^(i+j+k+l) * (n-j-2*k-3*l)!/(i!*j!*k!*l!), 0))))); 
for(n=0, M, print1(a(n), ", "))