a(n) = if(n==0, 1, (-1/n) * sum(k=1, n, sumdiv(k, d, (-1)^(k/d)*d*(d^2+1)) * a(n-k)));
for(n=0, 20, print1(a(n),", "))  