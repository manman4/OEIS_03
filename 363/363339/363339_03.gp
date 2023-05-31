a(n) = if(n==0, 1, (1/n) * sum(k=1, n, sumdiv(k, d, (d%4==1) * (-1)^(k/d+1) * d * a(d\4) ) * a(n-k)));
for(n=0, 26, print1(a(n),", "))

