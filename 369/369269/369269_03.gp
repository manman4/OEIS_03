M=22;

\\ a(n) = (1/(n+1)) * [x^n] ( 1/(1-x)^2 * (1+x^3)^3 )^(n+1). 
a(n) = my(x='x+O('x^(n+2))); (1/(n+1)) * polcoef( ( 1/(1-x)^2 * (1+x^3)^3 )^(n+1), n);
for(n=0, M, print1(a(n), ", "))
