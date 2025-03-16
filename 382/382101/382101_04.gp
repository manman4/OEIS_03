M=10;

\\ E.g.f. of column k: exp( Series_Reversion( x/(1+x)^k ) ).
a(n, k) = n! * polcoef( exp(serreverse( x/(1+x)^k + x*O(x^(n+1))) ), n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", "))) 

  