M=11;

Bell_poly(n, x) = exp(-x)*suminf(k=0, k^n*x^k/k!);
T(n, k) = round((Bell_poly(n, sqrt(k))+Bell_poly(n, -sqrt(k))))/2;
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 