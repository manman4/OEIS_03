b(n) = sumdiv(n, d, ((d%7==0)+(d%7==3)+(d%7==4))*d);
a(n) = if(n==0, 1, -(1/n) * sum(k=1, n, b(k) * a(n-k)));
for(n=0, 24, print1(a(n), ", "))

