M=22;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = if(n==0, 0, round((Bell_poly(n, sqrt(n)*I)-Bell_poly(n, -sqrt(n)*I))/(2*sqrt(n)*I)));
for(n=0, M, print1(a(n), ", "));