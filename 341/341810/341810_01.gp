a(n) = my(k=1); while (denominator(1/sum(j=1, n, 1/eulerphi(k*j))) != 1, k++); k;
for(n=1, 55, print(n, " ", a(n)))