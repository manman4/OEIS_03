M=22;

a(n) = sum(i=0, n\2, sum(j=0, n\3, if(2*i+3*j<=n, (-1)^(i+j) * (n-i-2*j)!/(i!*j!), 0))); 
for(n=0, M, print1(a(n), ", "))