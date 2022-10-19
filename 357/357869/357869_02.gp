M=11;

T(n, k) = if(k==0, 0^n, n!*polcoef(exp((exp(x+x*O(x^n))-1)^k), n));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));