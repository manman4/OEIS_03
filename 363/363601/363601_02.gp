a(n) = if(n==0, 1, (1/n) * sum(k=1, n, (sigma(k, 3)-sigma(k)) * a(n-k)));
for(n=0, 20, print1(a(n),", "))  