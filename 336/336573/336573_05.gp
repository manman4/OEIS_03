\\ G.f. of column k: 1 + Series_Reversion( x / ((1+x)^k * (1+2*x)) ).
T(n, k) = my(x='x+O('x^(n+5))); polcoeff( 1 + serreverse( x / ((1+x)^k * (1+2*x)) ), n); 
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")))    
