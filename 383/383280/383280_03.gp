\\ a(n) = (n!)^2 * [x^n] 1/sqrt(1-x) * exp(3*x/2).
a(n) = my(x='x+O('x^(n+1))); (n!)^2 * polcoef( 1/sqrt(1-x) * exp(3*x/2), n);
for(n=0, 20, print1(a(n),", "))