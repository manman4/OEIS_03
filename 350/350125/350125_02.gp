M=18;

a(n) = sum(k=1, n, k^2*sumdiv(k, d, (d^n-(d-1)^n)/d^2));
for(n=1, M, print1(a(n), ", "));