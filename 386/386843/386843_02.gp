\\ a(n) = [x^n] (1+x)^(2*n+2)/(1-x)^(n+1).
a(n) = polcoef( (1+x)^(2*n+2) / (1-x +x*O(x^n))^(n+1) , n);
for(n=0, 20, print1(a(n), ", "));


