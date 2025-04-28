M=25;

\\ a(n) = (n+1) * A049140(n+1).
a049140(n) = my(x='x+O('x^(n+1))); polcoef( serreverse( x*(1 - x - x^3) ), n);
a(n) = (n+1) * a049140(n+1);
for(n=0, M, print1(a(n), ", "))



