M=10;

\\ G.f. of column k: 1/( 1 - Series_Reversion( x/(1+x)^k ) ).
a(n, k) = polcoef(1/( 1 - serreverse( x/(1+x)^k + x*O(x^(n+1))) ), n);
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 

  