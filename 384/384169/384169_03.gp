M=17;

\\ a(n) = n! * [x^n] ( -log(1 - 4*x)/(4 * (1 - 4*x)^(n/4+1)) ).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( (-log(1 - 4*x))/(4 * (1 - 4*x)^(n/4+1)), n);
for(n=1, M, print1(a(n),", "));
