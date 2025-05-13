\\ a(n) = [x^n] 1/Product_{k=0..n} (1 - (n+k)*x).
a(n) = my(x='x+O('x^(n+1))); polcoef(1/prod(k=0, n, (1-(n+k)*x)), n);
for(n=0, 20, print1(a(n),", "));

