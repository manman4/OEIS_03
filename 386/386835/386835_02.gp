\\ a(n) = [x^n] (1+x)^(2*n+2)/(1-x)^n.
a(n) = polcoef( (1+x)^(2*n+2) / (1-x +x*O(x^n))^n , n);
for(n=0, 20, print1(a(n), ", "));


