M=14;

T(n, k) = polcoef(sum(i=1, n, x^((k+1)*i-1)*prod(j=1, i-1, (1-x^(k*i+j-1))/(1-x^j+x*O(x^n)))), n);
for(n=1, M, for(k=1, n, print1(T(n, k), ", ")));