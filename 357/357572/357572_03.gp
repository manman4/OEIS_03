M=23;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = round((Bell_poly(n, sqrt(3))-Bell_poly(n, -sqrt(3)))/(2*sqrt(3)));
for(n=0, M, print1(a(n), ", "));