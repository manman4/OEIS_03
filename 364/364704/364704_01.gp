b(n) = my(k=n); while(sigma(k)<sigma(k+1), k++); k-n+1;
a(n) = my(k=1); while(b(k)<n, k++); k;

for(n=1, 6, print1(a(n), ", "));