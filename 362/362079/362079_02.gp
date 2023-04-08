T(n, k) = polcoef(1/(1-1*x*(1+x)^n + x*O(x^n))^k, n);

for(n=0, 10, for(k=0, n, print1(T(k, n-k),", ")))  