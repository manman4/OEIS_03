M=24;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
a(n) = my(v=2^(1/3), w=(-1+sqrt(3)*I)/2); round((Bell_poly(n, v)+w*Bell_poly(n, v*w)+w^2*Bell_poly(n, v*w^2))/(3*v^2));
for(n=0, M, print1(a(n), ", "));