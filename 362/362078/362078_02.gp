T(n, k) = polcoef(1/(1-1*x*(1+x)^k + x*O(x^n))^n, n);

for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))  