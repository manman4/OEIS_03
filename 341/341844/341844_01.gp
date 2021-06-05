M=91;

a(n) = my(k=2); while((n+k^n)%(n+k)!=0, k++); k;
for(n=1, M, print1(a(n), ", "));