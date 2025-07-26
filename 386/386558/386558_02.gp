\\ G.f. of column k: (1/x) Series_Reverion( x*(1-x)^k ).
a(n, k) = my(x='x+O('x^(n+2))); polcoef(serreverse( x*(1-x)^k )/x, n);
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));