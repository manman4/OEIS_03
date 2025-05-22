M=22;

\\ a(n) = n! * [x^n] (1 + 4*x)^(n/4).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( (1 + 4*x)^(n/4), n);
for(n=0, M, print1(a(n),", "));
