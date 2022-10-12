M=24;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = round((Bell_poly(n, sqrt(2)*I)+Bell_poly(n, -sqrt(2)*I)))/2;
for(n=0, M, print1(a(n), ", "));