\\ a(n) = n! * [x^n] (1+x) * Sum_{k=0..n} Stirling2(n+1,k+1) * log(1+x)^k.
a(n) = my(x='x+O('x^(n+1))); n! * polcoef((1+x) * sum(k=0, n, stirling(n+1,k+1,2) * log(1+x)^k ), n);
for(n=0, 20, print1(a(n), ", "););
