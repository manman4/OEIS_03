\\ a(n) = [x^n] 1/(1 - x * (1 + x^2)^n).
a(n) = polcoef(1/(1 - x * (1 + x^2)^n + O(x^(n+10))), n);
for(n=0, 27, print1(a(n),", "))
