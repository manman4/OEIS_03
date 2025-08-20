\\ G.f. of column k: (1/x) * Series_Reversion( x/(1+(2*k+1)*x+(k*x)^2) ).
a(n, k) = my(x='x+O('x^(n+3))); polcoef(serreverse(x/(1+(2*k+1)*x+(k*x)^2))/x, n);
for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));