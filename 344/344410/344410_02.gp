M=24;

p(k, n) = n*((k-2)*n-k+4)/2;
a(n) = p(3*n+2, 3*n^3-3*n+1);
for(n=0, M, print1(a(n), ", "));