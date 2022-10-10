M=21;

a(n) = (prod(k=0, n-1, 2+k)+prod(k=0, n-1, -2+k))/2;
for(n=0, M, print1(a(n), ", "));