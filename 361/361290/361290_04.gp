M=10;

T(n, k) = polcoef(x/(1-2*k*x+(k-1)*k*x^2+x*O(x^n)), n);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));
