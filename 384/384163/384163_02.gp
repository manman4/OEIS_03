M=16;

\\ a(n) = n! * [x^n] 1/(1 - 3*x)^(2*n/3).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( 1/(1 - 3*x)^(2*n/3), n);
for(n=0, M, print1(a(n),", "));
