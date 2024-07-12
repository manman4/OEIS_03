a(n) = my(x='x+O('x^(n+1))); polcoef(sum(k=-sqrtint(n), sqrtint(n), x^k^2)^8, n);
for(n=0, 50, print1(a(n^2),", "))        
 