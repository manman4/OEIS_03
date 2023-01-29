M=24;

a(n) = sum(i=0, n, sum(j=i, n, (-1)^(i+j)*(i+j)!/(i!*j!))); 
for(n=0, M, print1(a(n),", "))
