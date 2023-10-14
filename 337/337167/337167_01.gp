a(n) = sum(k=0, n, 3^k*binomial(n, k)*(2*k)!/(k!*(k+1)!));
for(n=0, 20, print1(a(n), ", "))