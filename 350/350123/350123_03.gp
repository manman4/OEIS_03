M=43;

a(n) = sum(k=1, n, 2*k*sigma(k)-sigma(k, 2));
for(n=1, M, print1(a(n), ", "));