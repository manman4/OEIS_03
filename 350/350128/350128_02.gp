M=18;

a(n) = sum(k=1, n, 2*k*sigma(k, n-1)-sigma(k, n));
for(n=1, M, print1(a(n), ", "));