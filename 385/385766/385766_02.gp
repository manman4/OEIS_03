\\ a(0) = 1; a(n) = 1 + Sum_{i, j, k>=0 and i+j+k=n-1} (i+1) a(i) * a(j) * a(k).
a(n) = if(n==0, 1, 1 + sum(i=0, n-1, sum(j=0, n-1-i, (i+1) * a(i) * a(j) * a(n-1-i-j))));  
for(n=0, 10, print1(a(n),", "));   

