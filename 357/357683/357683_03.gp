M=20;

a(n) = round((prod(k=0, n-1, sqrt(n)+k)+prod(k=0, n-1, -sqrt(n)+k)))/2;
for(n=0, M, print1(a(n), ", "));