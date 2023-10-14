M=7;

a(n) = my(k=1, m=n^n); while(k*(k+1)/2%m!=0, k++); k;
for(n=1, M, print1(a(n), ", "));