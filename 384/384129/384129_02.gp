M=14;

\\ a(n) = (3*n)! * [x^(3*n)] log(1 - x)^(2*n) / (2*n)!.
a(n) = my(x='x+O('x^(3*n+1))); (3*n)! * polcoef( (-log(1 - x))^(2*n) / (2*n)!, 3*n);
for(n=0, M, print1(a(n),", "));
