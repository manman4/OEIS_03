M=20;

a(n) = sum(k=0, n, prod(i=1, k, prod(j=1, i, (n-k+i+j-1)/(i+j-1))));
for(n=0, M, print1(a(n), ", "))  