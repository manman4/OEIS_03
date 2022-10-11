M=23;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = round((Bell_poly(n, 3)+Bell_poly(n, -3)))/2;
for(n=0, M, print1(a(n), ", "));