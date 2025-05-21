M=16;

\\ a(n) = n! * [x^n] 1/(1 - 2*x)^(3*n/2).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( 1/(1 - 2*x)^(3*n/2), n);
for(n=0, M, print1(a(n),", "));
