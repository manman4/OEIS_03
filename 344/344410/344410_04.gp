M=24;

p(k, n) = n*((k-2)*n-k+4)/2;
a(n) = if(n==0, 1, sum(j=1, 3*n^2-2, p(3*n+2, j)));
for(n=0, M, print1(a(n), ", "));