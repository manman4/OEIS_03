M=23;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = round((Bell_poly(n, 2)-Bell_poly(n, -2)))/4;
for(n=0, M, print1(a(n), ", "));