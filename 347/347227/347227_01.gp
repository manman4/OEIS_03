M=11;

T(n, k) = sumdiv(n, d, moebius(d)*moebius(n/d)*d^k);
for(n=1, M, for(k=1, n, print1(T(k, n-k),", ")));