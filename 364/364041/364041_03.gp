b(n) = sumdiv(n, d, (d%2==1)*d^(d+1));

a(n) = if(n==0, 1, (1/n) * sum(k=1, n, b(k) * a(n-k)));

for(n=0, 15, print1(a(n), ", "))