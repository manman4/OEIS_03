M=22;

a(n) = (-1)^n*round((prod(k=0, n-1, sqrt(2)*I+k)+prod(k=0, n-1, -sqrt(2)*I+k)))/2;
for(n=0, M, print1(a(n), ", "));