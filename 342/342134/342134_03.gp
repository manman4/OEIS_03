T(n, k) = round((-sqrt(k)*I)^n*polchebyshev(n, 2, sqrt(k)*I));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))