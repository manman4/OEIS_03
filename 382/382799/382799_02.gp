\\ A(n,k) = n! * k! * [x^n * y^k] 1 / (1 - log(1-x) * log(1-y))^2.
a(n, k) = my(x='x+O('x^(n+1)), y='y+O('y^(k+1))); n!*k!*polcoef(polcoef( 1 / (1 - log(1-x) * log(1-y))^2, n), k);
for(n=0, 20, for(k=0, n, print1(a(k, n-k)", ")));