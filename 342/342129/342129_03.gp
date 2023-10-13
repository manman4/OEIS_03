T(n, k) = round(sqrt(k)^n*polchebyshev(n, 2, sqrt(k)/2));
for(n=0, 11, for(k=0, n, print1(T(k, n-k), ", ")))