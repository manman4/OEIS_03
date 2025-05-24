M=16;

\\ a(n) = n! * [x^n] 1/(1 - x)^(3*n-1).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( 1/(1 - x)^(3*n-1), n);
for(n=0, M, print1(a(n),", "));
