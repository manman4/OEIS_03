b(n) = sumdiv(n, d, ((d%7==0)+(d%7==2)+(d%7==5))*d);
a(n) = if(n==0, 1, -(1/n) * sum(k=1, n, b(k) * a(n-k)));
for(n=0, 27, print1(a(n), ", "))

