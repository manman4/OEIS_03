\\ a(n) = [x^n] (1 + x + x * (1 + x)^4)^n.
a(n) = polcoef( (1 + x + x * (1 + x)^4)^n, n);
for(n=0, 25, print1(a(n), ", "))

