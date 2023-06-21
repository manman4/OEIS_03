a(n) = my(k=1); while (!(sum(j=1, k^(1/n), (k>j^n)*ispower(k-j^n, n+1))==2), k++); k;
for(n=1, 4, print1(a(n),", "))  

