M=20;

a(n) = 2*polchebyshev(n, 1, (n+1)/2);
for(n=0, M, print1(a(n), ", "));