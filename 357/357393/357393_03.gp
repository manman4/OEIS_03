M=17;

a(n) = if(n==0, 0, (4*n-1)!/(3*n)!);
for(n=0, M, print1(a(n), ", "));