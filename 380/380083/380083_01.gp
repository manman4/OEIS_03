\\ a(n) = Pell(n^2)/Pell(n).
pell(n) = ([2, 1; 1, 0]^n)[2, 1];
a(n) = pell(n^2)/pell(n);
for(n=1, 12, print1(a(n),", "))