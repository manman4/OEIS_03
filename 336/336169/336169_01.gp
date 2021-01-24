T(n, k) = {x='x+O('x^(n+1)); polcoef(sum(j=0,n, (k*j)!/j!^k * x^j / (1+x)^(k*j+1)),n)};
matrix(7, 7, n, k, T(n-1, k-1))
