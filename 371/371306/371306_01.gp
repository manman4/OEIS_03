a(n) = my(k=1); while(binomial(k^2, k)%n>0, k++); k;
for(n=1, 100, print1(a(n),", "))    