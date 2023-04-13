M=20;

a(n) = n! * sum(i=0, n, sum(j=0, n-i, i^j * stirling(n-i-j,j,2)/(n-i-j)!));
for(n=0, M, print1(a(n), ", ")); 