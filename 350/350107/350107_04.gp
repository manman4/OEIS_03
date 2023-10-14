M=48;

a(n) = sum(k=1, n, 2*k*numdiv(k)-sigma(k));
for(n=1, M, print1(a(n), ", "));