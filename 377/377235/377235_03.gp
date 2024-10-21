a(n) = if(n==0, 1, 3 * sum(k=0, n-1, (5-2*k/n) * a(k)));
for(n=0, 20, print1(a(n),", "))   