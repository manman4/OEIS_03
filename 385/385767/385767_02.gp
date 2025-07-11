\\ a(n) = 1 + Sum_{i, j, k, l>=0 and i+j+k+l=n-1} (i+1) a(i) * a(j) * a(k) * a(l).
a(n) = 1 + sum(i=0, n-1, sum(j=0, n-1-i, sum(k=0, n-1-i-j, (i+1) * a(i) * a(j) * a(k) * a(n-1-i-j-k))));  
for(n=0, 8, print1(a(n),", "));
