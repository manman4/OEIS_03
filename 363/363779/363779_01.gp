T(n, k) = polcoef(1/sum(j=0, n^(1/3), x^j^3)^k +x*O(x^n), n);
for(n=0, 11, for(k=0, n, print1(T(k,n-k),", ")))
