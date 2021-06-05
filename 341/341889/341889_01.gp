M=94;

a(n) = if(n==2, 0, my(k=2); while((n^n+k)%(n+k)!=0, k++); k);
for(n=1, M, print1(a(n), ", "));