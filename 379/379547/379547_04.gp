\\ a(n) = (1/(n+1)) * [x^n] ( (1+x)^2 * (1+2*x)^4 )^(n+1).
a(n) = 1/(n+1) * polcoef( ( (1+x)^2 * (1+2*x)^4 )^(n+1), n);
for(n=0, 15, print1(a(n),", "))  