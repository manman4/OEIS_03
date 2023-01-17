M=18;

a(n) = my(x='x+O('x^(n+10))); polcoef(sum(k=1, n, (n * x)^k / (1 - (k * x)^k)), n); 
for(n=1, M, print1(a(n), ", "));