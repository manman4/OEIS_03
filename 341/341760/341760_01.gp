M=90;

a(n) = my(k=2); while((n^k+k)%(n+k)!=0, k++); k;
for(n=0, M, print1(a(n), ", "));