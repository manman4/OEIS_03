M=16;

\\ a(n) = (2*n)! * [x^(2*n)] 1/(1 - x)^(3*n).
a(n) = my(x='x+O('x^(2*n+1))); (2*n)! * polcoef( 1/(1 - x)^(3*n), 2*n);
for(n=0, M, print1(a(n),", "));
