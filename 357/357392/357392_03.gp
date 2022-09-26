M=17;

a(n) = if(n==0, 0, (3*n-1)!/(2*n)!);
for(n=0, M, print1(a(n), ", "));