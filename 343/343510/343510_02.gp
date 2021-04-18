T(n, k) = sumdiv(n, d, eulerphi(n/d)*d^k);
for(n=1, 11, for(k=1, n, print1(T(k, n-k+1), ", ")))