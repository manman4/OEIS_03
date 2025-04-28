M=26;

\\ a(n) = (n+1) * A063021(n+1).
a063021(n) = my(x='x+O('x^(n+1))); polcoef( serreverse( x*(1 - x - x^4) ), n);
a(n) = (n+1) * a063021(n+1);
for(n=0, M, print1(a(n), ", "))



