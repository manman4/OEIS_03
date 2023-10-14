a(n) = my(k=1); while (denominator(n/sum(j=1, n, 1/eulerphi(k*j))) != 1, k++); k;
for(n=1, 45, print1(a(n), ", "))