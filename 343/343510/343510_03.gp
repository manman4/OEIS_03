T(n, k) = sumdiv(n, d, moebius(n/d)*d*sigma(d, k-1));
for(n=1, 11, for(k=1, n, print1(T(k, n-k+1), ", ")))