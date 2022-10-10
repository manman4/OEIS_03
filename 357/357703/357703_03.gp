M=21;

a(n) = round((prod(k=0, n-1, sqrt(3)+k)+prod(k=0, n-1, -sqrt(3)+k)))/2;
for(n=0, M, print1(a(n), ", "));