M=11;

T(n, k) = if(k==0, 0^n, n!*polcoef(1/(1-(-log(1-x+x*O(x^n)))^k), n));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));