M=25;

\\ a(n) = (1/(n+1)) * [x^n] ( (1-x)^2 * (1+x)^3 )^(n+1).
a(n) = (1/(n+1)) * polcoeff( ( (1-x)^2 * (1+x)^3 )^(n+1), n);
for(n=0, M, print1(a(n), ", "))

