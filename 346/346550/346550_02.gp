M=21;

a(n) = if(n<3, 2^n, (n-2)*a(n-1)+2*(n-1)*a(n-2)+(n-2)*a(n-3));
for(n=0, M, print1(a(n), ", "));