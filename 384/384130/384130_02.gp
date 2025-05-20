M=14;

\\ a(n) = (4*n)! * [x^(4*n)] (-log(1 - x))^(3*n) / (3*n)!.
a(n) = my(x='x+O('x^(4*n+1))); (4*n)! * polcoef( (-log(1 - x))^(3*n) / (3*n)!, 4*n);
for(n=0, M, print1(a(n),", "));
