M=12;

T(n, k) = if(k==0, 0^n, n!*polcoef(sum(j=0, n\k, (exp(x+x*O(x^n))-1)^(k*j)/(k*j)!), n));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));