M=14;

a(n) = if(n==0, 1, n*a(n-1)+n^(2*n));
for(n=0, M, print1(a(n), ", "));